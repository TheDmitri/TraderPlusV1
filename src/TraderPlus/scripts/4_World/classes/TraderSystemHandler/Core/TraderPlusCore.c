class TraderPlusCore
{
	ref TraderPlusGeneralSettings     m_TraderPlusGeneralSettings;
	ref TraderPlusVehiclesSettings    m_TraderPlusVehiclesSettings;
	ref TraderPlusPriceSettings    	  m_TraderPlusPriceSettings;
	ref TraderPlusIDsSettings		  m_TraderPlusIDsSettings;
	ref TraderPlusInsuranceSettings   m_TraderPlusInsuranceSettings;

	ref TraderPlusGnrlConfigClient    m_TraderPlusGnrlConfigClient;

	ref TraderPlusCategories m_TraderPlusCategories;

  	ref TraderPlusClient m_TraderPlusClient;
  	ref TraderPlusServer m_TraderPlusServer;

	void TraderPlusCore()
    {
		GetTraderPlusLogger().LogInfo("TraderPlusCore - Started !");

		/*We load every config, class that will run on server side here*/
		if (GetGame().IsServer() || !GetGame().IsMultiplayer())
		{
			//LOADING CONFIG FILES...
			m_TraderPlusGeneralSettings = TraderPlusGeneralSettings.Load();
			m_TraderPlusVehiclesSettings = TraderPlusVehiclesSettings.Load();
			m_TraderPlusPriceSettings   = TraderPlusPriceSettings.Load();
			m_TraderPlusIDsSettings = TraderPlusIDsSettings.Load();
			m_TraderPlusInsuranceSettings = TraderPlusInsuranceSettings.Load();

			CheckForOldStockFile();

			m_TraderPlusCategories = new TraderPlusCategories;
			m_TraderPlusCategories.ConvertArrayCategoryToMap();

			m_TraderPlusGnrlConfigClient = new TraderPlusGnrlConfigClient;
			m_TraderPlusGnrlConfigClient.TransformToSendableConfig(m_TraderPlusGeneralSettings,m_TraderPlusIDsSettings,m_TraderPlusVehiclesSettings,m_TraderPlusInsuranceSettings);
			//Creating server side part
			m_TraderPlusServer = new TraderPlusServer;
		}
		else
		{
			/*Then we run every config and class that will run on client only*/
			m_TraderPlusClient = new TraderPlusClient;
			m_TraderPlusGnrlConfigClient = new TraderPlusGnrlConfigClient;
		}
		//we call our function that will initiaze our RPC
		InitRPC();
    }

	void InitRPC()
	{
		if (GetGame().IsServer())
		{
			//Server calls
			//GetRPCManager().AddRPC("ModName (try to be as specific as you can)", "NameOfYourRPCFunction", function where the RPC will be called, SingeplayerExecutionType.Client (Client when you do client to server));
			GetRPCManager().AddRPC("TraderPlus", "GetTradeRequest", m_TraderPlusServer, SingeplayerExecutionType.Client);
			GetRPCManager().AddRPC("TraderPlus", "GetStockRequestBasedOnID", m_TraderPlusServer, SingeplayerExecutionType.Client);
			GetRPCManager().AddRPC("TraderPlus", "GetCarNameReceiptRequest", this, SingeplayerExecutionType.Client);
			GetRPCManager().AddRPC("TraderPlus", "GetInsuranceRequest", m_TraderPlusServer, SingeplayerExecutionType.Client);
			GetRPCManager().AddRPC("TraderPlus", "GetReceiptCarNameRequest", m_TraderPlusServer, SingeplayerExecutionType.Client);
		}
		else
		{
			//Client calls
			//GetRPCManager().AddRPC("ModName (try to be as specific as you can)", "NameOfYourRPCFunction", function where the RPC will be called, SingeplayerExecutionType.Server (Server when you do Server to Client));
			GetRPCManager().AddRPC("TraderPlus", "GetInsuranceResponse", m_TraderPlusClient, SingeplayerExecutionType.Server);
			GetRPCManager().AddRPC("TraderPlus", "GetStockResponseBasedOnID", m_TraderPlusClient, SingeplayerExecutionType.Server);
			GetRPCManager().AddRPC("TraderPlus", "GetTraderPlusGnrlConfigClient", this, SingeplayerExecutionType.Server);
			GetRPCManager().AddRPC("TraderPlus", "GetPriceRequestFromCategory", m_TraderPlusClient, SingeplayerExecutionType.Server);
			GetRPCManager().AddRPC("TraderPlus", "GetCarNameReceiptResponse", this, SingeplayerExecutionType.Server);
		}
	}

	//Here we send gnrl config and price config to client
	void TransfertTraderPlusConfigToClient(PlayerBase player)
	{
		/*we create a temporary config class that we will send to client with RPC*/
		GetRPCManager().SendRPC("TraderPlus", "GetTraderPlusGnrlConfigClient",  new Param1<ref TraderPlusGnrlConfigClient>(m_TraderPlusGnrlConfigClient), true, player.GetIdentity());
		GetRPCManager().SendRPC("TraderPlus", "GetPriceRequestFromCategory",  new Param1<ref TraderPlusCategories>(m_TraderPlusCategories), true, player.GetIdentity());
	}

	void CheckForOldStockFile()
	{
		foreach(TraderPlusIDs id: m_TraderPlusIDsSettings.IDs)
		{
			oldTraderPlusStock oldStock = oldTraderPlusStock.Load(id.Id);
			if(oldStock)
				ConvertStock(oldStock, id.Id);
		}
	}

	void ConvertStock(oldTraderPlusStock stock, int id)
	{
		GetTraderPlusLogger().LogDebug("Convert old Stock to new Stock !");
		foreach(TraderPlusCategory category: m_TraderPlusPriceSettings.TraderCategories)
		{
			TraderPlusCategory tpStockCategory = TraderPlusHelper.GetStockCategory(id, category.CategoryName);

			foreach(string productInCategory: category.Products )
			{
				TStringArray categoryArr = new TStringArray;
				productInCategory.Split(",",categoryArr);
				foreach(string product: stock.TraderPlusItems)
				{
					TStringArray productArr = new TStringArray;
					product.Split(" ", productArr);
					if(categoryArr[0] == productArr[0])
						tpStockCategory.Products.Insert(product);
				}
			}
			if(tpStockCategory.Products.Count() == 0)continue;
			tpStockCategory.Save(id);
		}
		DeleteFile(TRADERPLUS_STOCK_CONFIG + "_" + id.ToString() + ".json");
	}


	//-------------------------------------------RPC PART--------------------------------------------------------//
	void GetTraderPlusGnrlConfigClient(CallType type, ParamsReadContext ctx, PlayerIdentity sender, Object target)
	{
		if(!GetGame().IsClient())return;

		Param1<ref TraderPlusGnrlConfigClient> data;
	    if (!ctx.Read(data))
	    	return;

		m_TraderPlusGnrlConfigClient = data.param1;
		#ifdef TRADERPLUSDEBUG
		JsonFileLoader<ref TraderPlusGnrlConfigClient>.JsonSaveFile(TRADERPLUS_CONFIG, m_TraderPlusGnrlConfigClient);
		#endif
		//we reset the price config before next RPC fills it
		m_TraderPlusClient.ResetPriceConfig();
	}

	void GetCarNameReceiptRequest(CallType type, ParamsReadContext ctx, PlayerIdentity sender, Object target)
	{
		if(!GetGame().IsServer())return;

		Param1<NewReceipt> data;
	    if (!ctx.Read(data))
	    return;

		PlayerBase player = TraderPlusGetPlayerByIdentity(sender);

		NewReceipt receipt = data.param1;
		if(receipt)
			GetRPCManager().SendRPC("TraderPlus", "GetCarNameReceiptResponse",  new Param2<NewReceipt, string>(receipt, receipt.CarClassName), true, player.GetIdentity());
	}

	void GetCarNameReceiptResponse(CallType type, ParamsReadContext ctx, PlayerIdentity sender, Object target)
	{
		if(!GetGame().IsClient())return;

		Param2<NewReceipt, string> data;
	    if (!ctx.Read(data))
	       return;

		NewReceipt receipt = data.param1;
	 	if(receipt)
			receipt.CarClassName = data.param2;
	}
};
