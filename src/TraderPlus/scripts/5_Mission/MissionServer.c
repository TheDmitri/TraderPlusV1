modded class MissionServer extends MissionBase
{
	void MissionServer()
	{
		m_TraderPlusCore = NULL;
		GetTraderPlusCore();
	}

	void ~MissionServer() {
	}

	override void OnInit()
	{
		super.OnInit();
		GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(RunAutoCalculation, 30000);
		#ifdef TRADER
		GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(ShowTraderVariables, 60000);
		#endif
	}

	void RunAutoCalculation()
	{
		if(!GetTraderPlusPriceConfig().EnableAutoCalculation)return;

		GetTraderPlusLogger().LogInfo("RunAutoCalculation - Started !");
		/*for(int i=0; i<GetTraderPlusPriceConfig().TraderCategories.Count();i++)
		{
			for(int j=0;j<GetTraderPlusPriceConfig().TraderCategories.GetElement(i).Count();j++)
			{
				TStringArray token = new TStringArray;
				string line = GetTraderPlusPriceConfig().TraderCategories.GetElement(i)[j];
				line.Split(",",token);
				int stock = token[2].ToInt();
				float BuyPrice           = token[4].ToInt();
				float BuyPriceStockLimit = BuyPrice;
				float coefficient = token[1].ToFloat();
				float SellPrice   = token[5].ToFloat();
				float SellPriceStockLimit = SellPrice;
				GetTraderPlusLogger().LogInfo(token[0]+token[1]+token[2]+token[3]+token[4]+token[5]);
				if(stock == -2)
				{
					GetTraderPlusLogger().LogInfo("stock == -2");
					stock = TraderPlusHelper.CalculateStockBasedOnCE(token[0]);
					if(token[0].Contains("Ammo_"))stock = stock/token[3].ToInt();
					if(SellPrice < 1.0 && SellPrice > 0)
					{
						GetTraderPlusLogger().LogInfo("SellPrice < 1.0 && SellPrice > 0");
						SellPrice = BuyPrice * SellPrice;
					}
					if(coefficient > 0.0 && coefficient < 1.0)
					{
						GetTraderPlusLogger().LogInfo("coefficient > 0 && coefficient < 0.9");
						BuyPriceStockLimit = BuyPriceStockLimit * coefficient;
						SellPriceStockLimit= SellPriceStockLimit * coefficient;
					}
					if(coefficient == -2.0)
					{
						BuyPriceStockLimit = BuyPriceStockLimit * 0.2;
						SellPriceStockLimit= SellPriceStockLimit * 0.2;
					}
					if(BuyPrice > 8)
					{
						coefficient = Math.Pow((BuyPriceStockLimit/BuyPrice), (1/(stock - 1)));
					}else if(SellPrice > 8 && BuyPrice == 0)
					{
						coefficient = Math.Pow((SellPriceStockLimit/SellPrice), (1/(stock - 1)));
					}else coefficient = 1.0;
				}
				if(stock < 0 && coefficient != -1.0)coefficient=-1.0;
				int sellprice = SellPrice;
				string data = token[0] + ","+ coefficient.ToString() + ","+stock.ToString()+"," + token[3] + "," + BuyPrice.ToString() + "," + sellprice.ToString();
				if(token.Count() > 6)data += ","+token[6];
				GetTraderPlusPriceConfig().TraderCategories.GetElement(i).Set(j, data);
				token.Clear();
			}
		}*/
		GetTraderPlusPriceConfig().Save();
		GetTraderPlusLogger().LogInfo("RunAutoCalculation - Ended !");
	}

	#ifdef TRADER
	void ShowTraderVariables()
	{
		if(!GetTraderPlusConfigServer().ConvertTraderConfigToTraderPlus)return;
		TraderPlusPriceSettings m_PriceConfig = new TraderPlusPriceSettings;
		GetTraderPlusLogger().LogInfo("Convertion Dr.Jones config to TraderPlus Config");

		TraderPlusGeneralSettings m_GeneralConfig = new TraderPlusGeneralSettings;
		m_GeneralConfig.DisableHeightFailSafeForReceiptDeployment = false;
		m_GeneralConfig.EnableShowAllCheckBox = true;
		m_GeneralConfig.ConvertTraderConfigToTraderPlus = false;
		m_GeneralConfig.ConvertTraderConfigToTraderPlusWithStockBasedOnCE = false;
		m_GeneralConfig.IsReceiptTraderOnly = true;
		m_GeneralConfig.LockPickChance = 0.3;
		m_GeneralConfig.LicenceKeyWord = "Licence";
    m_GeneralConfig.Licences.Insert("Car Licence");
		m_GeneralConfig.Licences.Insert("Admin Licence");
		m_GeneralConfig.AcceptedStates = new TraderPlusStates(true, true, true);
		m_GeneralConfig.StoreOnlyToPristineState = false;

		m_GeneralConfig.Currencies.Insert(new TraderPlusCurrency("TraderPlus_Money_Euro500,TraderPlus_Money_Ruble500",500));
		m_GeneralConfig.Currencies.Insert(new TraderPlusCurrency("TraderPlus_Money_Dollar100,TraderPlus_Money_Euro100,TraderPlus_Money_Ruble100",100));
		m_GeneralConfig.Currencies.Insert(new TraderPlusCurrency("TraderPlus_Money_Dollar50,TraderPlus_Money_Euro50,TraderPlus_Money_Ruble50",50));
		m_GeneralConfig.Currencies.Insert(new TraderPlusCurrency("TraderPlus_Money_Dollar20,TraderPlus_Money_Euro20",20));
		m_GeneralConfig.Currencies.Insert(new TraderPlusCurrency("TraderPlus_Money_Dollar10,TraderPlus_Money_Euro10,TraderPlus_Money_Ruble10",10));
		m_GeneralConfig.Currencies.Insert(new TraderPlusCurrency("TraderPlus_Money_Dollar5,TraderPlus_Money_Euro5,TraderPlus_Money_Ruble5,TraderPlus_Money_Ruble5_Coin",5));
		m_GeneralConfig.Currencies.Insert(new TraderPlusCurrency("TraderPlus_Money_Dollar2,TraderPlus_Money_Ruble2_Coin,TraderPlus_Money_Euro2",2));
		m_GeneralConfig.Currencies.Insert(new TraderPlusCurrency("TraderPlus_Money_Dollar1,TraderPlus_Coin,TraderPlus_Money_Ruble1_Coin,TraderPlus_Money_Euro1",1));

		for(int tn = 0; tn < m_Trader_TraderPositions.Count();tn++)
		{
			m_GeneralConfig.Traders.Insert(new TraderPlusData(m_Trader_TraderIDs[tn],"TraderPlusSign",m_Trader_TraderNames[m_Trader_TraderIDs[tn]],m_Trader_TraderNames[m_Trader_TraderIDs[tn]],m_Trader_TraderPositions[tn], Vector(0, 0,	0)));
		}
		m_GeneralConfig.Save();

		for(int i=0;i<m_Trader_Categorys.Count();i++)
		{
			m_PriceConfig.AddCategoryToConfig(m_Trader_Categorys.Get(i));
		}

		TraderPlusIDsSettings m_IDConfig = new TraderPlusIDsSettings;
		for(int l=0;l<m_Trader_TraderIDs.Count();l++)
		{
			m_IDConfig.AddID();
		}

		int prev = -1;
		for(int j=0;j<m_Trader_ItemsClassnames.Count();j++)
		{
			if(prev != m_Trader_ItemsCategoryId[j])
			{
				m_IDConfig.AddCategoryToID(m_Trader_ItemsTraderId[j],m_Trader_Categorys.Get(m_Trader_ItemsCategoryId[j]));
			}
			prev = m_Trader_ItemsCategoryId[j];
			int qty = GetQuantityBasedOnTraderQuantityHandler(m_Trader_ItemsQuantity[j]);
			if(qty == -1)continue;
			string strqty = qty.ToString();
			if(qty == -2)strqty = "0.75";
			int stock = -1;
			if(GetTraderPlusConfigServer().ConvertTraderConfigToTraderPlusWithStockBasedOnCE)
			{
				stock = TraderPlusHelper.CalculateStockBasedOnCE(m_Trader_ItemsClassnames[j]);
				if(m_Trader_ItemsClassnames[j].Contains("Ammo_"))stock = stock/qty;
			}
			float buyprice = m_Trader_ItemsBuyValue[j];
			float sellprice = m_Trader_ItemsSellValue[j];
			float coefficient=1.0,BuyPriceStockLimit, SellPriceStockLimit;
			if(buyprice > 8 && GetTraderPlusConfigServer().ConvertTraderConfigToTraderPlusWithStockBasedOnCE)
			{
				BuyPriceStockLimit = buyprice*0.2;
				coefficient = Math.Pow((BuyPriceStockLimit/buyprice), (1/(stock - 1)));
			}else if(buyprice == 0 && sellprice > 8 && GetTraderPlusConfigServer().ConvertTraderConfigToTraderPlusWithStockBasedOnCE)
			{
				SellPriceStockLimit = sellprice * 0.2;
				coefficient = Math.Pow((SellPriceStockLimit/sellprice), (1/(stock - 1)));
			}else coefficient = 1;
			string data = m_Trader_ItemsClassnames[j] + ","+ coefficient.ToString() + ","+stock.ToString()+"," + strqty + "," + m_Trader_ItemsBuyValue[j].ToString() + "," + m_Trader_ItemsSellValue[j].ToString();
			m_PriceConfig.AddProductToCategory(m_Trader_ItemsCategoryId[j],data);
		}
		m_PriceConfig.Save();

		for(int n=0;n<m_IDConfig.IDs.Count();n++)
		{
			for(int o=0;o<m_IDConfig.IDs.Count();o++)
			{
				if(m_IDConfig.IDs[o].Categories.Count()==0)
				{
					m_IDConfig.IDs.RemoveOrdered(o);
				}
			}
		}
		m_IDConfig.Save();

		TraderPlusVehiclesSettings m_VehicleConfig = new TraderPlusVehiclesSettings;
		for(int k=0;k<m_Trader_Vehicles.Count();k++)
		{
			m_VehicleConfig.AddVehicleToConfig(m_Trader_Vehicles[k]);
		}

		for(int m=0;m<m_Trader_VehiclesParts.Count();m++)
		{
			m_VehicleConfig.AddAttachmentsToVehicle(m_Trader_VehiclesPartsVehicleId[m],m_Trader_VehiclesParts[m]);
		}
		m_VehicleConfig.Save();
	}

	int GetQuantityBasedOnTraderQuantityHandler(int quantity)
	{
		switch(quantity)
		{
			case -3:return 0;
			break;

			case -4:return 1;
			break;

			case -6:return -1;
			break;

			case -2:return 1;
			break;

			case -5:return -2;
			break;

			case -7:return -1;
			break;

			case -8:return -1;
			break;

			default:return quantity;
			break;
		}
		return -1;
	}
	#endif

	override void InvokeOnConnect(PlayerBase player, PlayerIdentity identity)
	{
		/*I usually send config data from server to client here when the player join*/
		InitializeTraderPlusConfigToPlayer(player, identity);
		super.InvokeOnConnect(player,identity);
	}

	void InitializeTraderPlusConfigToPlayer(PlayerBase player, PlayerIdentity identity)
	{
		if(player)
				GetTraderPlusCore().TransfertTraderPlusConfigToClient(player);
	}

	override void InvokeOnDisconnect( PlayerBase player )
	{
		/*When I need to save data on server side in a database, that's where I do it*/
		super.InvokeOnDisconnect(player);
	}

	override void OnUpdate(float timeslice)
	{
		super.OnUpdate(timeslice);
		GetTraderPlusServer().OnUpdate(timeslice);
	}
};
