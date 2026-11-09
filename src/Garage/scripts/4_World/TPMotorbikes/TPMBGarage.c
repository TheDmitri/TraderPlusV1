modded class GarageServer
{
    protected ref set<string> m_TPMBActive = new set<string>;
    protected ref map<string,ref TPMBDelivery> m_TPMBDeliveries = new map<string,ref TPMBDelivery>;

    override void GarageRequestHandler(PlayerBase player, int id, vector pos, bool isFlag)
    {
        if (player && player.GetIdentity() && id == TPMB.PlayerId(player))
        {
            GarageData garage = GarageData.Load(id.ToString(), player.GetIdentity().GetName());
            bool pending = false;
            bool touched = false;
            foreach (VehiculeData entry : garage.Vehicles)
            {
                if (entry.TPMBVersion != 1 || entry.TPMBState == "Stored")
                    continue;
                pending = true;
                // UseGarageOnlyToTrade must not expire an interrupted motorbike transaction.
                if (GetGarageConfig().UseGarageOnlyToTrade && entry.Timestamp != TraderPlusHelper.GetTimeStamp())
                {
                    entry.Timestamp = TraderPlusHelper.GetTimeStamp();
                    touched = true;
                }
            }
            if (touched)
                TPMB.Save(id, garage);
            if (pending)
            {
                Print("[TPMB] Pending motorbike entry in Garage " + id.ToString() + " (" + player.GetIdentity().GetPlainId() + "): admin check needed");
                TPMB.Notify(player, "A motorbike operation in your Garage was interrupted. That motorbike stays locked until an admin checks it; your other vehicles are available.");
            }
        }
        super.GarageRequestHandler(player, id, pos, isFlag);
    }

    void TPMBResponse(PlayerBase player, GarageData garage, vector pos)
    {
        TStringArray names = new TStringArray;
        TIntArray health = new TIntArray;
        foreach (VehiculeData entry : garage.Vehicles)
        {
            names.Insert(entry.VehicleName);
            health.Insert(GarageHelpers.ConvertHealthToHealthLevel(entry.VehicleName, entry.EngineHealth));
        }
        GetRPCManager().SendRPC("Garage", "GarageResponse", new Param4<TStringArray, TIntArray, vector, int>(names, health, pos, TraderPlusResponse.NO_TRADE), true, player.GetIdentity());
    }

    bool TPMBCanPay(PlayerBase player, int cost)
    {
        if (cost < 0)
            return false;
        if (cost == 0)
            return true;
        if (GetGarageConfig().PayWithBankAccount && !player.GetBankAccount())
            return false;
        return GarageHelpers.CheckifPlayerHasEnoughMoney(player, cost);
    }

    bool TPMBPay(PlayerBase player, int cost)
    {
        if (cost == 0)
            return true;
        return GarageHelpers.RemoveMoneyFromPlayer(player, cost);
    }

    void TPMBRefund(PlayerBase player, int amount)
    {
        if (amount <= 0)
            return;
        if (GetGarageConfig().PayWithBankAccount && player.GetBankAccount())
        {
            player.GetBankAccount().MoneyAmount = player.GetBankAccount().MoneyAmount + amount;
            player.GetBankAccount().UpdateAccount(player);
        }
        else
            TraderPlusBankHelpers.AddMoneyToPlayer(player, amount);
    }

    override void ParkOutRequestHandler(PlayerBase player, int id, int listpos, string carname, vector pos)
    {
        if (!TPMB.Supports(carname))
        {
            super.ParkOutRequestHandler(player, id, listpos, carname, pos);
            return;
        }
        if (!player || !player.GetIdentity() || id != TPMB.PlayerId(player))
            return;
        string key = player.GetIdentity().GetPlainId();
        if (m_TPMBActive.Find(key) != -1)
            return;
        m_TPMBActive.Insert(key);
        TPMBParkOut(player, id, listpos, carname, pos);
        if (!m_TPMBDeliveries.Contains(key)) m_TPMBActive.RemoveItem(key);
    }

    void TPMBParkOut(PlayerBase player, int id, int index, string type, vector requested)
    {
        GarageNPC npc = TPMB.NPC(player, requested);
        if (!npc)
        {
            TPMB.Notify(player, "Use a Garage NPC and stand next to it.");
            return;
        }
        vector pos = npc.ParkingPosition;
        vector ori = npc.ParkingOrientation;
        if (!GarageHelpers.IsParkingAvailable(pos, ori) || TPMB.Nearby(pos))
        {
            TPMB.Notify(player, "The parking spot is taken. Clear it before retrieving a vehicle.");
            return;
        }
        GarageData garage = GarageData.Load(id.ToString(), player.GetIdentity().GetName());
        if (!garage || index < 0 || index >= garage.Vehicles.Count())
            return;
        VehiculeData entry = garage.Vehicles[index];
        if (entry.VehicleName != type || entry.TPMBVersion != 1 || !entry.TPMBData)
        {
            TPMB.Notify(player, "This Garage entry has no motorbike data. Ask an admin to check it.");
            return;
        }
        if (entry.TPMBState != "Stored")
        {
            TPMB.Notify(player, "This motorbike has an interrupted operation. Ask an admin to check it.");
            return;
        }
        int cost = GetGarageConfig().ParkOutCost;
        if (!TPMBCanPay(player, cost))
        {
            TPMB.Notify(player, GetGarageConfig().NotEnoughMoney);
            return;
        }
        entry.TPMBState = "Withdrawing";
        if (!TPMB.Save(id, garage))
        {
            TPMB.Notify(player, "The Garage could not be saved. Retrieval cancelled.");
            return;
        }
        MotorbikeScript bike = MotorbikeScript.Cast(GetGame().CreateObject(type, pos, false, false, true));
        if (!bike)
        {
            entry.TPMBState="Stored"; TPMB.Save(id,garage);
            TPMB.Notify(player,"The motorbike could not be created. Your Garage entry is kept.");
            return;
        }
        bike.m_TPMBDeliveryPending=true;
        bike.SetPosition(pos+Vector(0,0.1,0));
        bike.SetOrientation(ori);
        TPMBDelivery delivery=new TPMBDelivery;
        delivery.Player=player; delivery.Bike=bike; delivery.Id=id;
        delivery.UID=player.GetIdentity().GetPlainId(); delivery.Name=player.GetIdentity().GetName();
        delivery.Token=entry.TPMBToken; delivery.Requested=requested; delivery.Position=pos;
        delivery.Cost=cost;
        m_TPMBDeliveries.Set(delivery.UID,delivery);
        GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(TPMBResumeDelivery,250,false,delivery);
    }

    void TPMBCancelDelivery(TPMBDelivery delivery,string message)
    {
        if (delivery.Bike) GetGame().ObjectDelete(delivery.Bike);
        GarageData garage=GarageData.Load(delivery.Id.ToString(),delivery.Name);
        if (garage)
        {
            foreach (VehiculeData entry:garage.Vehicles)
            {
                if (entry.TPMBToken==delivery.Token && entry.TPMBState=="Withdrawing")
                {
                    entry.TPMBState="Stored";
                    if (!TPMB.Save(delivery.Id,garage)) Print("[TPMB] Cancel save failed; inspect pending token "+delivery.Token);
                    break;
                }
            }
        }
        TPMB.Notify(delivery.Player,message);
    }

    void TPMBResumeDelivery(TPMBDelivery delivery)
    {
        if (TPMBAdvanceDelivery(delivery))
        {
            m_TPMBDeliveries.Remove(delivery.UID);
            m_TPMBActive.RemoveItem(delivery.UID);
        }
    }

    bool TPMBAdvanceDelivery(TPMBDelivery delivery)
    {
        PlayerBase player=delivery.Player;
        MotorbikeScript bike=delivery.Bike;
        if (!player || !player.GetIdentity() || player.GetIdentity().GetPlainId()!=delivery.UID || !player.IsAlive() || !TPMB.NPC(player,delivery.Requested) || !bike || bike.IsRuined())
        {
            TPMBCancelDelivery(delivery,"Retrieval cancelled. Your Garage entry is kept and nothing was charged.");
            return true;
        }
        if (vector.Distance(bike.GetPosition(),delivery.Position)>2 || bike.EngineIsOn())
        {
            TPMBCancelDelivery(delivery,"The motorbike moved during retrieval. Retrieval cancelled.");
            return true;
        }
        for (int seat=0;seat<bike.CrewSize();seat++)
        {
            if (bike.CrewMember(seat))
            {
                TPMBCancelDelivery(delivery,"Someone got on the motorbike during retrieval. Retrieval cancelled.");
                return true;
            }
        }
        // Re-read current storage on every continuation: do not overwrite concurrent purchases.
        GarageData garage=GarageData.Load(delivery.Id.ToString(),delivery.Name);
        int index=-1;
        if (garage)
        {
            for (int j=0;j<garage.Vehicles.Count();j++)
                if (garage.Vehicles[j].TPMBToken==delivery.Token) { index=j; break; }
        }
        if (index<0 || garage.Vehicles[index].TPMBState!="Withdrawing")
        {
            TPMBCancelDelivery(delivery,"The Garage entry changed during retrieval. Retrieval cancelled.");
            return true;
        }
        VehiculeData entry=garage.Vehicles[index];
        if (!delivery.Restored)
        {
            if (!TPMB.Restore(bike,entry))
            {
                TPMBCancelDelivery(delivery,"The motorbike parts could not be restored. Your Garage entry is kept.");
                return true;
            }
            delivery.ExpectedFuel=entry.FuelAmmount;
            delivery.Restored=true;
            GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(TPMBResumeDelivery,250,false,delivery);
            return false;
        }
        if (TPMB.Supports(bike.GetType()))
        {
            float capacity=bike.GetFluidCapacity(MotorbikeFluid.FUEL);
            float actual=bike.GetFluidFraction(MotorbikeFluid.FUEL)*capacity;
            if (capacity<=0 || delivery.ExpectedFuel<0 || delivery.ExpectedFuel>capacity+0.01)
            {
                TPMBCancelDelivery(delivery,"Invalid fuel amount. Your Garage entry is kept.");
                return true;
            }
            if (Math.AbsFloat(actual-delivery.ExpectedFuel)>0.01)
            {
                if (delivery.Retries>=3)
                {
                    TPMBCancelDelivery(delivery,"Fuel could not be restored. Your Garage entry is kept and nothing was charged.");
                    return true;
                }
                delivery.Retries++;
                bike.LeakAll(MotorbikeFluid.FUEL);
                bike.Fill(MotorbikeFluid.FUEL,delivery.ExpectedFuel);
                GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(TPMBResumeDelivery,250,false,delivery);
                return false;
            }
        }
        int id=delivery.Id;
        int cost=delivery.Cost;
        vector pos=delivery.Position;
        if (!TPMBCanPay(player,cost))
        {
            TPMBCancelDelivery(delivery,GetGarageConfig().NotEnoughMoney);
            return true;
        }
        TPMBCommitDelivery(player,bike,garage,entry,index,id,cost,pos);
        return true;
    }

    void TPMBCommitDelivery(PlayerBase player,MotorbikeScript bike,GarageData garage,VehiculeData entry,int index,int id,int cost,vector pos)
    {
        if (!TPMB.RegisterOwner(bike, player, entry.TPMBToken))
        {
            TPMB.ForgetOwner(bike, player);
            GetGame().ObjectDelete(bike);
            entry.TPMBState = "Stored";
            TPMB.Save(id, garage);
            TPMB.Notify(player, "The owner could not be registered. Retrieval cancelled; nothing was charged.");
            return;
        }
        bike.SetKickstand(1.0);
        bike.SetSynchDirty();
        if (!TPMBPay(player, cost))
        {
            TPMB.ForgetOwner(bike, player);
            GetGame().ObjectDelete(bike);
            entry.TPMBState = "Stored";
            TPMB.Save(id, garage);
            TPMB.Notify(player, "Payment failed. Retrieval cancelled.");
            return;
        }
        garage.Vehicles.RemoveOrdered(index);
        if (!TPMB.Save(id, garage))
        {
            TPMB.ForgetOwner(bike, player);
            GetGame().ObjectDelete(bike);
            TPMBRefund(player, cost);
            // The on-disk Withdrawing entry stays quarantined until an administrator checks it.
            TPMB.Notify(player, "The Garage could not be saved: retrieval cancelled and the fee returned. Ask an admin to check the entry.");
            return;
        }
        bike.m_TPMBDeliveryPending=false;
        Print("[TPMB] ParkOut committed: " + keyForLog(player, entry));
        TPMB.Notify(player, GetGarageConfig().ParkOutSuccess);
        TPMBResponse(player, garage, pos);
    }

    string keyForLog(PlayerBase player, VehiculeData entry)
    {
        return player.GetIdentity().GetPlainId() + " / " + entry.VehicleName + " / " + entry.TPMBToken;
    }

    void TPMBParkIn(PlayerBase player, vector requested)
    {
        GarageNPC npc = TPMB.NPC(player, requested);
        if (!npc)
            return;
        MotorbikeScript bike = TPMB.Nearby(npc.ParkingPosition);
        if (!bike)
            return;
        if (!TPMB.IsRegisteredOwner(bike, player))
        {
            TPMB.Notify(player, "This motorbike is not registered to you.");
            return;
        }
        for (int seat = 0; seat < bike.CrewSize(); seat++)
        {
            if (bike.CrewMember(seat))
            {
                TPMB.Notify(player, GetGarageConfig().CarHasMember);
                return;
            }
        }
        if (bike.GetSpeedometerAbsolute() > 0.1 || (TPMB.Supports(bike.GetType()) && bike.EngineIsOn()))
        {
            TPMB.Notify(player, "Stop the motorbike, get off and turn the engine off before storing it.");
            return;
        }
        TPMBSnapshot snapshot = TPMB.Capture(bike);
        if (!snapshot)
        {
            TPMB.Notify(player, "Remove cargo and non-standard parts before storing the motorbike.");
            return;
        }
        int id = TPMB.PlayerId(player);
        GarageData garage = GarageData.Load(id.ToString(), player.GetIdentity().GetName());
        if (!garage || garage.Vehicles.Count() >= GetGarageConfig().MaxVehicleStored)
        {
            TPMB.Notify(player, GetGarageConfig().MaxVehicleStoredReached);
            return;
        }
        int cost = GetGarageConfig().ParkInCost;
        if (!TPMBCanPay(player, cost))
        {
            TPMB.Notify(player, GetGarageConfig().NotEnoughMoney);
            return;
        }
        VehiculeData entry = TPMB.Entry(bike, "Parking");
        if (!entry)
            return;
        garage.Vehicles.Insert(entry);
        if (!TPMB.Save(id, garage))
        {
            TPMB.Notify(player, "The Garage could not be saved. The motorbike stays in the world.");
            return;
        }
        if (!TPMBPay(player, cost))
        {
            garage.Vehicles.RemoveOrdered(garage.Vehicles.Count() - 1);
            TPMB.Save(id, garage);
            TPMB.Notify(player, "Payment failed. The motorbike stays in the world.");
            return;
        }
        TPMB.ForgetOwner(bike, player);
        GetGame().ObjectDelete(bike);
        entry.TPMBState = "Stored";
        if (!TPMB.Save(id, garage))
        {
            TPMB.Notify(player, "The motorbike was stored but the Garage could not confirm it. Ask an admin to check it before retrieving it.");
            return;
        }
        Print("[TPMB] ParkIn committed: " + keyForLog(player, entry));
        TPMB.Notify(player, GetGarageConfig().ParkInSuccess);
        TPMBResponse(player, garage, npc.ParkingPosition);
    }

    void TPMBParkInRequest(CallType type, ParamsReadContext ctx, PlayerIdentity sender, Object target)
    {
        if (!GetGame().IsServer())
            return;
        Param1<vector> data;
        if (!ctx.Read(data))
            return;
        PlayerBase player = GMGetPlayerByIdentity(sender);
        if (!player || !player.GetIdentity())
            return;
        string key = player.GetIdentity().GetPlainId();
        if (m_TPMBActive.Find(key) != -1)
            return;
        m_TPMBActive.Insert(key);
        TPMBParkIn(player, data.param1);
        m_TPMBActive.RemoveItem(key);
    }
};

