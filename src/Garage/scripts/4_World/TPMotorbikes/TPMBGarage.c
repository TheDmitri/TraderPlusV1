modded class GarageServer
{
    protected ref set<string> m_TPMBActive = new set<string>;
    protected ref map<string,ref TPMBDelivery> m_TPMBDeliveries = new map<string,ref TPMBDelivery>;

    override void GarageRequestHandler(PlayerBase player, int id, vector pos, bool isFlag)
    {
        if (player && player.GetIdentity() && id == TPMB.PlayerId(player))
        {
            GarageData garage = GarageData.Load(id.ToString(), player.GetIdentity().GetName());
            foreach (VehiculeData entry : garage.Vehicles)
            {
                if (entry.TPMBVersion == 1 && entry.TPMBState != "Stored")
                {
                    // Do not let UseGarageOnlyToTrade auto-expire an interrupted transaction.
                    foreach (GarageNPC npc : GetGarageConfig().NPCs)
                    {
                        if (vector.Distance(pos, npc.Position) < 1.0 && vector.Distance(player.GetPosition(), npc.Position) < 5.0)
                        {
                            TPMBResponse(player, garage, npc.ParkingPosition);
                            TPMB.Notify(player, "Garage com operacao pendente. Solicite verificacao administrativa antes de repetir.");
                            return;
                        }
                    }
                    return;
                }
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
            TPMB.Notify(player, "Use o Garage terrestre e fique proximo do atendente.");
            return;
        }
        vector pos = npc.ParkingPosition;
        vector ori = npc.ParkingOrientation;
        if (!GarageHelpers.IsParkingAvailable(pos, ori) || TPMB.Nearby(pos))
        {
            TPMB.Notify(player, "Estacionamento ocupado. Liberte o ponto antes de retirar.");
            return;
        }
        GarageData garage = GarageData.Load(id.ToString(), player.GetIdentity().GetName());
        if (!garage || index < 0 || index >= garage.Vehicles.Count())
            return;
        VehiculeData entry = garage.Vehicles[index];
        if (entry.VehicleName != type || entry.TPMBVersion != 1 || !entry.TPMBData)
        {
            TPMB.Notify(player, "Registro sem snapshot compativel. Solicite verificacao administrativa.");
            return;
        }
        if (entry.TPMBState != "Stored")
        {
            TPMB.Notify(player, "Operacao pendente: nao repetir a entrega. Solicite verificacao administrativa.");
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
            TPMB.Notify(player, "Falha no banco do Garage. Retirada cancelada.");
            return;
        }
        MotorbikeScript bike = MotorbikeScript.Cast(GetGame().CreateObject(type, pos, false, false, true));
        if (!bike)
        {
            entry.TPMBState="Stored"; TPMB.Save(id,garage);
            TPMB.Notify(player,"Falha ao criar moto. Registro preservado no Garage.");
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
                    if (!TPMB.Save(delivery.Id,garage)) Print("[TPMB s01] Cancel save failed; inspect pending token "+delivery.Token);
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
            TPMBCancelDelivery(delivery,"Entrega cancelada. Registro preservado; nenhuma taxa cobrada.");
            return true;
        }
        if (vector.Distance(bike.GetPosition(),delivery.Position)>2 || bike.EngineIsOn())
        {
            TPMBCancelDelivery(delivery,"Moto movida durante a entrega. Retirada cancelada.");
            return true;
        }
        for (int seat=0;seat<bike.CrewSize();seat++)
        {
            if (bike.CrewMember(seat))
            {
                TPMBCancelDelivery(delivery,"Moto ocupada durante a entrega. Retirada cancelada.");
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
            TPMBCancelDelivery(delivery,"Registro da entrega alterado. Solicite verificacao administrativa.");
            return true;
        }
        VehiculeData entry=garage.Vehicles[index];
        if (!delivery.Restored)
        {
            if (!TPMB.Restore(bike,entry))
            {
                TPMBCancelDelivery(delivery,"Falha ao restaurar pecas ou dados. Registro preservado no Garage.");
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
            Print("[TPMB s01] Fuel tick: "+bike.GetType()+" / expected="+delivery.ExpectedFuel.ToString()+" / actual="+actual.ToString()+" / retry="+delivery.Retries.ToString());
            if (capacity<=0 || delivery.ExpectedFuel<0 || delivery.ExpectedFuel>capacity+0.01)
            {
                TPMBCancelDelivery(delivery,"Quantidade de combustivel invalida. Registro preservado.");
                return true;
            }
            if (Math.AbsFloat(actual-delivery.ExpectedFuel)>0.01)
            {
                if (delivery.Retries>=3)
                {
                    TPMBCancelDelivery(delivery,"Abastecimento nao confirmado. Registro preservado; nenhuma taxa cobrada.");
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
            TPMB.Notify(player, "Falha no registro do comprador. Retirada cancelada; nenhuma taxa cobrada.");
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
            TPMB.Notify(player, "Pagamento falhou. Retirada cancelada.");
            return;
        }
        garage.Vehicles.RemoveOrdered(index);
        if (!TPMB.Save(id, garage))
        {
            TPMB.ForgetOwner(bike, player);
            GetGame().ObjectDelete(bike);
            TPMBRefund(player, cost);
            // The on-disk Withdrawing entry stays quarantined until an administrator checks it.
            TPMB.Notify(player, "Falha na confirmacao: veiculo removido e taxa devolvida. Solicite verificacao do registro pendente.");
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
            TPMB.Notify(player, "Este veiculo nao tem registro de compra em seu nome neste patch.");
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
            TPMB.Notify(player, "Pare o veiculo, desca e desligue o motor antes de guardar.");
            return;
        }
        TPMBSnapshot snapshot = TPMB.Capture(bike);
        if (!snapshot)
        {
            TPMB.Notify(player, "Retire carga, cadeado e acessorios especiais antes de guardar.");
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
            TPMB.Notify(player, "Falha ao gravar o Garage. Veiculo preservado no mundo.");
            return;
        }
        if (!TPMBPay(player, cost))
        {
            garage.Vehicles.RemoveOrdered(garage.Vehicles.Count() - 1);
            TPMB.Save(id, garage);
            TPMB.Notify(player, "Pagamento falhou. Veiculo preservado no mundo.");
            return;
        }
        TPMB.ForgetOwner(bike, player);
        GetGame().ObjectDelete(bike);
        entry.TPMBState = "Stored";
        if (!TPMB.Save(id, garage))
        {
            TPMB.Notify(player, "Veiculo registrado como Parking. Solicite verificacao administrativa antes de retirar.");
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
