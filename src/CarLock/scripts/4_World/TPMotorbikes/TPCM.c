#ifndef CARLOCKDISABLE
class TPCM
{
    static bool Supports(MotorbikeScript bike)
    {
        if (!bike) return false;
        switch (bike.GetType())
        {
            case "Motorbike_01_Blue": case "Motorbike_01_Red": case "Motorbike_01_Yellow":
            case "Motorbike_02_Blue": case "Motorbike_02_Green": case "Motorbike_02_Red": case "Motorbike_02_Yellow": return true;
        }
        return false;
    }
    static string Directory()
    {
        string dir="$profile:TPMotorbikeCarLock";
        if (!FileExist(dir)) MakeDirectory(dir);
        return dir+"\\";
    }
    static string Path(MotorbikeScript bike)
    {
        int a,b,c,d;
        bike.GetPersistentID(a,b,c,d);
        if (a==0 && b==0 && c==0 && d==0) return "";
        return Directory()+a.ToString()+"_"+b.ToString()+"_"+c.ToString()+"_"+d.ToString()+".json";
    }
    static bool Valid(TPCMRecord record, MotorbikeScript bike)
    {
        return record && record.Version==1 && record.Type==bike.GetType() && (record.Owner!="" || !record.Locked);
    }
    static bool Save(string path, TPCMRecord record)
    {
        if (path=="") return false;
        string error,expected,actual;
        if (!JsonFileLoader<TPCMRecord>.SaveFile(path+".stage",record,error)) return false;
        TPCMRecord check=new TPCMRecord;
        if (!JsonFileLoader<TPCMRecord>.LoadFile(path+".stage",check,error)) return false;
        if (!JsonFileLoader<TPCMRecord>.MakeData(record,expected,error) || !JsonFileLoader<TPCMRecord>.MakeData(check,actual,error) || expected!=actual) return false;
        if (FileExist(path) && !CopyFile(path,path+".backup")) return false;
        if (!CopyFile(path+".stage",path)) return false;
        check=new TPCMRecord;
        if (!JsonFileLoader<TPCMRecord>.LoadFile(path,check,error) || !JsonFileLoader<TPCMRecord>.MakeData(check,actual,error) || expected!=actual)
        {
            if (FileExist(path+".backup")) CopyFile(path+".backup",path);
            return false;
        }
        return true;
    }
    static TPCMRecord Copy(TPCMRecord source)
    {
        if (!source) return null;
        TPCMRecord record=new TPCMRecord;
        record.Type=source.Type; record.Owner=source.Owner; record.Password=source.Password; record.Locked=source.Locked;
        return record;
    }
    static MotorbikeScript Target(ActionTarget target)
    {
        if (!target) return null;
        MotorbikeScript bike=MotorbikeScript.Cast(target.GetObject());
        if (!bike) bike=MotorbikeScript.Cast(target.GetParent());
        if (Supports(bike)) return bike;
        return null;
    }
    static bool Empty(MotorbikeScript bike)
    {
        if (!bike || bike.EngineIsOn() || bike.GetSpeedometerAbsolute()>0.5) return false;
        for (int i=0;i<bike.CrewSize();i++) if (bike.CrewMember(i)) return false;
        return true;
    }
    static bool Near(PlayerBase player,MotorbikeScript bike)
    {
        return player && player.GetIdentity() && player.IsAlive() && !player.IsUnconscious() && Supports(bike) && !bike.IsRuined() && vector.Distance(player.GetPosition(),bike.GetPosition())<=4;
    }
    static bool Owner(PlayerBase player,MotorbikeScript bike)
    {
        return Near(player,bike) && bike.m_TPCMRecord && bike.m_TPCMRecord.Owner==player.GetIdentity().GetPlainId();
    }
    // Empty input is an owner-only shortcut. Any supplied PIN must match.
    static bool CanToggle(bool owner,string stored,string attempt)
    {
        return (owner && attempt=="") || (stored!="" && stored==attempt);
    }
    static bool Password(string password)
    {
        if (password.Length()!=8) return false;
        for (int i=0;i<8;i++) if ("0123456789".IndexOf(password.Substring(i,1))<0) return false;
        return true;
    }
    static void Notify(PlayerBase player,string message)
    {
        if (player && player.GetIdentity()) NotificationSystem.SendNotificationToPlayerIdentityExtended(player.GetIdentity(),5,"Motorbike CarLock",message,"Garage/image/CarLogo.paa");
    }
    static bool Commit(MotorbikeScript bike,TPCMRecord record)
    {
        if (!GetGame().IsServer() || !Valid(record,bike) || !Save(Path(bike),record)) return false;
        bike.m_TPCMRecord=record;
        bike.m_TPCMReady=true;
        bike.TPCMApply();
        return true;
    }
};

