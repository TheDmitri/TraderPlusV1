class TraderPlusServer
{
    ref array<ref TraderPlusStock> TraderStocks;
    ref array<ref ProductQueue> StockQueues;

    ref array<Object> ObjectTraders;

    float TraderTick;

    ref map<int, int> QueueID;

    void TraderPlusServer()
    {
      GetTraderPlusLogger().LogInfo("TraderPlusServer - Started");
      TraderStocks      = new array<ref TraderPlusStock>;
      QueueID           = new map<int, int>;
      StockQueues       = new array<ref ProductQueue>;
      ObjectTraders     = new array<Object>;
      TraderTick        = 0;

      //I usually make a small delay to make sure that my static config class is available
      GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(initTraderPlusServer, 1000);
    }

    void  ~TraderPlusServer(){
    }

    void OnUpdate(float timeslice)
    {
      TraderTick+=timeslice;
      if(TraderTick > TRADERPLUS_QUEUE_TRANSACTION_INTERVAL)
      {
        foreach(ProductQueue queue: StockQueues)
        {
          if(queue.HasNextQueue())
          {
            TradeRequest(queue.Peek());
            queue.DeQueue();
          }
        }
        TraderTick = 0;
      }
    }

    void DestockHandler()
    {
      if(!GetTraderPlusPriceConfig().EnableAutoDestockAtRestart)
        return;

      array<ref TraderPlusIDs>IDs = GetTraderPlusIDConfig().IDs;
      foreach(int idx, TraderPlusIDs tpId: IDs)
      {
        foreach(string category: tpId.Categories)
        {
          TraderPlusCategory tpStockCategory = TraderPlusHelper.GetStockCategory(tpId.Id,category);
          TStringArray tempProducts = new TStringArray;
          foreach(int jdx, string product: tpStockCategory.Products)
          {
            string line = product;
            TStringArray token = new TStringArray;
            line.Split(" ", token);
            TraderPlusPriceSettings traderPlusPriceSettings = GetTraderPlusPriceConfig();
            GetTraderPlusLogger().LogDebug("ID:" + tpId.Id + "look for " + token[0] + " in priceconfig");

            foreach(TraderPlusCategory tpCategoryPrice: traderPlusPriceSettings.TraderCategories)
            {
              if(tpCategoryPrice.CategoryName != category)continue;
              TStringArray tpPriceConfigProducts = new TStringArray;
              tpPriceConfigProducts = tpCategoryPrice.Products;
              foreach(string priceproduct: tpPriceConfigProducts)
              {
                TStringArray strs = new TStringArray;
                string productline = priceproduct;
                productline.Split(",", strs);
                if(strs[0] == token[0])
                {
                  GetTraderPlusLogger().LogDebug("ID:" + tpId.Id + strs[0] + " = " + token[0]);
                  if(token[1].ToInt() >= strs[2].ToInt())
                  {
                    GetTraderPlusLogger().LogDebug(token[1] + " = " + strs[2]);
                    if(strs.Count() >= 7)
                    {
                      GetTraderPlusLogger().LogDebug(" strs.count = " + strs.Count());
                      if(strs[6].ToFloat() != 1.0)
                      {
                        token[1] = (token[1].ToInt() - Math.Round(token[1].ToInt() * strs[6].ToFloat())).ToString();
                        GetTraderPlusLogger().LogDebug("new stock value:" + token[1]);
                        string result_data = token[0] + " " + token[1] + " " + token[2];
                        tempProducts.Insert(result_data);
                      }
                    }
                  }
                  else
                  {
                    tempProducts.Insert(token[0] + " " + token[1] + " " + token[2]);
                  }
                }
              }
            }
          }
          tpStockCategory.Products.Clear();
          tpStockCategory.Products = tempProducts;
          tpStockCategory.Save(tpId.Id);
        }
      }
    }

    void DefaultTraderStock()
    {
      GetTraderPlusLogger().LogInfo("DefaultTraderStock - Started");
      if(GetTraderPlusPriceConfig().EnableDefaultTraderStock == 0)
            return;

      array<ref TraderPlusIDs>tpIDs = GetTraderPlusIDConfig().IDs;

      foreach(int idx, TraderPlusIDs tpId: tpIDs)
      {
        GetTraderPlusLogger().LogDebug("DefaultTraderStock id:"+tpId.Id.ToString());
        foreach(string category: tpId.Categories)
        {
          GetTraderPlusLogger().LogDebug("DefaultTraderStock category:"+category);
          TraderPlusCategory tpStockCategory = new TraderPlusCategory(category);

          TraderPlusPriceSettings traderPlusPriceSettings = GetTraderPlusPriceConfig();

          foreach(TraderPlusCategory tpCategoryPrice: traderPlusPriceSettings.TraderCategories)
          {
            if(tpCategoryPrice.CategoryName != category)continue;
            TStringArray tpPriceConfigProducts = new TStringArray;
            tpPriceConfigProducts = tpCategoryPrice.Products;
            foreach(string product: tpPriceConfigProducts)
            {
              TStringArray strs = new TStringArray;
              string productline = product;
              productline.Split(",", strs);
              int maxStock = strs[2].ToInt();
              if(maxStock > 0)
              {
                string data = "";
                if(GetTraderPlusPriceConfig().EnableDefaultTraderStock == 2)
                {
                  int randomStock = Math.RandomIntInclusive(0,maxStock);
                  data = strs[0] + " " + randomStock.ToString() + " " + "0";
                }
                else
                {
                  data = strs[0] + " " + strs[2] + " " + "0";
                }
                tpStockCategory.Products.Insert(data);
              }
            }
          }
          tpStockCategory.Save(tpId.Id);
        }
        GetTraderPlusLogger().LogDebug("DefaultTraderStock save");
      }
      GetTraderPlusLogger().LogDebug("DefaultTraderStock - Ended");
    }

    void initTraderPlusServer()
    {
      LoadStockConfig();
      SetTradersSpawner();
      SetObjectSpawner();
      DefaultTraderStock();
      DestockHandler();
    }

    void LoadStockConfig()
    {
      int count = GetTraderPlusIDConfig().IDs.Count();
      GetTraderPlusLogger().LogDebug("IDS count: " + count.ToString());
      for(int i=0;i<count;i++)
      {
        QueueID.Set(GetTraderPlusIDConfig().IDs[i].Id,i);
        StockQueues.Insert(new ProductQueue());
        foreach(string category: GetTraderPlusIDConfig().IDs[i].Categories)
        {
          TraderPlusCategory tpStockCategory = TraderPlusHelper.GetStockCategory(GetTraderPlusIDConfig().IDs[i].Id,category);
        }
      }
    }

    void SetTradersSpawner()
    {
      GetTraderPlusLogger().LogDebug("SetTradersSpawner: Started");

      array<ref TraderPlusData>Traders = GetTraderPlusConfigServer().Traders;
      for(int i = 0;i<Traders.Count();i++)
      {
        string objetName = Traders.Get(i).Name;
    		Object obj = GetGame().CreateObject(objetName, vector.Zero, false, false);
    		if (!obj)
        {
          GetTraderPlusLogger().LogError("obj was not created: "+objetName + " please make sure the syntaxe is correct!");
          continue;
        }
        GetTraderPlusLogger().LogInfo("obj created: "+objetName);
        ObjectTraders.Insert(obj);

        PlayerBase TraderPlayer = PlayerBase.Cast(obj);
    		if (TraderPlayer)
        {
          GetTraderPlusLogger().LogInfo("TraderPlayer created and added!: ");
          TraderPlayer.SetupTrader(Traders.Get(i));
          continue;
        }

        BuildingBase TraderBuilding = BuildingBase.Cast(obj);
    		if (TraderBuilding)
        {
          GetTraderPlusLogger().LogInfo("TraderStatic created and added!: ");
          TraderBuilding.SetupTrader(Traders.Get(i));
        }
        else
        {
          GetTraderPlusLogger().LogError("TraderStatic was NOT created ! Make sure your static object extends BuildingBase as the documentation tells you!");
        }
      }

      GetTraderPlusLogger().LogDebug("SetTradersSpawner: Ended");
    }

    void SetObjectSpawner()
    {
      GetTraderPlusLogger().LogDebug("SetObjectSpawner: Started");
      array<ref TraderPlusObject>TraderObjects = GetTraderPlusConfigServer().TraderObjects;
      for(int i = 0;i<TraderObjects.Count();i++)
      {
        array<Object> excluded = new array<Object>;
        array<Object> nearby   = new array<Object>;
        GetGame().IsBoxColliding(TraderObjects[i].Position - "0.5 0.5 0.5", vector.Zero, "1 1 1", excluded, nearby);
        bool exists = false;
        foreach (Object nearby_object : nearby)
        {
          if (TraderObjects[i].ObjectName == nearby_object.GetType())
          {
            exists = true;
            break;
          }
        }

        if (!exists)
        {
          string objetName = TraderObjects[i].ObjectName;
          Object obj = GetGame().CreateObject(objetName, vector.Zero, false, false);
          if (!obj)
          {
            GetTraderPlusLogger().LogError("obj was not created: "+objetName + " please make sure the syntaxe is correct!");
            continue;
          }

          obj.SetPosition(TraderObjects[i].Position);
          obj.SetOrientation(TraderObjects[i].Orientation);

          obj.SetOrientation(obj.GetOrientation());
          obj.Update();
        }
      }
      GetTraderPlusLogger().LogDebug("SetObjectSpawner: Ended");
    }

    bool BuyRequest(TraderPlusProduct product)
    {
      product.Debug();
      bool state;
      int moneyAmount,substract_result;
      string line, result_data;
      TStringArray token = new TStringArray;

      bool canTrade = TraderPlusHelper.CheckifPlayerHasEnoughMoney(product.Customer,product.Price, product.TraderID, moneyAmount);
      if(!canTrade)return false;


      if(product.MaxStock == TRADEMODE_UNLIMITED)
      {
        state = TraderPlusHelper.CreateInInventoryWithState(product.Customer, product.ClassName, product.Health,product.Quantity, product.CollateralMoney);
        if(!state)return false;
        TraderPlusHelper.RemoveMoneyFromPlayer(product.Customer,product.Price, product.TraderID);
        return true;
      }

      TraderPlusCategory tpStockCategory = TraderPlusHelper.GetStockCategory(product.TraderID,product.Category, false);
      TStringArray stockProducts = tpStockCategory.Products;
      //we look for our product in stock in the category it's suppose to be
      for(int i=0; i<stockProducts.Count();i++)
      {
        line = stockProducts[i];
        line.Split(" ",token);
        if(token[0] == product.ClassName && token[2] == product.Health.ToString())
        {
					if(token[1].ToInt()<product.Multiplier)
          {
            return false;
          }
          else
          {
            state = TraderPlusHelper.CreateInInventoryWithState(product.Customer, product.ClassName, product.Health,product.Quantity, product.CollateralMoney);
            if(!state)return false;
            TraderPlusHelper.RemoveMoneyFromPlayer(product.Customer,product.Price,product.TraderID);
            substract_result = token[1].ToInt() - Math.AbsInt(product.Multiplier);
            if(substract_result == 0)
            {
              tpStockCategory.Products.RemoveOrdered(i);
            }
            else
            {
              result_data = token.Get(0) + " " + substract_result.ToString() + " " + token.Get(2);
              tpStockCategory.Products.Set(i, result_data);
            }
            tpStockCategory.Save(product.TraderID);
            return true;
          }
        }
        token.Clear();
      }
      return false;
    }

    bool SellInsuranceRequest(TraderPlusProduct product)
    {
      product.Debug();

      TraderPlusBankingData account = product.Customer.GetBankAccount();

      if(account && account.Insurances[product.CollateralMoney])
      {
        account.Insurances.Remove(product.CollateralMoney);
        account.UpdateAccount(product.Customer);
        TraderPlusHelper.AddMoneyToPlayer(product.Customer,product.Price,product.TraderID);

        return true;
      }

      return false;
    }

    bool SellRequest(TraderPlusProduct product)
    {
      //#ifdef TRADERPLUSDEBUG
      product.Debug();
      //#endif
      bool state;
      string data;

      if(product.MaxStock == TRADEMODE_UNLIMITED)
      {
        if(product.TradMode == TRADEMODE_SELLVEHICLE)
            state = TraderPlusHelper.RemoveVehicleFromGarage(product.Customer, product.ClassName, product.Quantity, product.Health);
        else
            state = TraderPlusHelper.RemoveOurProduct(product.Customer, product.ClassName, product.Quantity, product.Health, false, product.Multiplier);

        if(state)
            TraderPlusHelper.AddMoneyToPlayer(product.Customer,product.Price,product.TraderID);

        return state;
      }

      //we don't store infinite stock item to reduce the stock file size
      state = CheckStock(product);
      if(state)return true;

      if(product.TradMode == TRADEMODE_SELLVEHICLE)
          state = TraderPlusHelper.RemoveVehicleFromGarage(product.Customer, product.ClassName, product.Quantity, product.Health);
      else
          state = TraderPlusHelper.RemoveOurProduct(product.Customer, product.ClassName, product.Quantity, product.Health, false, product.Multiplier);

      if(state)
      {
        //the stock doesn't contain our item so we can add it to our stock
        GetTraderPlusLogger().LogDebug("the stock doesn't contain our item so we can add it to our stock");
        TraderPlusCategory tpStockCategory = TraderPlusHelper.GetStockCategory(product.TraderID,product.Category, false);

        TraderPlusHelper.AddMoneyToPlayer(product.Customer,product.Price,product.TraderID);
        if(GetTraderPlusConfigServer().StoreOnlyToPristineState)
            data = product.ClassName + " " + product.Multiplier.ToString() +" "+"0";
        else
            data = product.ClassName + " " + product.Multiplier.ToString() +" "+product.Health.ToString();
        tpStockCategory.Products.Insert(data);
        tpStockCategory.Save(product.TraderID);

        return state;
      }

      return false;
    }

    bool CheckStock(TraderPlusProduct product)
    {
      GetTraderPlusLogger().LogDebug("CheckStock");

      int health = product.Health;

      //if server owner only wants to store item with a pristine state
      if(GetTraderPlusConfigServer().StoreOnlyToPristineState)
          health=0;

      TraderPlusCategory tpStockCategory = TraderPlusHelper.GetStockCategory(product.TraderID,product.Category, false);

      for(int j=0; j<tpStockCategory.Products.Count();j++)
      {
        TStringArray token = new TStringArray;
        string line = tpStockCategory.Products[j];
        line.Split(" ",token);

        string StockClassName = token[0];
        string ProductClassName = product.ClassName;
        StockClassName.ToLower();
        ProductClassName.ToLower();
        bool state = false;
        if(StockClassName == ProductClassName && token[2] == health.ToString())
        {
          if(product.TradMode == TRADEMODE_SELLVEHICLE)
              state = TraderPlusHelper.RemoveVehicleFromGarage(product.Customer, product.ClassName, product.Quantity, product.Health);
          else
              state = TraderPlusHelper.RemoveOurProduct(product.Customer, product.ClassName, product.Quantity, product.Health, false, product.Multiplier);

          if(state)
          {
            TraderPlusHelper.AddMoneyToPlayer(product.Customer,product.Price,product.TraderID);
            int substract_result = token[1].ToInt() + product.Multiplier;
            string data;
            if(GetTraderPlusConfigServer().StoreOnlyToPristineState)
                data = token[0] + " " + substract_result.ToString() + " " + "0";
            else
                data = token[0] + " " + substract_result.ToString() + " " + token[2];
            tpStockCategory.Products.Set(j, data);
            tpStockCategory.Save(product.TraderID);
            return true;
          }
          else
          {
            return false;
          }
        }
        token.Clear();
      }
      return false;
    }

    bool LicenceBuyHandler(TraderPlusProduct product)
    {
      if(product.ClassName.Contains(GetTraderPlusConfigServer().LicenceKeyWord))
      {
        if(product.TradMode == TRADEMODE_SELL)return false;
        foreach(string licence: GetTraderPlusConfigServer().Licences)
        {
          if(product.ClassName == licence)
          {
            TraderPlusBankingData account = product.Customer.GetBankAccount();
            if(account)
            {
              //In case licence is already there, who knows why ...
              if(account.Licences.Find(product.ClassName) == -1)
              {
                account.Licences.Insert(product.ClassName);
                TraderPlusHelper.RemoveMoneyFromPlayer(product.Customer,product.Price, product.TraderID );
                account.UpdateAccount(product.Customer);
                GetTraderPlusLogger().LogInfo("Transaction Buy Succeeded:"+product.Customer.GetIdentity().GetName()+" product:"+product.ClassName +" qty:"+(product.Quantity*product.Multiplier).ToString()+" health:"+product.Health.ToString()+" price:"+product.Price.ToString());
                GetRPCManager().SendRPC("TraderPlus", "GetStockResponseBasedOnID",  new Param3<int,TraderPlusStock, string>(TraderPlusResponse.BUY_SUCCESS,TraderPlusStock.GetStockFromID(product.TraderID, TraderPlusHelper.GetCategoriesFromID(product.TraderID)), product.Category), true, product.Customer.GetIdentity());
              }
              else
              {
                GetTraderPlusLogger().LogInfo("Transaction Buy Failed:"+product.Customer.GetIdentity().GetName()+" product:"+product.ClassName +" qty:"+(product.Quantity*product.Multiplier).ToString()+" health:"+product.Health.ToString()+" price:"+product.Price.ToString());
                GetRPCManager().SendRPC("TraderPlus", "GetStockResponseBasedOnID",  new Param3<int,TraderPlusStock, string>(TraderPlusResponse.BUY_FAILED,TraderPlusStock.GetStockFromID(product.TraderID, TraderPlusHelper.GetCategoriesFromID(product.TraderID)), product.Category), true, product.Customer.GetIdentity());
              }
            }
            else
            {
              GetTraderPlusLogger().LogInfo("Transaction Buy Failed:"+product.Customer.GetIdentity().GetName()+" product:"+product.ClassName +" qty:"+(product.Quantity*product.Multiplier).ToString()+" health:"+product.Health.ToString()+" price:"+product.Price.ToString());
              GetRPCManager().SendRPC("TraderPlus", "GetStockResponseBasedOnID",  new Param3<int,TraderPlusStock, string>(TraderPlusResponse.BUY_FAILED,TraderPlusStock.GetStockFromID(product.TraderID, TraderPlusHelper.GetCategoriesFromID(product.TraderID)), product.Category), true, product.Customer.GetIdentity());
            }
            return true;
          }
        }
      }
      return false;
    }

    void TradeRequest(TraderPlusProduct product)
    {
      if(!product.Customer)
        return;

      if(product.TradMode == TRADEMODE_SELL || product.TradMode == TRADEMODE_SELLVEHICLE || product.TradMode == TRADEMODE_SELLINSURANCE)
      {
        bool SellSucceed = false;
        if(product.TradMode == TRADEMODE_SELLINSURANCE)
          SellSucceed = SellInsuranceRequest(product);
        else
          SellSucceed = SellRequest(product);

        if(SellSucceed)
        {
          GetTraderPlusLogger().LogInfo("Transaction Sell Succeeded:"+product.Customer.GetIdentity().GetName()+" product:"+product.ClassName +" quantity:"+(product.Quantity*product.Multiplier).ToString()+" health:"+product.Health.ToString()+" price:"+product.Price.ToString());
          GetRPCManager().SendRPC("TraderPlus", "GetStockResponseBasedOnID",  new Param3<int,TraderPlusStock, string>(TraderPlusResponse.SELL_SUCCESS,TraderPlusStock.GetStockFromID(product.TraderID, TraderPlusHelper.GetCategoriesFromID(product.TraderID)), product.Category), true, product.Customer.GetIdentity());
        }
        else
        {
          GetTraderPlusLogger().LogInfo("Transaction Sell Failed:"+product.Customer.GetIdentity().GetName()+" product:"+product.ClassName +" quantity:"+(product.Quantity*product.Multiplier).ToString()+" health:"+product.Health.ToString()+" price:"+product.Price.ToString());
          GetRPCManager().SendRPC("TraderPlus", "GetStockResponseBasedOnID",  new Param3<int,TraderPlusStock, string>(TraderPlusResponse.SELL_FAILED,TraderPlusStock.GetStockFromID(product.TraderID, TraderPlusHelper.GetCategoriesFromID(product.TraderID)), product.Category), true, product.Customer.GetIdentity());
        }
      }
      else
      {
        bool BuySucceed = BuyRequest(product);
        if(BuySucceed)
        {
          GetTraderPlusLogger().LogInfo("Transaction Buy Succeeded:"+product.Customer.GetIdentity().GetName()+" product:"+product.ClassName +" quantity:"+(product.Quantity*product.Multiplier).ToString()+" health:"+product.Health.ToString()+" price:"+product.Price.ToString());
          GetRPCManager().SendRPC("TraderPlus", "GetStockResponseBasedOnID",  new Param3<int,TraderPlusStock, string>(TraderPlusResponse.BUY_SUCCESS,TraderPlusStock.GetStockFromID(product.TraderID, TraderPlusHelper.GetCategoriesFromID(product.TraderID)), product.Category), true, product.Customer.GetIdentity());
        }
        else
        {
          GetTraderPlusLogger().LogInfo("Transaction Buy Failed:"+product.Customer.GetIdentity().GetName()+" product:"+product.ClassName +" quantity:"+(product.Quantity*product.Multiplier).ToString()+" health:"+product.Health.ToString()+" price:"+product.Price.ToString());
          GetRPCManager().SendRPC("TraderPlus", "GetStockResponseBasedOnID",  new Param3<int,TraderPlusStock, string>(TraderPlusResponse.BUY_FAILED,TraderPlusStock.GetStockFromID(product.TraderID, TraderPlusHelper.GetCategoriesFromID(product.TraderID)), product.Category), true, product.Customer.GetIdentity());
        }
      }
    }

    void InsuranceRequestHandler(PlayerBase player)
    {
      GetTraderPlusLogger().LogDebug("InsuranceRequestHandler");
      TBoolArray insurancesStates = new TBoolArray;
      if(player)
      {
        TraderPlusBankingData account = player.GetBankAccount();
        if(account && account.Insurances && account.Insurances.Count() != 0)
        {
          foreach(int key, string value : account.Insurances)
          {
            CarScript car = CarScript.GetMapAll()[key];
            if(car)
              insurancesStates.Insert(true);
            else
              insurancesStates.Insert(false);
          }
        }
        GetRPCManager().SendRPC("TraderPlus", "GetInsuranceResponse",  new Param1<TBoolArray>(insurancesStates), true, player.GetIdentity());
      }
    }

    //-------------------------------------------RPC PART--------------------------------------------------------//
    void GetStockRequestBasedOnID(CallType type, ParamsReadContext ctx, PlayerIdentity sender, Object target)
    {
      if (!GetGame().IsServer())
        return;

      Param1<Object> data;
      if (!ctx.Read(data))
        return;

      int traderID=-1;

      PlayerBase TraderPlayer = PlayerBase.Cast(data.param1);
      if(TraderPlayer)
        traderID = TraderPlayer.TraderID;

      BuildingBase TraderBuilding = BuildingBase.Cast(data.param1);
      if (TraderBuilding)
        traderID = TraderBuilding.TraderID;

      if(traderID==-1)
        return;

      TraderPlusStock tpStock = new TraderPlusStock;
      tpStock = TraderPlusStock.GetStockFromID(traderID, TraderPlusHelper.GetCategoriesFromID(traderID));

  		GetTraderPlusLogger().LogDebug("Stock count: "+tpStock.TraderPlusItems.Count().ToString());

      GetRPCManager().SendRPC("TraderPlus", "GetStockResponseBasedOnID",  new Param3<int,TraderPlusStock, string>(TraderPlusResponse.NO_TRADE,tpStock, ""), true, sender);

      GetTraderPlusLogger().LogDebug("GetStockRequestBasedOnID by player:"+sender.GetName()); //sender.GetName() = player.GetIdentity().GetName()
    }

    void GetTradeRequest(CallType type,  ParamsReadContext ctx,  PlayerIdentity sender,  Object target)
    {
      if (!GetGame().IsServer())
        return;

      Param1<ref TraderPlusProduct> data;
      if (!ctx.Read(data))
        return;

      PlayerBase player = TraderPlusGetPlayerByIdentity(sender);
      TraderPlusProduct product = data.param1;
      product.Customer = player;

      if(LicenceBuyHandler(product))return;

      StockQueues.Get(QueueID[product.TraderID]).EnQueue(product);

      if(product.TradMode == TRADEMODE_BUY){
        GetTraderPlusLogger().LogDebug("GetBuyRequest:"+sender.GetName()); //sender.GetName() = player.GetIdentity().GetName()
      }
      else{
        GetTraderPlusLogger().LogDebug("GetSellRequest by player:"+sender.GetName()); //sender.GetName() = player.GetIdentity().GetName()
      }
    }

    void GetReceiptCarNameRequest(CallType type,  ParamsReadContext ctx,  PlayerIdentity sender,  Object target)
    {
      if (!GetGame().IsServer())
        return;

      PlayerBase player = TraderPlusGetPlayerByIdentity(sender);
      if(!player)
        return;

      array<EntityAI> itemsArray = new array<EntityAI>;
      player.GetInventory().EnumerateInventory(InventoryTraversalType.PREORDER, itemsArray);
      foreach(EntityAI entity: itemsArray)
      {
        NewReceiptBase receipt = NewReceipt.Cast(entity);
        if (receipt)
          GetRPCManager().SendRPC("TraderPlus", "GetCarNameReceiptResponse",  new Param2<NewReceipt, string>(receipt, receipt.CarClassName), true, player.GetIdentity());
      }
    }

    void GetInsuranceRequest(CallType type,  ParamsReadContext ctx,  PlayerIdentity sender,  Object target)
    {
      if (!GetGame().IsServer())
       return;

      PlayerBase player = TraderPlusGetPlayerByIdentity(sender);

      InsuranceRequestHandler(player);
    }
    //---------------------------------------------------------------------------------------------------------//
};
