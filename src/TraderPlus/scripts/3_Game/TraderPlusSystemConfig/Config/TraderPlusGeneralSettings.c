class TraderPlusGeneralSettings
{
	string 																 Version = "";
	bool                                   ConvertTraderConfigToTraderPlus;
	bool                                   ConvertTraderConfigToTraderPlusWithStockBasedOnCE;
	bool                                   UseGarageToTradeCar;
	bool                                   DisableHeightFailSafeForReceiptDeployment;
	bool                                   HideInsuranceBtn;
	bool                                   HideGarageBtn;
	bool                                   HideLicenceBtn;
	bool                                   EnableShowAllPrices;
	bool                                   EnableShowAllCheckBox;
	bool                                   IsReceiptTraderOnly;
	bool                                   IsReceiptSaveLock;
	bool                                   IsReceiptSaveAttachment;
	bool                                   IsReceiptSaveCargo;
	float                                  LockPickChance;
	string                                 LicenceKeyWord;
	ref TStringArray                       Licences;
	ref TraderPlusStates				   AcceptedStates;
	bool                                   StoreOnlyToPristineState;
	ref array<ref TraderPlusCurrency>			 Currencies;
	ref array<ref TraderPlusData>					 Traders;
	ref array<ref TraderPlusObject>				 TraderObjects;

	void TraderPlusGeneralSettings()
	{
		Currencies 						= new array<ref TraderPlusCurrency>;
		Licences         			= new TStringArray;
		Traders 							= new array<ref TraderPlusData>;
		TraderObjects 				= new array<ref TraderPlusObject>;
	}

	void DefaultTraderPlusGeneralSettings()
	{
		UseGarageToTradeCar = false;
		DisableHeightFailSafeForReceiptDeployment = false;
		EnableShowAllPrices = true;
		EnableShowAllCheckBox = true;
		ConvertTraderConfigToTraderPlus = false;
		ConvertTraderConfigToTraderPlusWithStockBasedOnCE = false;
		HideInsuranceBtn = false;
		HideGarageBtn = false;
		HideLicenceBtn = false;
		IsReceiptTraderOnly = true;
		IsReceiptSaveAttachment = true;
		IsReceiptSaveCargo = true;
		LockPickChance = 0.3;
		LicenceKeyWord = "Licence";
    Licences.Insert("Car Licence");
		Licences.Insert("Admin Licence");
		AcceptedStates = new TraderPlusStates(true, true, true);
		StoreOnlyToPristineState = false;
		Currencies.Insert(new TraderPlusCurrency("TraderPlus_Money_Euro500,TraderPlus_Money_Ruble500",500));
		Currencies.Insert(new TraderPlusCurrency("TraderPlus_Money_Dollar100,TraderPlus_Money_Euro100,TraderPlus_Money_Ruble100",100));
		Currencies.Insert(new TraderPlusCurrency("TraderPlus_Money_Dollar50,TraderPlus_Money_Euro50,TraderPlus_Money_Ruble50",50));
		Currencies.Insert(new TraderPlusCurrency("TraderPlus_Money_Dollar20,TraderPlus_Money_Euro20",20));
		Currencies.Insert(new TraderPlusCurrency("TraderPlus_Money_Dollar10,TraderPlus_Money_Euro10,TraderPlus_Money_Ruble10",10));
		Currencies.Insert(new TraderPlusCurrency("TraderPlus_Money_Dollar5,TraderPlus_Money_Euro5,TraderPlus_Money_Ruble5,TraderPlus_Money_Ruble5_Coin",5));
		Currencies.Insert(new TraderPlusCurrency("TraderPlus_Money_Dollar2,TraderPlus_Money_Ruble2_Coin,TraderPlus_Money_Euro2",2));
		Currencies.Insert(new TraderPlusCurrency("TraderPlus_Money_Dollar1,TraderPlus_Coin,TraderPlus_Money_Ruble1_Coin,TraderPlus_Money_Euro1",1));
		Traders.Insert(new TraderPlusData(0,"TraderPlusSign","Marie","Food Trader",Vector(11622.5048828125,  58.740455627441409,  14678.955078125), Vector(54, 0,			0)));
		Traders.Insert(new TraderPlusData(1,"TraderPlusSign","Kevin","Weapon Trader", Vector(11620.630859375, 59.04042053222656,  14656.8369140625), Vector(145, 0,			0)));
		Traders.Insert(new TraderPlusData(2,"TraderPlusSign","Bob","Tool Trader", Vector(11623.11328125,     59.0405158996582,    14658.916015625), Vector(145, 0,			0)));
		Traders.Insert(new TraderPlusData(3,"TraderPlusSign","John","Clothes Trader", Vector(11596.5576171875, 58.740474700927737, 14674.5458984375), Vector(-120, 0,			0)));
		Traders.Insert(new TraderPlusData(4,"TraderPlusSign","Pablo","Car Trader", Vector(11617.357421875,   58.740440368652347,   14686.9541015625), Vector(54, 0,			0)));
		TraderObjects.Insert(new TraderPlusObject("bldr_Misc_RoadBarrier",Vector(11537.2001953125,  59.0, 14726.099609375), Vector(55.000003814697269, 0.0, 0.0)));
		TraderObjects.Insert(new TraderPlusObject("bldr_Wall_Barricade1_10",Vector(11590.5498046875, 58.224754333496097, 14760.6728515625), Vector(144,	0,	0)));
		TraderObjects.Insert(new TraderPlusObject("bldr_Wall_Barricade1_4",Vector(11615.189453125,   57.636619567871097, 14626.82421875), Vector(153,	0,	0)));
		TraderObjects.Insert(new TraderPlusObject("bldr_Misc_Barricade",Vector(11613.2607421875,    57.99546432495117,   14622.95703125), Vector(54,	0,	0)));
		TraderObjects.Insert(new TraderPlusObject("Land_Wreck_S1023_Blue",Vector(11596.6044921875,  58.728797912597659,  14630.3427734375), Vector(-135,0,		0)));
		TraderObjects.Insert(new TraderPlusObject("Land_Wreck_Lada_Red",Vector(11598.5400390625,    58.41791915893555,   14628.796875), Vector(171.999535,0,		0)));
		TraderObjects.Insert(new TraderPlusObject("bldr_Wall_Barricade1_4",Vector( 11593.5087890625, 57.81910705566406,  14632.2060546875), Vector(63,	0,	0)));
		TraderObjects.Insert(new TraderPlusObject("bldr_Wall_Barricade1_10",Vector(11609.3408203125, 58.077457427978519, 14766.326171875), Vector(54,0,0)));
		TraderObjects.Insert(new TraderPlusObject("bldr_Wall_Barricade1_4",Vector( 11593.5087890625, 57.81910705566406,  14632.2060546875), Vector(63,	0,	0)));
		TraderObjects.Insert(new TraderPlusObject("bldr_hbarrier_4m",Vector(11664.1875, 57.7883186340332, 14659.564453125), Vector(-36.00001525878906, 0.0, 0.0)));
		TraderObjects.Insert(new TraderPlusObject("bldr_hbarrier_4m",Vector(11654.0380859375, 58.30573272705078, 14652.6845703125), Vector(-36.00001525878906, 0.0, 0.0)));
    TraderObjects.Insert(new TraderPlusObject("bldr_hbarrier_4m",Vector(11656.80078125, 58.16636657714844, 14653.1533203125), Vector(53.9, 0.0, 0.0)));
		TraderObjects.Insert(new TraderPlusObject("bldr_hbarrier_4m",Vector(11662.8837890625, 57.84144973754883,14657.0732421875), Vector(53.9, 0.0, 0.0)));
		TraderObjects.Insert(new TraderPlusObject("Land_City_Stand_FastFood",Vector(11594.310546875, 59.78278350830078, 14673.0546875), Vector(-117.0, 0.0, 0.0)));
		TraderObjects.Insert(new TraderPlusObject("Land_Misc_TrailRoof_Small",Vector(11598.7998046875, 59.28260040283203, 14669.2001953125), Vector(62, 0.0, 0.0)));
		TraderObjects.Insert(new TraderPlusObject("Land_Misc_TrailRoof_Small",Vector(11600.9658203125, 59.28261184692383, 14665.07421875), Vector(62, 0.0, 0.0)));
		TraderObjects.Insert(new TraderPlusObject("Land_Misc_Well_Pump_Yellow",Vector(11610.8466796875,58.61098861694336, 14674.2265625), Vector(126, 0.0, 0.0)));
		TraderObjects.Insert(new TraderPlusObject("Land_Garage_Office",Vector(11627.701171875, 67.97467041015625, 14805.9326171875), Vector(0, 0.0, 0.0)));
		TraderObjects.Insert(new TraderPlusObject("Land_Garage_Small",Vector( 11615.798828125, 59.178348541259769,14687.2724609375), Vector(50, 0.0, 0.0)));
		JsonFileLoader<TraderPlusGeneralSettings>.JsonSaveFile(TRADERPLUS_CONFIG, this);
	}

	void Save()
	{
		TraderPlusJsonLoader<TraderPlusGeneralSettings>.SaveToFile(TRADERPLUS_CONFIG, this);
	}

	void AddTradersToConfig(int id, string classname, string name, vector pos, vector ori)
	{
		Traders.Insert(new  TraderPlusData(id,classname,classname,name,pos, ori));
	}

	void AddClotheToTrader(int id, string classname)
	{
		Traders.Get(id).AddClotheToTraderPlus(classname);
	}

	void AddObjectsToConfig(string classname, vector pos, vector ori)
	{
		TraderObjects.Insert(new  TraderPlusObject(classname,pos, ori));
	}

	void CheckVersion()
	{
		if(Version != TRADERPLUS_CURRENT_VERSION)
		{
			Version = TRADERPLUS_CURRENT_VERSION;
			AcceptedStates.CoefficientWorn = 0.75;
			AcceptedStates.CoefficientDamaged = 0.5;
			AcceptedStates.CoefficientBadlyDamaged = 0.25;
			Save();
		}
	}

	static ref TraderPlusGeneralSettings Load()	{
		TraderPlusGeneralSettings settings = new TraderPlusGeneralSettings();

		if ( !FileExist( TRADERPLUS_CONFIG_ROOT_SERVER ) )
		{
			MakeDirectory( TRADERPLUS_CONFIG_ROOT_SERVER );
			if ( !FileExist( TRADERPLUS_CONFIG_DIR_SERVER ) )
			{
				MakeDirectory( TRADERPLUS_CONFIG_DIR_SERVER );
			}
			if ( !FileExist( TRADERPLUS_LOGGER_DIR_SERVER ) )
			{
				MakeDirectory( TRADERPLUS_LOGGER_DIR_SERVER );
			}
			if ( !FileExist( TRADERPLUS_DB_DIR_SERVER ) )
			{
				MakeDirectory( TRADERPLUS_DB_DIR_SERVER );
			}
		}
		//Now we check if config exist, if yes, we load it. if no, we call function defaultTraderPlussettings that will initialize default value
		if (FileExist(TRADERPLUS_CONFIG)) {
			GetTraderPlusLogger().LogInfo("TraderPlus file exist ! loading...");
			TraderPlusJsonLoader<TraderPlusGeneralSettings>.LoadFromFile(TRADERPLUS_CONFIG, settings);
			settings.CheckVersion();
			//JsonFileLoader<TraderPlusGeneralSettings>.JsonSaveFile(TRADERPLUS_CONFIG, settings);
			//JsonFileLoader<TraderPlusGeneralSettings>.JsonLoadFile(TRADERPLUS_CONFIG, settings);
		}
		else {
			GetTraderPlusLogger().LogInfo("TraderPlus file doesn't exist ! creating...");
			settings.DefaultTraderPlusGeneralSettings();
		}

		return settings;
	}
}