modded class MotorbikeScript
{
    ref TPCMRecord m_TPCMRecord;
    ref TPCMRecord m_TPCMPending;
    bool m_TPCMReady;
    bool m_TPCMLocked;
    bool m_TPCMInstalled;
    bool m_TPCMHasPassword;
    bool m_TPCMFault;
    bool m_TPCMFromStorage;
    int m_TPCMIdRetries;
    int m_TPCMSound;
    int m_TPCMLastSound;
    EffectSound m_TPCMEffect;
    void MotorbikeScript()
    {
        RegisterNetSyncVariableBool("m_TPCMLocked");
        RegisterNetSyncVariableBool("m_TPCMInstalled");
        RegisterNetSyncVariableBool("m_TPCMHasPassword");
        RegisterNetSyncVariableBool("m_TPCMFault");
        RegisterNetSyncVariableInt("m_TPCMSound");
    }
    void ~MotorbikeScript()
    {
        if (GetGame()) GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).Remove(TPCMLoad);
    }
    override void EEInit()
    {
        super.EEInit();
        if (GetGame().IsServer() && TPCM.Supports(this)) GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(TPCMLoad,1000,false);
    }
    override bool OnStoreLoad(ParamsReadContext ctx,int version)
    {
        m_TPCMFromStorage=true;
        return super.OnStoreLoad(ctx,version);
    }
    override void AfterStoreLoad()
    {
        super.AfterStoreLoad();
        if (GetGame().IsServer() && TPCM.Supports(this)) GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(TPCMLoad,1000,false);
    }
    void TPCMLoad()
    {
        if (m_TPCMReady) return;
        string path=TPCM.Path(this);
        if (path=="")
        {
            m_TPCMIdRetries++;
            if (m_TPCMIdRetries<60) { GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(TPCMLoad,1000,false); return; }
            // No persistent ID after a minute: run without a lock record.
            Print("[TPCM] No persistent ID for "+GetType()+"; motorbike lock disabled for this instance");
            m_TPCMReady=true;
            TPCMApply();
            return;
        }
        m_TPCMReady=true;
        if (!m_TPCMFromStorage && (FileExist(path) || FileExist(path+".backup")))
        {
            // A bike that was not loaded from storage is new: a record for its ID is stale.
            Print("[TPCM] Removing stale lock record for a new bike: "+path);
            DeleteFile(path);
            DeleteFile(path+".backup");
            TPCMApply();
            return;
        }
        if (!FileExist(path) && FileExist(path+".backup"))
        {
            m_TPCMFault=true; m_TPCMLocked=true; SetSynchDirty();
            Print("[TPCM] Missing lock record with backup; vehicle blocked: "+path);
            return;
        }
        if (FileExist(path))
        {
            string error;
            TPCMRecord record=new TPCMRecord;
            if (!JsonFileLoader<TPCMRecord>.LoadFile(path,record,error) || !TPCM.Valid(record,this))
            {
                m_TPCMFault=true; m_TPCMLocked=true; SetSynchDirty();
                Print("[TPCM] Invalid lock record; vehicle blocked: "+path);
                return;
            }
            m_TPCMRecord=record;
        }
        TPCMApply();
    }
    void TPCMApply()
    {
        m_TPCMInstalled=m_TPCMRecord && m_TPCMRecord.Owner!="";
        m_TPCMLocked=m_TPCMInstalled && m_TPCMRecord.Locked;
        m_TPCMHasPassword=m_TPCMInstalled && m_TPCMRecord.Password!="";
        TPCMInventory();
        SetSynchDirty();
    }
    bool TPCMBlocked()
    {
        if (GetGame().IsServer() && TPCM.Supports(this) && !m_TPCMReady && TPCM.Path(this)!="") TPCMLoad();
        return TPCM.Supports(this) && (m_TPCMLocked || m_TPCMFault);
    }
    void TPCMInventory()
    {
        if (!TPCM.Supports(this)) return;
        if (TPCMBlocked()) GetInventory().LockInventory(HIDE_INV_FROM_SCRIPT);
        else GetInventory().UnlockInventory(HIDE_INV_FROM_SCRIPT);
    }
    void TPCMSound(int sound)
    {
        m_TPCMSound=((m_TPCMSound/4)+1)*4+sound;
        SetSynchDirty();
    }
    override void OnVariablesSynchronized()
    {
        super.OnVariablesSynchronized(); TPCMInventory();
        if (m_TPCMSound==m_TPCMLastSound) return;
        m_TPCMLastSound=m_TPCMSound;
        if (m_TPCMEffect) m_TPCMEffect.SoundStop();
        int sound=m_TPCMSound%4;
        if (sound==1) PlaySoundSet(m_TPCMEffect,"cl_CarLock_CarLock_SoundSet",0,0);
        if (sound==2) PlaySoundSet(m_TPCMEffect,"cl_CarLock_CarUnlock_SoundSet",0,0);
        if (sound==3) PlaySoundSet(m_TPCMEffect,"cl_CarLock_CarAlarm_SoundSet",0,0);
    }
    override bool CanGetIn() { return !(TPCM.Supports(this) && GetGame().IsServer() && !m_TPCMReady) && !TPCMBlocked() && super.CanGetIn(); }
    override bool OnBeforeEngineStart() { return !(TPCM.Supports(this) && GetGame().IsServer() && !m_TPCMReady) && !TPCMBlocked() && super.OnBeforeEngineStart(); }
    override bool CanReceiveAttachment(EntityAI attachment,int slotId) { return !TPCMBlocked() && super.CanReceiveAttachment(attachment,slotId); }
    override bool CanReleaseAttachment(EntityAI attachment) { return !TPCMBlocked() && super.CanReleaseAttachment(attachment); }
    override bool CanReceiveItemIntoCargo(EntityAI item) { return !TPCMBlocked() && super.CanReceiveItemIntoCargo(item); }
    override bool CanReleaseCargo(EntityAI cargo) { return !TPCMBlocked() && super.CanReleaseCargo(cargo); }
    override bool IsInventoryVisible() { return !TPCMBlocked() && super.IsInventoryVisible(); }
};

