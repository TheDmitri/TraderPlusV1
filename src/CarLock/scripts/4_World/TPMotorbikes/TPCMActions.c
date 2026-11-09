#ifndef CARLOCKDISABLE
class TPCMMenu extends UIScriptedMenu
{
    MotorbikeScript m_Bike;
    EditBoxWidget m_Input;
    TextWidget m_Output;
    ButtonWidget m_OK;
    ButtonWidget m_Cancel;
    void TPCMMenu(MotorbikeScript bike) { m_Bike=bike; }
    override Widget Init()
    {
        if (layoutRoot) return layoutRoot;
        layoutRoot=GetGame().GetWorkspace().CreateWidgets("CarLock/gui/CarLockUI.layout");
        m_Input=EditBoxWidget.Cast(layoutRoot.FindAnyWidget("PasswordInput"));
        m_Output=TextWidget.Cast(layoutRoot.FindAnyWidget("TextOutPut"));
        m_OK=ButtonWidget.Cast(layoutRoot.FindAnyWidget("BttnOK"));
        m_Cancel=ButtonWidget.Cast(layoutRoot.FindAnyWidget("BttnCancel"));
        m_OK.SetText("SET CAR PASSWORD");
        return layoutRoot;
    }
    override void OnShow()
    {
        super.OnShow();
        if (layoutRoot) layoutRoot.Show(true);
        PPEffects.SetBlurMenu(0.5);
        GetGame().GetMission().GetHud().Show(false);
        GetGame().GetMission().AddActiveInputExcludes({"menu"});
        if (m_Bike && !m_Bike.m_TPCMHasPassword) m_Output.SetText("DEFINE PASSWORD (8)");
        else m_Output.SetText("ENTER PASSWORD (8)");
        SetFocus(m_Input);
    }
    override void OnHide()
    {
        if (layoutRoot) layoutRoot.Show(false);
        SetFocus(NULL);
        PPEffects.SetBlurMenu(0);
        if (GetGame().GetMission()) GetGame().GetMission().GetHud().Show(true);
        if (GetGame().GetMission()) GetGame().GetMission().RemoveActiveInputExcludes({"menu"},true);
        super.OnHide();
    }
    override bool OnClick(Widget w,int x,int y,int button)
    {
        if (w==m_Cancel) { Close(); return true; }
        if (w!=m_OK) return false;
        string password=m_Input.GetText();
        if (password!="" && !TPCM.Password(password)) { m_Output.SetText("8 DIGITS ONLY"); return true; }
        GetRPCManager().SendRPC("TPMotorbikeCarLock","TPCMRequest",new Param2<MotorbikeScript,string>(m_Bike,password),true,NULL);
        Close();
        return true;
    }
};
class ActionTPCMMenu: ActionInteractBase
{
    void ActionTPCMMenu() { m_CommandUID=DayZPlayerConstants.CMD_ACTIONMOD_INTERACTONCE; m_StanceMask=DayZPlayerConstants.STANCEMASK_ERECT | DayZPlayerConstants.STANCEMASK_CROUCH; }
    override void CreateConditionComponents() { m_ConditionTarget=new CCTObject(4); m_ConditionItem=new CCINone; }
    override string GetText() { return "Open Lock Menu"; }
    override bool ActionCondition(PlayerBase player,ActionTarget target,ItemBase item)
    {
        MotorbikeScript bike=TPCM.Target(target);
        return bike && bike.m_TPCMInstalled && !bike.m_TPCMFault;
    }
    override void OnStartClient(ActionData action_data)
    {
        MotorbikeScript bike=TPCM.Target(action_data.m_Target);
        if (!bike || GetGame().GetUIManager().GetMenu()) return;
        action_data.m_Player.m_TPCMMenu=new TPCMMenu(bike);
        GetGame().GetUIManager().ShowScriptedMenu(action_data.m_Player.m_TPCMMenu,NULL);
    }
};
class ActionTPCMInstallCB: ActionContinuousBaseCB
{
    override void CreateActionComponent() { m_ActionData.m_ActionComponent=new CAContinuousTime(15); }
};
class ActionTPCMInstall: ActionContinuousBase
{
    void ActionTPCMInstall()
    {
        m_CallbackClass=ActionTPCMInstallCB; m_CommandUID=DayZPlayerConstants.CMD_ACTIONFB_CRAFTING;
        m_FullBody=true; m_StanceMask=DayZPlayerConstants.STANCEMASK_CROUCH;
    }
    override void CreateConditionComponents() { m_ConditionTarget=new CCTNonRuined(4); m_ConditionItem=new CCINonRuined; }
    override string GetText() { return "Attach CarLock"; }
    override bool ActionCondition(PlayerBase player,ActionTarget target,ItemBase item)
    {
        MotorbikeScript bike=TPCM.Target(target);
        return bike && !bike.m_TPCMInstalled && !bike.m_TPCMFault && TPCM.Empty(bike) && item && item.IsKindOf("CarLock");
    }
    override void OnFinishProgressServer(ActionData action_data)
    {
        MotorbikeScript bike=TPCM.Target(action_data.m_Target);
        if (!TPCM.Near(action_data.m_Player,bike) || !bike.m_TPCMReady || !ActionCondition(action_data.m_Player,action_data.m_Target,action_data.m_MainItem)) return;
        if (action_data.m_Player.GetItemInHands()!=action_data.m_MainItem || action_data.m_MainItem.IsRuined()) return;
        TPCMRecord record=new TPCMRecord;
        record.Type=bike.GetType(); record.Owner=action_data.m_Player.GetIdentity().GetPlainId(); record.Locked=true;
        if (!TPCM.Commit(bike,record)) { TPCM.Notify(action_data.m_Player,"Falha ao salvar trava; item preservado."); return; }
        action_data.m_MainItem.Delete();
        TPCM.Notify(action_data.m_Player,"CarLock instalado. Defina a senha.");
        GetRPCManager().SendRPC("TPMotorbikeCarLock","TPCMOpenMenu",new Param1<MotorbikeScript>(bike),true,action_data.m_Player.GetIdentity());
    }
};
class ActionTPCMRemove: ActionTPCMInstall
{
    override string GetText() { return "Remove Motorbike CarLock (owner)"; }
    override bool ActionCondition(PlayerBase player,ActionTarget target,ItemBase item)
    {
        MotorbikeScript bike=TPCM.Target(target);
        if (!bike || !bike.m_TPCMInstalled || bike.m_TPCMLocked || bike.m_TPCMFault || !TPCM.Empty(bike) || !item || !item.IsKindOf("Pliers")) return false;
        if (GetGame().IsServer()) return TPCM.Owner(player,bike);
        return true;
    }
    override void OnFinishProgressServer(ActionData action_data)
    {
        MotorbikeScript bike=TPCM.Target(action_data.m_Target);
        if (!TPCM.Near(action_data.m_Player,bike) || !ActionCondition(action_data.m_Player,action_data.m_Target,action_data.m_MainItem)) return;
        if (action_data.m_Player.GetItemInHands()!=action_data.m_MainItem || action_data.m_MainItem.IsRuined()) return;
        // Prepare the returned item before changing the durable state.
        ItemBase device=ItemBase.Cast(GetGame().CreateObjectEx("CarLock",action_data.m_Player.GetPosition(),ECE_PLACE_ON_SURFACE));
        if (!device) return;
        TPCMRecord record=new TPCMRecord; record.Type=bike.GetType();
        if (!TPCM.Commit(bike,record)) { device.Delete(); return; }
        action_data.m_MainItem.AddHealth("","Health",-10);
        TPCM.Notify(action_data.m_Player,"CarLock removido; item no chao.");
    }
};
class ActionTPCMLockCB: ActionContinuousBaseCB
{
    override void CreateActionComponent() { m_ActionData.m_ActionComponent=new CAContinuousTime(1); }
};
class ActionTPCMLock: ActionContinuousBase
{
    void ActionTPCMLock()
    {
        m_CallbackClass=ActionTPCMLockCB;
        m_CommandUID=DayZPlayerConstants.CMD_ACTIONMOD_INTERACTONCE;
        m_StanceMask=DayZPlayerConstants.STANCEMASK_ERECT | DayZPlayerConstants.STANCEMASK_CROUCH;
    }
    bool WantLocked() { return true; }
    override void CreateConditionComponents() { m_ConditionTarget=new CCTObject(4); m_ConditionItem=new CCINone; }
    override string GetText() { return "Lock Motorbike"; }
    override bool ActionCondition(PlayerBase player,ActionTarget target,ItemBase item)
    {
        MotorbikeScript bike=TPCM.Target(target);
        return bike && bike.m_TPCMInstalled && bike.m_TPCMHasPassword && !bike.m_TPCMFault && bike.m_TPCMLocked!=WantLocked();
    }
    override void OnStartServer(ActionData action_data)
    {
        MotorbikeScript bike=TPCM.Target(action_data.m_Target);
        if (!TPCM.Near(action_data.m_Player,bike) || !bike.m_TPCMReady || !ActionCondition(action_data.m_Player,action_data.m_Target,action_data.m_MainItem)) return;
        if (!TPCM.Owner(action_data.m_Player,bike))
        {
            GetRPCManager().SendRPC("TPMotorbikeCarLock","TPCMOpenMenu",new Param1<MotorbikeScript>(bike),true,action_data.m_Player.GetIdentity());
            return;
        }
        if (!TPCM.Empty(bike)) { TPCM.Notify(action_data.m_Player,"Desligue, pare e desocupe a moto."); return; }
        TPCMRecord record=TPCM.Copy(bike.m_TPCMRecord);
        record.Locked=WantLocked();
        if (!TPCM.Commit(bike,record)) { TPCM.Notify(action_data.m_Player,"Falha ao salvar; operacao cancelada."); return; }
        if (record.Locked) { bike.TPCMSound(1); TPCM.Notify(action_data.m_Player,"Trava salva: bloqueada."); }
        else { bike.TPCMSound(2); TPCM.Notify(action_data.m_Player,"Trava salva: desbloqueada."); }
    }
};
class ActionTPCMUnlock: ActionTPCMLock
{
    override bool WantLocked() { return false; }
    override string GetText() { return "Unlock Motorbike"; }
};
class TPCMPickData: ActionData
{
    ref TPCMRecord OriginalLock;
};
class ActionTPCMPickCB: ActionContinuousBaseCB
{
    override void CreateActionComponent() { m_ActionData.m_ActionComponent=new CAContinuousTime(90); }
};
class ActionTPCMPick: ActionTPCMInstall
{
    void ActionTPCMPick() { m_CallbackClass=ActionTPCMPickCB; }
    override ActionData CreateActionData() { return new TPCMPickData; }
    override string GetText() { return "Lockpick Motorbike CarLock"; }
    override bool ActionCondition(PlayerBase player,ActionTarget target,ItemBase item)
    {
        MotorbikeScript bike=TPCM.Target(target);
        return bike && bike.m_TPCMInstalled && !bike.m_TPCMFault && TPCM.Empty(bike) && player.GetSafeZoneStatus()!=SZ_IN_SAFEZONE && item && item.IsKindOf("CarLockPick");
    }
    override void OnStartServer(ActionData action_data)
    {
        if (!ActionCondition(action_data.m_Player,action_data.m_Target,action_data.m_MainItem)) return;
        MotorbikeScript bike=TPCM.Target(action_data.m_Target);
        if (!TPCM.Near(action_data.m_Player,bike)) return;
        TPCMPickData pick=TPCMPickData.Cast(action_data);
        pick.OriginalLock=bike.m_TPCMRecord;
        bike.TPCMSound(3);
    }
    override void OnFinishProgressServer(ActionData action_data)
    {
        MotorbikeScript bike=TPCM.Target(action_data.m_Target);
        if (!TPCM.Near(action_data.m_Player,bike) || !ActionCondition(action_data.m_Player,action_data.m_Target,action_data.m_MainItem)) return;
        TPCMPickData pick=TPCMPickData.Cast(action_data);
        if (!pick || !pick.OriginalLock || pick.OriginalLock!=bike.m_TPCMRecord) return;
        if (action_data.m_Player.GetItemInHands()!=action_data.m_MainItem || action_data.m_MainItem.IsRuined()) return;
        if (GetTraderPlusConfigServer().LockPickChance>Math.RandomFloat01())
        {
            TPCMRecord record=new TPCMRecord; record.Type=bike.GetType();
            if (!TPCM.Commit(bike,record)) return;
            bike.TPCMSound(2);
            TPCM.Notify(action_data.m_Player,"CarLock removido por arrombamento.");
        }
        else TPCM.Notify(action_data.m_Player,"Arrombamento falhou.");
        action_data.m_MainItem.Delete();
    }
};
modded class ActionConstructor
{
    override void RegisterActions(TTypenameArray actions)
    {
        super.RegisterActions(actions); actions.Insert(ActionTPCMMenu); actions.Insert(ActionTPCMInstall); actions.Insert(ActionTPCMRemove); actions.Insert(ActionTPCMPick); actions.Insert(ActionTPCMLock); actions.Insert(ActionTPCMUnlock);
    }
};
modded class PlayerBase
{
    ref TPCMMenu m_TPCMMenu;
    override void SetActions() { super.SetActions(); AddAction(ActionTPCMMenu); AddAction(ActionTPCMLock); AddAction(ActionTPCMUnlock); }
};
modded class CarLock
{
    override void SetActions() { super.SetActions(); AddAction(ActionTPCMInstall); }
};
modded class Pliers
{
    override void SetActions() { super.SetActions(); AddAction(ActionTPCMRemove); }
};
modded class CarLockPick
{
    override void SetActions() { super.SetActions(); AddAction(ActionTPCMPick); }
};
#endif