modded class GarageCore
{
    override void InitRPC()
    {
        super.InitRPC();
        if (GetGame().IsServer())
            GetRPCManager().AddRPC("TPMB", "TPMBParkInRequest", m_GarageServer, SingeplayerExecutionType.Client);
    }
};

// Transient delivery state only; never append data to native vehicle persistence.
class TPMBDelivery
{
    PlayerBase Player;
    MotorbikeScript Bike;
    int Id;
    string UID;
    string Name;
    string Token;
    vector Requested;
    vector Position;
    int Cost;
    bool Restored;
    float ExpectedFuel;
    int Retries;
};
modded class MotorbikeScript
{
    bool m_TPMBDeliveryPending;
    bool m_TPMBFromStorage;
    bool m_TPMBRegistered;

    override bool OnStoreLoad(ParamsReadContext ctx, int version)
    {
        m_TPMBFromStorage = true;
        return super.OnStoreLoad(ctx, version);
    }

    override void EEInit()
    {
        super.EEInit();
        if (GetGame().IsServer() && TPMB.Supports(GetType()))
            GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(TPMBCheckFresh, 1000, false);
    }

    // A bike that was not loaded from storage is new: any owner record for its ID is stale.
    void TPMBCheckFresh()
    {
        if (!m_TPMBFromStorage && !m_TPMBDeliveryPending && !m_TPMBRegistered)
            TPMB.ForgetStaleOwner(this);
    }

    override bool CanGetIn() { return !m_TPMBDeliveryPending && super.CanGetIn(); }
    override bool OnBeforeEngineStart() { return !m_TPMBDeliveryPending && super.OnBeforeEngineStart(); }
    override bool CanReleaseAttachment(EntityAI attachment) { return !m_TPMBDeliveryPending && super.CanReleaseAttachment(attachment); }
    override bool CanReleaseCargo(EntityAI cargo) { return !m_TPMBDeliveryPending && super.CanReleaseCargo(cargo); }
};

