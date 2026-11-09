// Motorbikes through receipts (UseGarageToTradeCar off), the same way TraderPlus handles cars:
// a bought or packed bike becomes a NewReceipt, and deploying the receipt spawns the bike.

class TPMBReceipt
{
    // Called from TraderPlusHelper.CreateVehicle when a receipt holds a motorbike.
    static bool Deploy(PlayerBase player, string classname, vector position, vector orientation, array<string> attachments, EntityAI receipt, int carPassword)
    {
        MotorbikeScript bike = MotorbikeScript.Cast(GetGame().CreateObject(classname, position, false, false, true));
        if (!bike)
            return false;
        bike.SetPosition(position);
        bike.SetOrientation(orientation);
        // Owned from the start: the fresh-bike cleanup must not drop the records written below.
        bike.m_TPMBRegistered = true;

        if (attachments)
        {
            foreach (string line : attachments)
            {
                TStringArray fields = new TStringArray;
                line.Split(",", fields);
                if (fields.Count() == 0 || !TPMB.AllowedPart(fields[0]))
                    continue;
                EntityAI part = bike.GetInventory().CreateAttachment(fields[0]);
                if (part && fields.Count() > 1 && fields[1].ToFloat() >= 0)
                    part.SetHealth("", "", fields[1].ToFloat());
            }
        }

        // A bought receipt carries the configured parts only; vanilla bikes also need wheels, plug and light.
        TPMB.EquipVanillaParts(bike);

        bike.Fill(MotorbikeFluid.FUEL, bike.GetFluidCapacity(MotorbikeFluid.FUEL));
        if (receipt)
            TraderPlusHelper.TransferInventoryVehicle(receipt, bike);

        // The persistent ID needed by the owner and lock records is assigned shortly after creation.
        GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(RegisterOwner, 1000, false, bike, player, carPassword, 0);
        return true;
    }

    static void RegisterOwner(MotorbikeScript bike, PlayerBase player, int carPassword, int attempt)
    {
        if (!bike || !player || !player.GetIdentity())
            return;
        if (TPMB.OwnerPath(bike) == "")
        {
            if (attempt < 30)
                GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(RegisterOwner, 1000, false, bike, player, carPassword, attempt + 1);
            return;
        }
        #ifndef CARLOCKDISABLE
        TPCMRecord record = new TPCMRecord;
        record.Type = bike.GetType();
        record.Owner = player.GetIdentity().GetPlainId();
        record.Locked = true;
        string password = carPassword.ToString();
        if (carPassword > 0 && TPCM.Password(password))
            record.Password = password;
        bike.m_TPCMPending = record;
        #endif
        if (!TPMB.RegisterOwner(bike, player, TPMB.Token()))
            GetTraderPlusLogger().LogInfo("[TPMB] Receipt bike " + bike.GetType() + " deployed without an owner record");
    }

};

class ActionPackMotorbikeCB : ActionContinuousBaseCB
{
    override void CreateActionComponent()
    {
        m_ActionData.m_ActionComponent = new CAContinuousTime(15);
    }
};

// Packs an owned, empty, unlocked motorbike into a receipt, like ActionPackVehicle does for cars.
class ActionPackMotorbike : ActionContinuousBase
{
    void ActionPackMotorbike()
    {
        m_CallbackClass = ActionPackMotorbikeCB;
        m_CommandUID = DayZPlayerConstants.CMD_ACTIONFB_CRAFTING;
        m_FullBody = true;
        m_StanceMask = DayZPlayerConstants.STANCEMASK_CROUCH;
        m_SpecialtyWeight = UASoftSkillsWeight.PRECISE_LOW;
    }

    override void CreateConditionComponents()
    {
        m_ConditionTarget = new CCTNonRuined(10);
        m_ConditionItem = new CCINonRuined;
    }

    override string GetText()
    {
        return "#tpm_pack_vehicle";
    }

    static MotorbikeScript Target(ActionTarget target)
    {
        if (!target)
            return null;
        MotorbikeScript bike = MotorbikeScript.Cast(target.GetObject());
        if (!bike)
            bike = MotorbikeScript.Cast(target.GetParent());
        if (bike && TPMB.Supports(bike.GetType()))
            return bike;
        return null;
    }

