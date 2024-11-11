class GarageServer
{
  ref array<Object> ObjectNPCs;


  void GarageServer()
  {
      ObjectNPCs = new array<Object>;
      GetGMLogger().LogInfo("GarageServer - Started");

      //I usually make a small delay to make sure that my static config class is available
      GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(initGarageServer, 500);
  }

  void initGarageServer()
  {
    SpawnNPCs();
    /*Do your code here*/
  }

  void SpawnNPCs()
  {
    for(int i = 0;i<GetGarageConfig().NPCs.Count();i++)
    {
      string objetName = GetGarageConfig().NPCs.Get(i).ClassName;
      Object obj = GetGame().CreateObject(objetName, vector.Zero, false, false);
      if (!obj) return;
      GetGMLogger().LogInfo("obj created: "+objetName);
      ObjectNPCs.Insert(obj);

      PlayerBase TraderPlayer = PlayerBase.Cast(obj);
      if (TraderPlayer)
      {
        GetGMLogger().LogInfo("TraderPlayer created and added!: ");
        TraderPlayer.SetupGarageNPC(GetGarageConfig().NPCs[i]);
        continue;
      }

      BuildingBase TraderBuilding = BuildingBase.Cast(obj);
      if (TraderBuilding)
      {
        GetGMLogger().LogInfo("TraderStatic created and added!: ");
        TraderBuilding.SetupGarageNPC(GetGarageConfig().NPCs[i]);
      }else GetGMLogger().LogInfo("TraderStatic was NOT created ! Make sure your static object extends BuildingBase as the documentation tells you!");
    }
  }

  void  ~GarageServer()
  {

  }

  void GarageRequestHandler(PlayerBase player, int id, vector pos,bool isFlag)
  {
    #ifdef GMDEBUG
    GetGMLogger().LogInfo("GarageRequestHandler");
    #endif

    int start = TickCount(0);
    GarageData garageData = GarageData.Load(id.ToString(), player.GetIdentity().GetName());
    int time = TickCount(start);
    #ifdef GMDEBUG
    GetGMLogger().LogInfo("loading time:"+time);
    #endif
    if(garageData)
    {
      TStringArray vehiclesName = new TStringArray;
      TIntArray vehiclesHealth = new TIntArray;

      if(GetGarageConfig().UseGarageOnlyToTrade)
      {
        for(int k = garageData.Vehicles.Count() - 1; k >= 0; k--)
        {
          if( (garageData.Vehicles[k].Timestamp + GetGarageConfig().SavedVehicleInGarageForTradeInHour*3600)  < TraderPlusHelper.GetTimeStamp() )
          {
            int playerId = GarageHelpers.GetLowSteamID(player.GetIdentity().GetPlainId());
            garageData.Vehicles[k].DeleteCargoIfExist(playerId);
            garageData.Vehicles.Remove(k);
            garageData.Save(playerId.ToString());
            NotificationSystem.SendNotificationToPlayerIdentityExtended(player.GetIdentity(), 2, "Garage", GetGarageConfig().TradeVehicleHasBeenDeleted , "Garage/image/CarLogo.paa");
          }
        }
      }

      foreach(VehiculeData vehicle: garageData.Vehicles)
      {
        vehiclesName.Insert(vehicle.VehicleName);
        vehiclesHealth.Insert(GarageHelpers.ConvertHealthToHealthLevel(vehicle.VehicleName, vehicle.EngineHealth));
      }

      vector parkingpos;
      if(isFlag)
      {
        parkingpos = garageData.ParkingPosition;
        #ifdef GMDEBUG
        GetGMLogger().LogInfo("parkingpos = garageData.ParkingPosition: "+garageData.ParkingPosition);
        #endif
      }
      else
      {
        for(int i=0;i<ObjectNPCs.Count();i++)
        {
          Object npc = ObjectNPCs[i];
          float distance = vector.Distance(npc.GetPosition(),pos);
          if(distance < 1)
          {
            parkingpos = GetGarageConfig().NPCs[i].ParkingPosition;
          }
        }
      }

      #ifdef GMDEBUG
      GetGMLogger().LogInfo("send GarageResponse");
      #endif

      GetRPCManager().SendRPC("Garage", "GarageResponse",  new Param4<ref TStringArray, ref TIntArray, vector, int>(vehiclesName, vehiclesHealth, parkingpos, TraderPlusResponse.NO_TRADE), true, player.GetIdentity());
    }
  }

  void ParkInRequestHandler(PlayerBase player, int id, CarScript car, vector pos)
  {
    #ifdef GMDEBUG
    GetGMLogger().LogInfo("ParkInRequestHandler");
    #endif

    if(!GarageHelpers.CheckifPlayerHasEnoughMoney(player, GetGarageConfig().ParkInCost))
    {
      NotificationSystem.SendNotificationToPlayerIdentityExtended(player.GetIdentity(), 2, "Garage", GetGarageConfig().NotEnoughMoney, "Garage/image/CarLogo.paa");
      return;
      //not enough money
    }

    
    #ifdef GMDEBUG
    GetGMLogger().LogInfo(string.Format("MustHaveLock:%1 - SaveLock:%2", GetGarageConfig().VehicleMustHaveLock, GetGarageConfig().SaveVehicleLock)); 
    #endif

    CarScript CarToRemove;
    if(!car)
      CarToRemove = GarageHelpers.CheckForCarAtPos(pos);
    else
      CarToRemove = car;

    if(!car)
    {
      #ifdef GMDEBUG
      GetGMLogger().LogInfo("ParkInRequestHandler => car doesnt exist");
      #endif
      return;
    }

    if(GetGarageConfig().VehicleMustHaveLock)
    {
      #ifndef CARLOCKDISABLE
        if(car.m_CarLockOwner == -1)
        {
          NotificationSystem.SendNotificationToPlayerIdentityExtended(player.GetIdentity(), 2, "Garage", GetGarageConfig().NotRightToPark, "Garage/image/CarLogo.paa");
          #ifdef GMDEBUG
          GetGMLogger().LogInfo("ParkInRequestHandler => no lock 1");
          #endif
          return;
        }
      #else
        #ifdef MuchCarKey
          if(!car.m_HasCKAssigned)
          {
            NotificationSystem.SendNotificationToPlayerIdentityExtended(player.GetIdentity(), 2, "Garage", GetGarageConfig().NotRightToPark, "Garage/image/CarLogo.paa");
            #ifdef GMDEBUG
            GetGMLogger().LogInfo("ParkInRequestHandler => no lock 2");
            #endif
            return;
          }
        #else
         #ifdef TRADER
           if(!car.m_Trader_HasKey)
           {
             NotificationSystem.SendNotificationToPlayerIdentityExtended(player.GetIdentity(), 2, "Garage", GetGarageConfig().NotRightToPark, "Garage/image/CarLogo.paa");
             #ifdef GMDEBUG
             GetGMLogger().LogInfo("ParkInRequestHandler => no lock 3");
             #endif
             return;
           }
         #endif
        #endif
      #endif
    }

    #ifdef CARLOCKDISABLE
      if( (!player.GetAdminStatus() == SZ_IS_ADMIN && car.m_TraderPlus_LastDriverID != player.GetIdentity().GetPlainId()) || car.m_TraderPlus_LastDriverID == string.Empty)
      {
        NotificationSystem.SendNotificationToPlayerIdentityExtended(player.GetIdentity(), 2, "Garage", GetGarageConfig().NotRightToPark, "Garage/image/CarLogo.paa");
        #ifdef GMDEBUG
        GetGMLogger().LogInfo("ParkInRequestHandler => no last driver");
        #endif
        return;
      }
    #endif

    if(GarageHelpers.CarHasMember(car))
    {
      NotificationSystem.SendNotificationToPlayerIdentityExtended(player.GetIdentity(), 2, "Garage", GetGarageConfig().CarHasMember, "Garage/image/CarLogo.paa");
      //car has member
      #ifdef GMDEBUG
      GetGMLogger().LogInfo("ParkInRequestHandler => contain member cant be parked");
      #endif
      return;
    }

    bool state = GarageHelpers.RemoveMoneyFromPlayer(player, GetGarageConfig().ParkInCost);
    if(!state)
    {
      #ifdef GMDEBUG
      GetGMLogger().LogInfo("ParkInRequestHandler => money removed failed");
      #endif
      return;
    }

    string VehicleName = CarToRemove.GetType();
    int    vehicleData = -1;
    float  VehicleHealth= CarToRemove.GetHealth();
    float  VehicleFuelAmount= CarToRemove.GarageGetFuelAmmount();

    if(GetGarageConfig().VehicleMustHaveLock || GetGarageConfig().SaveVehicleLock)
    {
      #ifndef CARLOCKDISABLE
        if(CarToRemove.m_CarLockPassword != -1)
        {
          vehicleData = CarToRemove.m_CarLockPassword;
        }
      #else
        #ifdef MuchCarKey
          if(CarToRemove.m_HasCKAssigned)
          {
            vehicleData = CarToRemove.m_CarKeyId;
          }
        #else
          #ifdef TRADER
            vehicleData = CarToRemove.m_Trader_VehicleKeyHash;
          #endif
        #endif
      #endif
    }

    #ifdef GMDEBUG
    GetGMLogger().LogInfo("Vehicle Data"+VehicleName + " " + vehicleData + " " + VehicleHealth + " " + VehicleFuelAmount);
    #endif

    GarageItemData cargo = GarageHelpers.GetGarageItemData(car);

    array<EntityAI> VehiclesAttachments = new array<EntityAI>;
    array<ref ItemsCargo> itemsArr = new array<ref ItemsCargo>;
    //VehiclesAttachments = GarageHelpers.GetAttachmentsFromEntityAI(CarToRemove);
    //itemsArr = GarageHelpers.ConvertEntitiesToItemsCargo(VehiclesAttachments, GARAGEVEHICLEATTACHMENT);

    int cargoId = 0;
    int timeStamp = 0;

    cargoId = TraderPlusHelper.GetTimeStamp();
    cargo.Save(id, cargoId);
    /*TraderPlusVehicleCargo vehicleCargo = TraderPlusVehicleCargo.Cast(GetGame().CreateObject("TraderPlusVehicleCargo", CarToRemove.GetPosition()));
    vehicleCargo = TraderPlusVehicleCargo.Cast(TraderPlusHelper.TransferInventoryVehicle(CarToRemove, vehicleCargo));
    vehicleCargo.SetCargoId(cargoId);
    vehicleCargo.SetPosition("0 0 0");*/

    if(GetGarageConfig().UseGarageOnlyToTrade)
      timeStamp = TraderPlusHelper.GetTimeStamp();

    int collateralMoney = 0;
    string TypeAndcollateralMoney = "";
    TraderPlusBankingData account = player.GetBankAccount();
    if(CarToRemove.GetCarUniqueId() != 0 && account.Insurances.Find(CarToRemove.GetCarUniqueId(), TypeAndcollateralMoney))
    {
        collateralMoney = TraderPlusHelper.GetCollateralPriceInsuranceData(TypeAndcollateralMoney);
        account.Insurances.Remove(CarToRemove.GetCarUniqueId());
        account.UpdateAccount(player);
    }

    GarageData garageData = GarageData.Load(id.ToString(), player.GetIdentity().GetName());
    if(garageData)
    {
      #ifdef GMDEBUG
      GetGMLogger().LogInfo("ADD VEHICULE");
      #endif
      garageData.AddVehicle(VehicleName, cargoId, collateralMoney, vehicleData, VehicleHealth, VehicleFuelAmount, itemsArr, timeStamp);
      garageData.Save(id.ToString());

      if(GetGarageConfig().UseGarageOnlyToTrade){
        NotificationSystem.SendNotificationToPlayerIdentityExtended(player.GetIdentity(), 2, "Garage", string.Format(GetGarageConfig().TradeVehicleWarning, GetGarageConfig().SavedVehicleInGarageForTradeInHour), "Garage/image/CarLogo.paa");
      }
      else{
        NotificationSystem.SendNotificationToPlayerIdentityExtended(player.GetIdentity(), 2, "Garage", GetGarageConfig().ParkInSuccess, "Garage/image/CarLogo.paa");
      }
    }
    else
    {
      NotificationSystem.SendNotificationToPlayerIdentityExtended(player.GetIdentity(), 2, "Garage", GetGarageConfig().ParkInFail, "Garage/image/CarLogo.paa");
    }

    GetGame().ObjectDelete(CarToRemove);

    TStringArray vehiclesName = new TStringArray;
    TIntArray vehiclesHealth = new TIntArray;

    foreach(VehiculeData vehicle: garageData.Vehicles)
    {
      vehiclesName.Insert(vehicle.VehicleName);
      vehiclesHealth.Insert(GarageHelpers.ConvertHealthToHealthLevel(vehicle.VehicleName, vehicle.EngineHealth));
    }

    GetRPCManager().SendRPC("Garage", "GarageResponse",  new Param4<ref TStringArray, ref TIntArray, vector, int>(vehiclesName, vehiclesHealth, pos, TraderPlusResponse.BUY_SUCCESS),true, player.GetIdentity());
  }

  void ParkOutRequestHandler(PlayerBase player, int id, int listpos,string carname, vector pos)
  {
    #ifdef GMDEBUG
    GetGMLogger().LogInfo("ParkOutRequestHandler");
    #endif

    #ifdef GMDEBUG
    GetGMLogger().LogInfo("ParkOutRequestHandler data: "+id + " " + listpos + " " + carname+ " " + pos);
    #endif

    if(!GarageHelpers.CheckifPlayerHasEnoughMoney(player, GetGarageConfig().ParkOutCost))
    {
      NotificationSystem.SendNotificationToPlayerIdentityExtended(player.GetIdentity(), 2, "Garage", GetGarageConfig().NotEnoughMoney, "Garage/image/CarLogo.paa");
      //not enough money
    }

    vector ori;
    GarageData garageData = GarageData.Load(id.ToString(), player.GetIdentity().GetName());
    if(garageData)
    {
      if(pos == garageData.ParkingPosition)
      {
        #ifdef GMDEBUG
        GetGMLogger().LogInfo("pos == garageData.ParkingPosition");
        #endif
        ori = garageData.ParkingOrientation;
      }
      else
      {
        for(int i=0;i<ObjectNPCs.Count();i++)
        {
          float distance = vector.Distance(GetGarageConfig().NPCs[i].ParkingPosition,pos);
          #ifdef GMDEBUG
          GetGMLogger().LogInfo(string.Format("distance: %1", distance));
          #endif
          if(distance < 3)
          {
            #ifdef GMDEBUG
            GetGMLogger().LogInfo("distance < 5");
            #endif
            pos = GetGarageConfig().NPCs[i].ParkingPosition;
            ori = GetGarageConfig().NPCs[i].ParkingOrientation;
          }
        }
      }

      if(!GarageHelpers.IsParkingAvailable(pos,ori))
      {
        NotificationSystem.SendNotificationToPlayerIdentityExtended(player.GetIdentity(), 2, "Garage", GetGarageConfig().ParkingNotAvailable, "Garage/image/CarLogo.paa");
        //park not available
        return;
      }

      if(garageData.Vehicles.Count() > listpos)
      {
        if(garageData.Vehicles[listpos].VehicleName == carname)
        {
          #ifdef GMDEBUG
          GetGMLogger().LogInfo("garageData.Vehicles[listpos].VehicleName == carname");
          #endif
          bool state = GarageHelpers.CreateVehicle(player, carname, garageData.Vehicles[listpos].VehicleCargoId, garageData.Vehicles[listpos].CollateralMoney, garageData.Vehicles[listpos].VehicleID, garageData.Vehicles[listpos].EngineHealth, garageData.Vehicles[listpos].FuelAmmount, pos, ori, garageData.Vehicles[listpos].ItemsInCargo);
          if(state)
          {
            state = GarageHelpers.RemoveMoneyFromPlayer(player, GetGarageConfig().ParkOutCost);
            NotificationSystem.SendNotificationToPlayerIdentityExtended(player.GetIdentity(), 2, "Garage", GetGarageConfig().ParkOutSuccess, "Garage/image/CarLogo.paa");
            garageData.Vehicles.RemoveOrdered(listpos);
            garageData.Save(id.ToString());
          }
        }
      }

      if(!state)
      {
        for(int j=0;j<garageData.Vehicles.Count();j++)
        {
          if(garageData.Vehicles[j].VehicleName == carname)
          {
            int collateralMoney = garageData.Vehicles[j].CollateralMoney;
            int vehicleCargoId = garageData.Vehicles[j].VehicleCargoId;
            state = GarageHelpers.CreateVehicle(player, carname,vehicleCargoId, collateralMoney, garageData.Vehicles[j].VehicleID, garageData.Vehicles[j].EngineHealth, garageData.Vehicles[j].FuelAmmount, pos, ori,garageData.Vehicles[j].ItemsInCargo);
            if(state)
            {
              state = GarageHelpers.RemoveMoneyFromPlayer(player, GetGarageConfig().ParkOutCost);
              NotificationSystem.SendNotificationToPlayerIdentityExtended(player.GetIdentity(), 2, "Garage", GetGarageConfig().ParkOutSuccess, "Garage/image/CarLogo.paa");
              garageData.Vehicles.RemoveOrdered(j);
              garageData.Save(id.ToString());
            }
            else
            {
              NotificationSystem.SendNotificationToPlayerIdentityExtended(player.GetIdentity(), 2, "Garage", GetGarageConfig().ParkOutFail, "Garage/image/CarLogo.paa");
            }
          }
        }
      }

      TStringArray vehiclesName = new TStringArray;
      TIntArray vehiclesHealth = new TIntArray;

      foreach(VehiculeData vehicle: garageData.Vehicles)
      {
        vehiclesName.Insert(vehicle.VehicleName);
        vehiclesHealth.Insert(GarageHelpers.ConvertHealthToHealthLevel(vehicle.VehicleName, vehicle.EngineHealth));
      }

      GetRPCManager().SendRPC("Garage", "GarageResponse",  new Param4<ref TStringArray, ref TIntArray, vector, int>(vehiclesName, vehiclesHealth, pos, TraderPlusResponse.BUY_SUCCESS),true, player.GetIdentity());
    }
  }

  //-------------------------------------------RPC PART--------------------------------------------------------//
  void GarageRequest(CallType type, ParamsReadContext ctx, PlayerIdentity sender, Object target)
  {
   if (!GetGame().IsServer())
     return;

     Param3<int, vector, bool> data;

     if (!ctx.Read(data))
       return;

     PlayerBase player = GMGetPlayerByIdentity(sender);

     if(player)
       GarageRequestHandler(player, data.param1, data.param2, data.param3);

     #ifdef GMDEBUG
     GetGMLogger().LogInfo("GarageRequest by player:"+sender.GetName()); //sender.GetName() = player.GetIdentity().GetName()
     #endif
  }

  void ParkInRequest(CallType type, ParamsReadContext ctx, PlayerIdentity sender, Object target)
  {
   if (!GetGame().IsServer())
     return;

     Param3<int, CarScript, vector> data;

     if (!ctx.Read(data))
       return;

     PlayerBase player = GMGetPlayerByIdentity(sender);

     if(player)
       ParkInRequestHandler(player, data.param1, data.param2, data.param3);

     #ifdef GMDEBUG
     GetGMLogger().LogInfo("ParkInRequest by player:"+sender.GetName()); //sender.GetName() = player.GetIdentity().GetName()
     #endif
  }

  void ParkOutRequest(CallType type, ParamsReadContext ctx, PlayerIdentity sender, Object target)
  {
   if (!GetGame().IsServer())
     return;

     Param4<int, int, string, vector> data;

     if (!ctx.Read(data))
       return;

     PlayerBase player = GMGetPlayerByIdentity(sender);

     if(player)
       ParkOutRequestHandler(player, data.param1, data.param2, data.param3, data.param4);

     #ifdef GMDEBUG
     GetGMLogger().LogInfo("ParkOutRequest by player:"+sender.GetName()); //sender.GetName() = player.GetIdentity().GetName()
     #endif
  }
  //---------------------------------------------------------------------------------------------------------//
};
