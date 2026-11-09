class TPMB
{
    static string LockVault(string token)
    {
        if (token=="" || token.Length()>100) return "";
        for (int i=0;i<token.Length();i++) if ("0123456789_".IndexOf(token.Substring(i,1))<0) return "";
        return "$profile:TPMotorbikeCarLock\\garage_"+token+".json";
    }
    static bool SaveLockSnapshot(MotorbikeScript bike,VehiculeData entry)
    {
        #ifndef CARLOCKDISABLE
        if (bike.m_TPCMFault) return false;
        if (bike.m_TPCMRecord)
        {
            if (!TPCM.Save(LockVault(entry.TPMBToken),bike.m_TPCMRecord)) return false;
            entry.TPCMStateToken=entry.TPMBToken;
        }
        #endif
        return true;
    }
    static bool RestoreLockSnapshot(MotorbikeScript bike,VehiculeData entry)
    {
        #ifndef CARLOCKDISABLE
        if (entry.TPCMStateToken!="")
        {
            string path=LockVault(entry.TPCMStateToken);
            string error;
            TPCMRecord record=new TPCMRecord;
            if (path=="" || !JsonFileLoader<TPCMRecord>.LoadFile(path,record,error) || !TPCM.Valid(record,bike)) return false;
            bike.m_TPCMPending=record;
        }
        #endif
        return true;
    }
    static bool RegisterLockOwner(MotorbikeScript bike,PlayerBase player)
    {
        #ifndef CARLOCKDISABLE
        TPCMRecord record=TPCM.Copy(bike.m_TPCMPending);
        if (!record)
        {
            record=new TPCMRecord;
            record.Type=bike.GetType();record.Owner=player.GetIdentity().GetPlainId();record.Locked=true;
        }
        if (record.Owner!="" && record.Owner!=player.GetIdentity().GetPlainId()) return false;
        if (!TPCM.Commit(bike,record)) return false;
        bike.m_TPCMPending=null;
        #endif
        return true;
    }

    static bool Supports(string type)
    {
        switch (type)
        {
            case "Motorbike_01_Blue":
            case "Motorbike_01_Red":
            case "Motorbike_01_Yellow":
            case "Motorbike_02_Blue":
            case "Motorbike_02_Green":
            case "Motorbike_02_Red":
            case "Motorbike_02_Yellow":
                return true;
        }
        return false;
    }



    static void Notify(PlayerBase player, string text)
    {
        Print("[TPMB] " + text);
        if (player && player.GetIdentity())
            NotificationSystem.SendNotificationToPlayerIdentityExtended(player.GetIdentity(), 6, "Garage", text, "Garage/image/CarLogo.paa");
    }

    static int PlayerId(PlayerBase player)
    {
        return GarageHelpers.GetLowSteamID(player.GetIdentity().GetPlainId());
    }

    static string Token()
    {
        return TraderPlusHelper.GetTimeStamp().ToString() + "_" + Math.RandomInt(100000, 999999).ToString();
    }

    // Keep ownership outside entity serialization; old map vehicles have no new ctx fields.
    static string OwnerPath(MotorbikeScript bike)
    {
        int a, b, c, d;
        bike.GetPersistentID(a, b, c, d);
        if (a == 0 && b == 0 && c == 0 && d == 0)
            return "";
        return GM_DB_DIR_SERVER + "TPMBOwners\\" + a.ToString() + "_" + b.ToString() + "_" + c.ToString() + "_" + d.ToString() + ".json";
    }

    static bool RegisterOwner(MotorbikeScript bike, PlayerBase player, string token)
    {
        string path = OwnerPath(bike);
        if (path == "")
        {
            Notify(player, "ID persistente ainda indisponivel. Retirada cancelada; registro preservado.");
            return false;
        }
        string directory = GM_DB_DIR_SERVER + "TPMBOwners\\";
        if (!FileExist(directory) && !MakeDirectory(directory))
            return false;
        // A newly created persistent ID must not replace another live registration.
        if (FileExist(path))
            return false;
        TPMBOwnerRecord record = new TPMBOwnerRecord;
        record.Owner = player.GetIdentity().GetPlainId();
        record.Type = bike.GetType();
        record.Token = token;
        string error;
        if (!JsonFileLoader<TPMBOwnerRecord>.SaveFile(path + ".stage", record, error))
            return false;
        if (!CopyFile(path + ".stage", path))
            return false;
        TPMBOwnerRecord check = new TPMBOwnerRecord;
        if (!(JsonFileLoader<TPMBOwnerRecord>.LoadFile(path, check, error) && check.Owner == record.Owner && check.Type == record.Type && check.Token == record.Token)) return false;
        return RegisterLockOwner(bike,player);
    }

    static bool IsRegisteredOwner(MotorbikeScript bike, PlayerBase player)
    {
        string path = OwnerPath(bike);
        if (path == "" || !FileExist(path))
            return false;
        TPMBOwnerRecord record = new TPMBOwnerRecord;
        string error;
        if (!JsonFileLoader<TPMBOwnerRecord>.LoadFile(path, record, error))
            return false;
        return record.Type == bike.GetType() && record.Owner == player.GetIdentity().GetPlainId();
    }

    static void ForgetOwner(MotorbikeScript bike, PlayerBase player)
    {
        if (!IsRegisteredOwner(bike, player))
            return;
        string path = OwnerPath(bike);
        DeleteFile(path);
        if (FileExist(path + ".stage"))
            DeleteFile(path + ".stage");
    }

    // Two-phase file replacement with readback and backup. This is not an OS-atomic rename.
    static bool Save(int id, GarageData data)
    {
        string path = GM_DATABASE + id.ToString() + ".json";
        string stage = path + ".tlh-stage";
        string backup = path + ".tlh-backup";
        string error;
        if (!JsonFileLoader<GarageData>.SaveFile(stage, data, error))
        {
            Print("[TPMB] Save staging failed: " + error);
            return false;
        }
        GarageData check = new GarageData;
        if (!JsonFileLoader<GarageData>.LoadFile(stage, check, error) || !check || check.Vehicles.Count() != data.Vehicles.Count())
            return false;
        string expected;
        string actual;
        if (!JsonFileLoader<GarageData>.MakeData(data, expected, error) || !JsonFileLoader<GarageData>.MakeData(check, actual, error) || expected != actual)
            return false;
        if (FileExist(path) && !CopyFile(path, backup))
            return false;
        if (!CopyFile(stage, path))
        {
            if (FileExist(backup))
                CopyFile(backup, path);
            return false;
        }
        check = new GarageData;
        if (!JsonFileLoader<GarageData>.LoadFile(path, check, error) || !check || check.Vehicles.Count() != data.Vehicles.Count())
        {
            if (FileExist(backup))
                CopyFile(backup, path);
            return false;
        }
        if (!JsonFileLoader<GarageData>.MakeData(check, actual, error) || expected != actual)
        {
            if (FileExist(backup))
                CopyFile(backup, path);
            return false;
        }
        return true;
    }

    static GarageNPC NPC(PlayerBase player, vector requested)
    {
        foreach (GarageNPC npc : GetGarageConfig().NPCs)
        {
            if (npc.ParkingPosition == vector.Zero || npc.ParkingPosition == Vector(-1, -1, -1))
                continue;
            if (vector.Distance(npc.ParkingPosition, requested) > 0.25)
                continue;
            if (vector.Distance(player.GetPosition(), npc.Position) > 5.0)
                continue;
            return npc;
        }
        return null;
    }

    static MotorbikeScript Nearby(vector pos)
    {
        array<Object> objects = new array<Object>;
        GetGame().GetObjectsAtPosition3D(pos, 3.0, objects, null);
        foreach (Object object : objects)
        {
            MotorbikeScript bike = MotorbikeScript.Cast(object);
            if (bike && Supports(bike.GetType()))
                return bike;
        }
        return null;
    }

    static bool AllowedPart(string type)
    {
        if (type == "SparkPlug" || type == "HeadlightH7")
            return true;
        if (type == "Motorbike_01_Wheel_1" || type == "Motorbike_01_Wheel_2" || type == "Motorbike_02_Wheel_1" || type == "Motorbike_02_Wheel_2")
            return true;
        // Only exact vanilla panel names; never serialize keys, locks or custom scripted cargo.
        TStringArray colors = {"Blue", "Green", "Red", "Yellow"};
        foreach (string color : colors)
        {
            if (type == "Motorbike_02_ShieldFront_" + color || type == "Motorbike_02_ShieldLeft_" + color || type == "Motorbike_02_ShieldRight_" + color)
                return true;
        }
        return false;
    }

    static bool EnsurePart(MotorbikeScript bike, string type)
    {
        if (!AllowedPart(type))
            return false;
        TStringArray allowed = new TStringArray;
        GetGame().ConfigGetTextArray("CfgVehicles " + type + " inventorySlot", allowed);
        bool matched = false;
        for (int i = 0; i < bike.GetInventory().GetAttachmentSlotsCount(); i++)
        {
            int slot = bike.GetInventory().GetAttachmentSlotId(i);
            if (allowed.Find(InventorySlots.GetSlotName(slot)) == -1)
                continue;
            matched = true;
            EntityAI part = bike.GetInventory().FindAttachment(slot);
            if (!part)
                part = bike.GetInventory().CreateAttachmentEx(type, slot);
            if (!part || part.GetType() != type || part.GetHierarchyParent() != bike || part.IsRuined())
                return false;
        }
        return matched;
    }

    static bool Equip(MotorbikeScript bike)
    {
        TStringArray parts = TraderPlusHelper.GetVehicleParts(bike.GetType());
        if (!parts || parts.Count() == 0)
            return false;
        foreach (string configured : parts)
        {
            TStringArray tokens = new TStringArray;
            configured.Split(",", tokens);
            if (tokens.Count() == 0 || !EnsurePart(bike, tokens[0]))
                return false;
        }
        string type = bike.GetType();
        {
            string family = "Motorbike_02";
            if (type == "Motorbike_01_Blue" || type == "Motorbike_01_Red" || type == "Motorbike_01_Yellow")
                family = "Motorbike_01";
            if (!EnsurePart(bike, family + "_Wheel_1") || !EnsurePart(bike, family + "_Wheel_2") || !EnsurePart(bike, "HeadlightH7"))
                return false;
            if (bike.IsVitalSparkPlug() && !EnsurePart(bike, "SparkPlug"))
                return false;
        }
        return true;
    }

    static TPMBSnapshot Capture(MotorbikeScript bike)
    {
        #ifndef CARLOCKDISABLE
        if (!bike.m_TPCMReady) bike.TPCMLoad();
        if (bike.m_TPCMFault) return null;
        #endif
        CargoBase cargo = bike.GetInventory().GetCargo();
        if (cargo && cargo.GetItemCount() != 0)
            return null;
        TPMBSnapshot data = new TPMBSnapshot;
        bike.GetDamageZones(data.Zones);
        foreach (string zone : data.Zones)
            data.ZoneHealth.Insert(bike.GetHealth(zone, "Health"));
        for (int i = 0; i < bike.GetInventory().AttachmentCount(); i++)
        {
            EntityAI part = bike.GetInventory().GetAttachmentFromIndex(i);
            if (!part || !AllowedPart(part.GetType()) || part.GetInventory().AttachmentCount() > 0)
                return null;
            CargoBase nested = part.GetInventory().GetCargo();
            if (nested && nested.GetItemCount() > 0)
                return null;
            InventoryLocation location = new InventoryLocation;
            part.GetInventory().GetCurrentInventoryLocation(location);
            if (location.GetType() != InventoryLocationType.ATTACHMENT)
                return null;
            TPMBPart saved = new TPMBPart;
            saved.Type = part.GetType();
            saved.Slot = location.GetSlot();
            saved.Health = part.GetHealth();
            ItemBase item = ItemBase.Cast(part);
            if (item)
            {
                saved.Quantity = item.GetQuantity();
                saved.Wet = item.GetWet();
                if (item.GetCompEM())
                    saved.Energy = item.GetCompEM().GetEnergy();
            }
            data.Parts.Insert(saved);
        }
        return data;
    }

    static bool Restore(MotorbikeScript bike, VehiculeData entry)
    {
        TPMBSnapshot data = entry.TPMBData;
        if (!data || !data.Parts || !data.Zones || !data.ZoneHealth || data.Zones.Count() != data.ZoneHealth.Count())
            return false;
        foreach (TPMBPart saved : data.Parts)
        {
            if (!saved || !AllowedPart(saved.Type))
                return false;
            EntityAI part = bike.GetInventory().CreateAttachmentEx(saved.Type, saved.Slot);
            if (!part || bike.GetInventory().FindAttachment(saved.Slot) != part || part.GetHierarchyParent() != bike)
                return false;
            part.SetHealth("", "", saved.Health);
            ItemBase item = ItemBase.Cast(part);
            if (item)
            {
                if (item.HasQuantity())
                    item.SetQuantity(saved.Quantity);
                item.SetWet(saved.Wet);
                if (item.GetCompEM())
                    item.GetCompEM().SetEnergy(saved.Energy);
            }
        }
        bike.SetHealth("", "", entry.EngineHealth);
        for (int i = 0; i < data.Zones.Count(); i++)
            bike.SetHealth(data.Zones[i], "Health", data.ZoneHealth[i]);
        if (Supports(entry.VehicleName))
        {
            bike.LeakAll(MotorbikeFluid.FUEL);
            bike.Fill(MotorbikeFluid.FUEL, entry.FuelAmmount);
            float restoredFuel=bike.GetFluidFraction(MotorbikeFluid.FUEL)*bike.GetFluidCapacity(MotorbikeFluid.FUEL);
            Print("[TPMB s01] Restore fuel: "+bike.GetType()+" / expected="+entry.FuelAmmount.ToString()+" / actual="+restoredFuel.ToString());
            // Final fuel verification runs on subsequent server ticks before payment.
        }
        return RestoreLockSnapshot(bike,entry);
    }

    static VehiculeData Entry(MotorbikeScript bike, string state)
    {
        TPMBSnapshot snapshot = Capture(bike);
        if (!snapshot)
            return null;
        array<ref ItemsCargo> empty = new array<ref ItemsCargo>;
        float fuel = 0;
        if (Supports(bike.GetType()))
        {
            float capacity=bike.GetFluidCapacity(MotorbikeFluid.FUEL);
            if (capacity<=0) return null;
            fuel = bike.GetFluidFraction(MotorbikeFluid.FUEL) * capacity;
            if (state=="Buying")
            {
                fuel=capacity;
                Print("[TPMB s01] Purchase fuel target: "+bike.GetType()+" / "+fuel.ToString());
            }
        }
        int timestamp = 0;
        if (GetGarageConfig().UseGarageOnlyToTrade)
            timestamp = TraderPlusHelper.GetTimeStamp();
        VehiculeData entry = new VehiculeData(bike.GetType(), 0, 0, -1, bike.GetHealth(), fuel, empty, timestamp);
        entry.TPMBVersion = 1;
        entry.TPMBState = state;
        entry.TPMBToken = Token();
        entry.TPMBData = snapshot;
        if (!SaveLockSnapshot(bike,entry)) return null;
        return entry;
    }

    static bool Purchase(PlayerBase player, string type, int condition, out string token)
    {
        int id = PlayerId(player);
        GarageData garage = GarageData.Load(id.ToString(), player.GetIdentity().GetName());
        if (!garage || garage.Vehicles.Count() >= GetGarageConfig().MaxVehicleStored)
        {
            Notify(player, "Garage cheio. Compra cancelada.");
            return false;
        }
        MotorbikeScript bike = MotorbikeScript.Cast(GetGame().CreateObject(type, vector.Zero, false, false, true));
        if (!bike)
            return false;
        if (!Equip(bike))
        {
            GetGame().ObjectDelete(bike);
            Notify(player, "Configuracao de pecas invalida. Compra cancelada.");
            return false;
        }
        bike.SetHealth("", "", TraderPlusHelper.GetDamageStateFromLevel(condition) * bike.GetMaxHealth());
        if (Supports(type))
            bike.Fill(MotorbikeFluid.FUEL, bike.GetFluidCapacity(MotorbikeFluid.FUEL));
        VehiculeData entry = Entry(bike, "Buying");
        GetGame().ObjectDelete(bike);
        if (!entry)
            return false;
        token = entry.TPMBToken;
        garage.Vehicles.Insert(entry);
        if (!Save(id, garage))
        {
            Notify(player, "Falha ao salvar no Garage. Compra cancelada; avise a administracao.");
            return false;
        }
        Print("[TPMB] Purchase staged: " + type + " / " + entry.TPMBToken);
        return true;
    }
};