modded class CarLockCore
{
    override void InitRPC()
    {
        super.InitRPC();
        if (GetGame().IsServer()) GetRPCManager().AddRPC("TPMotorbikeCarLock","TPCMRequest",m_CarLockServer,SingeplayerExecutionType.Client);
        if (GetGame().IsServer()) GetRPCManager().AddRPC("TPMotorbikeCarLock","TPCMHotkeyRequest",m_CarLockServer,SingeplayerExecutionType.Client);
        else GetRPCManager().AddRPC("TPMotorbikeCarLock","TPCMOpenMenu",this,SingeplayerExecutionType.Server);
    }
    void TPCMOpenMenu(CallType type,ParamsReadContext ctx,PlayerIdentity sender,Object target)
    {
        if (GetGame().IsServer() || type!=CallType.Client) return;
        Param1<MotorbikeScript> data;
        if (!ctx.Read(data)) return;
        TPCMShowMenu(data.param1,0);
    }
    void TPCMShowMenu(MotorbikeScript bike,int attempt)
    {
        PlayerBase player=PlayerBase.Cast(GetGame().GetPlayer());
        if (!player || !TPCM.Supports(bike) || vector.Distance(player.GetPosition(),bike.GetPosition())>4) return;
        // Installation RPC may arrive before the entity net-sync update.
        if (!bike.m_TPCMInstalled || GetGame().GetUIManager().GetMenu())
        {
            if (attempt<4) GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(TPCMShowMenu,250,false,bike,attempt+1);
            return;
        }
        player.m_TPCMMenu=new TPCMMenu(bike);
        GetGame().GetUIManager().ShowScriptedMenu(player.m_TPCMMenu,NULL);
    }
};
modded class CarLockServer
{
    ref map<string,int> m_TPCMAttempts=new map<string,int>;
    ref map<string,int> m_TPCMWrongPins=new map<string,int>;
    ref map<string,int> m_TPCMLockedOut=new map<string,int>;
    // Five wrong PINs on one motorbike, from any account, lock its keypad for ten minutes.
    bool TPCMKeypadLocked(MotorbikeScript bike)
    {
        int until;
        return m_TPCMLockedOut.Find(TPCM.Path(bike),until) && GetGame().GetTime()<until;
    }
    void TPCMWrongPin(PlayerBase player,MotorbikeScript bike)
    {
        string key=TPCM.Path(bike);
        int count;
        m_TPCMWrongPins.Find(key,count);
        count++;
        if (count>=5)
        {
            m_TPCMWrongPins.Remove(key);
            m_TPCMLockedOut.Set(key,GetGame().GetTime()+600000);
            bike.TPCMSound(3);
            TPCM.Notify(player,"Wrong PIN. The keypad is blocked for 10 minutes.");
            return;
        }
        m_TPCMWrongPins.Set(key,count);
        TPCM.Notify(player,"Wrong PIN.");
    }
    void TPCMHotkeyRequest(CallType type,ParamsReadContext ctx,PlayerIdentity sender,Object target)
    {
        if (!GetGame().IsServer() || type!=CallType.Server || !sender) return;
        Param1<MotorbikeScript> data;
        if (!ctx.Read(data)) return;
        PlayerBase player=CLGetPlayerByIdentity(sender);
        MotorbikeScript bike=data.param1;
        if (!TPCM.Near(player,bike) || !bike.m_TPCMReady || bike.m_TPCMFault || !bike.m_TPCMRecord || bike.m_TPCMRecord.Owner=="") return;
        if (!TPCM.Owner(player,bike))
        {
            GetRPCManager().SendRPC("TPMotorbikeCarLock","TPCMOpenMenu",new Param1<MotorbikeScript>(bike),true,sender);
            return;
        }
        int previous;
        string uid=sender.GetPlainId();
        if (m_TPCMAttempts.Find(uid,previous) && GetGame().GetTime()-previous<3000) return;
        m_TPCMAttempts.Set(uid,GetGame().GetTime());
        if (!TPCM.Empty(bike)) { TPCM.Notify(player,"Turn the engine off, stop and get off the motorbike first."); return; }
        TPCMRecord record=TPCM.Copy(bike.m_TPCMRecord);
        record.Locked=!record.Locked;
        if (!TPCM.Commit(bike,record)) { TPCM.Notify(player,"The lock could not be saved. Nothing changed."); return; }
        if (record.Locked) { bike.TPCMSound(1); TPCM.Notify(player,"Motorbike locked."); }
        else { bike.TPCMSound(2); TPCM.Notify(player,"Motorbike unlocked."); }
    }
    void TPCMRequest(CallType type,ParamsReadContext ctx,PlayerIdentity sender,Object target)
    {
        if (!GetGame().IsServer() || type!=CallType.Server || !sender) return;
        Param2<MotorbikeScript,string> data;
        if (!ctx.Read(data)) return;
        PlayerBase player=CLGetPlayerByIdentity(sender);
        MotorbikeScript bike=data.param1;
        if (!TPCM.Near(player,bike) || !bike.m_TPCMReady || bike.m_TPCMFault || !bike.m_TPCMRecord || bike.m_TPCMRecord.Owner=="") return;
        string uid=sender.GetPlainId();
        int previous;
        if (m_TPCMAttempts.Find(uid,previous) && GetGame().GetTime()-previous<3000) return;
        m_TPCMAttempts.Set(uid,GetGame().GetTime());
        TPCMRecord record=TPCM.Copy(bike.m_TPCMRecord);
        bool owner=record.Owner==uid;
        bool defining=owner && record.Password=="" && data.param2!="";
        if (defining)
        {
            if (!TPCM.Password(data.param2)) { TPCM.Notify(player,"The PIN must be exactly 8 digits."); return; }
            record.Password=data.param2;
        }
        else
        {
            if (data.param2!="" && TPCMKeypadLocked(bike)) { TPCM.Notify(player,"The keypad is blocked. Try again later."); return; }
            if (!TPCM.CanToggle(owner,record.Password,data.param2)) { TPCMWrongPin(player,bike); return; }
            m_TPCMWrongPins.Remove(TPCM.Path(bike));
            if (!TPCM.Empty(bike)) { TPCM.Notify(player,"Turn the engine off, stop and get off the motorbike first."); return; }
            record.Locked=!record.Locked;
        }
        if (!TPCM.Commit(bike,record)) { TPCM.Notify(player,"The lock could not be saved. Nothing changed."); return; }
        if (defining)
        {
            TPCM.Notify(player,"PIN set. You own this lock.");
            return;
        }
        if (record.Locked) bike.TPCMSound(1);
        else bike.TPCMSound(2);
        if (record.Locked) TPCM.Notify(player,"Motorbike locked.");
        else TPCM.Notify(player,"Motorbike unlocked.");
    }
};
#endif