// Self-contained menu integration: no fields or methods from the private boat override.
modded class GarageMenu
{
    protected TextListboxWidget m_TPMBList;
    protected ref TStringArray m_TPMBStoredNames = new TStringArray;
    protected int m_TPMBWorldRow = -1;
    protected float m_TPMBNextRequest;

    override Widget Init()
    {
        Widget root = super.Init();
        m_TPMBList = TextListboxWidget.Cast(root.FindAnyWidget("GarageList"));
        return root;
    }

    override void SetResponseData(TStringArray vehicles, vector parkpos)
    {
        super.SetResponseData(vehicles, parkpos);
        m_TPMBStoredNames.Clear();
        if (vehicles)
        {
            foreach (string type : vehicles)
                m_TPMBStoredNames.Insert(type);
        }
    }

    int TPMBSelectedIndex()
    {
        if (!m_TPMBList)
            return -1;
        int row = m_TPMBList.GetSelectedRow();
        if (row == m_TPMBWorldRow)
            return -1;
        int offset = 0;
        if (GarageHelpers.CheckForCarAtPos(m_ParkingPos))
            offset = 1;
        int index = row - offset;
        if (index < 0 || index >= m_TPMBStoredNames.Count())
            return -1;
        return index;
    }

    override void UIHandle()
    {
        super.UIHandle();
        m_TPMBWorldRow = -1;
        MotorbikeScript bike = TPMB.Nearby(m_ParkingPos);
        if (bike && m_TPMBList)
        {
            m_TPMBWorldRow = m_TPMBList.GetNumItems();
            m_TPMBList.AddItem(bike.GetType(), null, 0);
            m_TPMBList.SetItemColor(m_TPMBWorldRow, 0, ARGB(255, 191, 48, 48));
        }
    }

    override void VehicleListHandler()
    {
        if (m_TPMBList && m_TPMBWorldRow >= 0 && m_TPMBList.GetSelectedRow() == m_TPMBWorldRow)
        {
            MotorbikeScript bike = TPMB.Nearby(m_ParkingPos);
            if (bike)
                UpdateVehiclePreview(bike.GetType());
            return;
        }
        super.VehicleListHandler();
    }

    override void ParkInHandler()
    {
        if (!TPMB.Nearby(m_ParkingPos))
        {
            super.ParkInHandler();
            return;
        }
        float now = GetGame().GetTime();
        if (now < m_TPMBNextRequest)
            return;
        m_TPMBNextRequest = now + 1500;
        GetRPCManager().SendRPC("TPMB", "TPMBParkInRequest", new Param1<vector>(m_ParkingPos), true, null);
    }

    override void ParkOutHandler()
    {
        if (m_TPMBList && m_TPMBWorldRow >= 0 && m_TPMBList.GetSelectedRow() == m_TPMBWorldRow)
            return;
        int index = TPMBSelectedIndex();
        if (index < 0 || !TPMB.Supports(m_TPMBStoredNames[index]))
        {
            super.ParkOutHandler();
            return;
        }
        if (TPMB.Nearby(m_ParkingPos))
        {
            NotificationSystem.AddNotificationExtended(4, "Garage", "Clear the parking area before retrieving a vehicle.", "Garage/image/CarLogo.paa");
            return;
        }
        float now = GetGame().GetTime();
        if (now < m_TPMBNextRequest)
            return;
        m_TPMBNextRequest = now + 1500;
        GetRPCManager().SendRPC("Garage", "ParkOutRequest", new Param4<int, int, string, vector>(m_LowUID, index, m_TPMBStoredNames[index], m_ParkingPos), true, null);
    }
};