modded class TraderPlusHelper
{
    override static int CheckItemType(string classname)
    {
        if (TPMB.Supports(classname))
            return TraderPlusItemType.VEHICLE;
        return super.CheckItemType(classname);
    }

    override static bool CreateInInventoryWithState(PlayerBase player, string classname, int state = 0, int quantity = 1, int collateralMoney = 0)
    {
        if (!TPMB.Supports(classname))
            return super.CreateInInventoryWithState(player, classname, state, quantity, collateralMoney);
        // Supported purchases must use TPMBBuy, which confirms payment and stock.
        return false;
    }
};

modded class TraderPlusServer
{
    override bool BuyRequest(TraderPlusProduct product)
    {
        if (product && TPMB.Supports(product.ClassName))
        {
            Print("[TPMB] BuyRequest standalone-s01: type=" + product.ClassName + " multiplier=" + product.Multiplier.ToString() + " internalQty=" + product.Quantity.ToString() + " collateral=" + product.CollateralMoney.ToString() + " useGarage=" + GetTraderPlusConfigServer().UseGarageToTradeCar.ToString());
            if (!GetTraderPlusConfigServer().UseGarageToTradeCar)
            {
                TPMB.Notify(product.Customer, "Compra recusada: a configuracao carregada nao habilita compra pelo Garage.");
                return false;
            }
            if (product.Multiplier != 1)
            {
                TPMB.Notify(product.Customer, "Compra recusada: selecione um veiculo por transacao.");
                return false;
            }
            if (product.CollateralMoney != 0)
            {
                TPMB.Notify(product.Customer, "Compra recusada: desative o seguro para motos.");
                return false;
            }
            // Native CalculateTradeQty clamps configured 1 to varQuantityMax=0
            // for vehicles. Multiplier, not this internal quantity, counts vehicles.
            if (product.Quantity != 0 && product.Quantity != 1)
            {
                TPMB.Notify(product.Customer, "Compra recusada: quantidade interna inesperada. Consulte a linha BuyRequest r04 no log.");
                return false;
            }
            return TPMBBuy(product);
        }
        return super.BuyRequest(product);
    }

    bool TPMBBuy(TraderPlusProduct product)
    {
        PlayerBase player = product.Customer;
        if (!GetGame().IsServer() || !player || !player.GetIdentity() || product.Price < 0)
            return false;
        int money;
        if (!TraderPlusHelper.CheckifPlayerHasEnoughMoney(player, product.Price, product.TraderID, money))
            return false;
        TraderPlusCategory category;
        int stockIndex = -1;
        TStringArray stockTokens = new TStringArray;
        if (product.MaxStock != TRADEMODE_UNLIMITED)
        {
            category = TraderPlusHelper.GetStockCategory(product.TraderID, product.Category, false);
            if (!category)
                return false;
            for (int i = 0; i < category.Products.Count(); i++)
            {
                stockTokens.Clear();
                category.Products[i].Split(" ", stockTokens);
                if (stockTokens.Count() >= 3 && stockTokens[0] == product.ClassName && stockTokens[2].ToInt() == product.Health && stockTokens[1].ToInt() >= 1)
                {
                    stockIndex = i;
                    break;
                }
            }
            if (stockIndex < 0)
                return false;
        }
        string token;
        if (!TPMB.Purchase(player, product.ClassName, product.Health, token))
            return false;
        int id = TPMB.PlayerId(player);
        GarageData garage = GarageData.Load(id.ToString(), player.GetIdentity().GetName());
        int entryIndex = -1;
        for (int j = 0; j < garage.Vehicles.Count(); j++)
        {
            if (garage.Vehicles[j].TPMBToken == token)
                entryIndex = j;
        }
        if (entryIndex < 0)
            return false;
        if (product.Price > 0 && !TraderPlusHelper.RemoveMoneyFromPlayer(player, product.Price, product.TraderID))
        {
            garage.Vehicles.RemoveOrdered(entryIndex);
            TPMB.Save(id, garage);
            return false;
        }
        if (category)
        {
            int remaining = stockTokens[1].ToInt() - 1;
            if (remaining == 0)
                category.Products.RemoveOrdered(stockIndex);
            else
                category.Products[stockIndex] = stockTokens[0] + " " + remaining.ToString() + " " + stockTokens[2];
            string folder = TRADERPLUS_DB_DIR_SERVER + "ID_" + product.TraderID.ToString() + "\\";
            string stockPath = folder + product.Category + ".json";
            string error;
            bool written = JsonFileLoader<TraderPlusCategory>.SaveFile(stockPath + ".tlh-stage", category, error);
            bool backed = false;
            if (written)
            {
                backed = CopyFile(stockPath, stockPath + ".tlh-backup");
                written = backed;
            }
            if (written)
                written = CopyFile(stockPath + ".tlh-stage", stockPath);
            TraderPlusCategory readback = new TraderPlusCategory;
            if (written)
                written = JsonFileLoader<TraderPlusCategory>.LoadFile(stockPath, readback, error);
            if (written)
            {
                written = readback.Products.Count() == category.Products.Count();
                for (int k = 0; written && k < category.Products.Count(); k++)
                    written = readback.Products[k] == category.Products[k];
            }
            if (!written)
            {
                if (backed)
                    CopyFile(stockPath + ".tlh-backup", stockPath);
                TraderPlusHelper.AddMoneyToPlayer(player, product.Price, product.TraderID);
                garage.Vehicles.RemoveOrdered(entryIndex);
                TPMB.Save(id, garage);
                TPMB.Notify(player, "Falha ao confirmar estoque. Compra cancelada; confira eventual pendencia com a administracao.");
                return false;
            }
        }
        garage.Vehicles[entryIndex].TPMBState = "Stored";
        if (!TPMB.Save(id, garage))
        {
            TPMB.Notify(player, "Compra paga, mas registro Buying pendente. Nao repita: solicite verificacao administrativa.");
            return false;
        }
        Print("[TPMB] Purchase committed: " + product.ClassName + " / " + token);
        return true;
    }

    override bool SellRequest(TraderPlusProduct product)
    {
        // Original vehicle sale removes a Garage entry through an unchecked save.
        if (product && TPMB.Supports(product.ClassName))
        {
            TPMB.Notify(product.Customer, "Revenda de duas rodas ainda indisponivel neste candidato.");
            return false;
        }
        return super.SellRequest(product);
    }
};