    static bool Empty(MotorbikeScript bike)
    {
        if (bike.EngineIsOn() || bike.GetSpeedometerAbsolute() > 0.5)
            return false;
        for (int i = 0; i < bike.CrewSize(); i++)
        {
            if (bike.CrewMember(i))
                return false;
        }
        return true;
    }

    override bool ActionCondition(PlayerBase player, ActionTarget target, ItemBase item)
    {
        MotorbikeScript bike = Target(target);
        if (!bike || !Empty(bike))
            return false;
        #ifndef CARLOCKDISABLE
        if (bike.m_TPCMLocked || bike.m_TPCMFault)
            return false;
        #endif
        if (GetGame().IsServer())
            return true;
        return player.GetSafeZoneStatus() == SZ_IN_SAFEZONE || !GetTraderPlusConfigClient().IsReceiptTraderOnly;
    }

    override void OnFinishProgressServer(ActionData action_data)
    {
        PlayerBase player = action_data.m_Player;
        MotorbikeScript bike = Target(action_data.m_Target);
        if (!bike || !player || !player.GetIdentity() || !action_data.m_MainItem || action_data.m_MainItem.GetHierarchyRootPlayer() != player)
            return;
        if (!Empty(bike) || vector.Distance(player.GetPosition(), bike.GetPosition()) > 10)
            return;
        // The client checks the trader-only rule too, but a modified client could skip it.
        bool traderOnly = GetTraderPlusConfigServer().IsReceiptTraderOnly && !GetTraderPlusConfigServer().AllowReceiptPackAnywhere;
        if (traderOnly && player.GetSafeZoneStatus() != SZ_IN_SAFEZONE)
        {
            TPMB.Notify(player, "Motorbikes can only be packed at a trader.");
            return;
        }
        // Only the registered owner packs a bike; the client condition cannot know ownership.
        if (!TPMB.IsRegisteredOwner(bike, player))
        {
            TPMB.Notify(player, "This motorbike is not registered to you.");
            return;
        }

        NewReceipt receipt = NewReceipt.Cast(GetGame().CreateObject("NewReceipt", player.GetPosition()));
        if (!receipt)
            return;
        receipt.CarClassName = bike.GetType();

        #ifndef CARLOCKDISABLE
        if (GetTraderPlusConfigServer().IsReceiptSaveLock && bike.m_TPCMRecord && bike.m_TPCMRecord.Password != "")
            receipt.CarPassword = bike.m_TPCMRecord.Password.ToInt();
        #endif

        receipt.Attachments = new TStringArray;
        if (GetTraderPlusConfigServer().IsReceiptSaveAttachment)
        {
            for (int i = 0; i < bike.GetInventory().AttachmentCount(); i++)
            {
                EntityAI part = bike.GetInventory().GetAttachmentFromIndex(i);
                if (part && TPMB.AllowedPart(part.GetType()))
                    receipt.Attachments.Insert(part.GetType() + "," + part.GetHealth().ToString());
            }
        }
        else
        {
            receipt.Attachments = TraderPlusHelper.GetVehicleAttachmentsFromConfig(receipt.CarClassName);
        }

        if (GetTraderPlusConfigServer().IsReceiptSaveCargo)
            receipt = NewReceipt.Cast(TraderPlusHelper.TransferInventoryVehicle(bike, receipt));

        receipt.GetInventory().LockInventory(HIDE_INV_FROM_SCRIPT);
        receipt.SetSynchDirty();

        TPMB.ForgetOwner(bike, player);
        GetGame().ObjectDelete(bike);
    }
};

modded class ActionConstructor
{
    override void RegisterActions(TTypenameArray actions)
    {
        super.RegisterActions(actions);
        actions.Insert(ActionPackMotorbike);
    }
};

modded class Wrench
{
    override void SetActions()
    {
        super.SetActions();
        AddAction(ActionPackMotorbike);
    }
};
