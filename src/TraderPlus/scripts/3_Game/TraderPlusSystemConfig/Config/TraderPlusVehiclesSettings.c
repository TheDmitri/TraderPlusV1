class TraderPlusVehiclesSettings
{
	string Version = "";
	ref array<ref TraderPlusVehicleParts>VehiclesParts;

	void TraderPlusVehiclesSettings()
	{
		VehiclesParts         = new array<ref TraderPlusVehicleParts>;
	}

	void DefaultTraderPlusVehiclesSettings()
	{
		VehiclesParts.Insert(new  TraderPlusVehicleParts("OffroadHatchback"));
		VehiclesParts.Get(0).AddAttachmentsToVehicle("SparkPlug");
		VehiclesParts.Get(0).AddAttachmentsToVehicle("CarBattery");
		VehiclesParts.Get(0).AddAttachmentsToVehicle("CarRadiator");
		VehiclesParts.Get(0).AddAttachmentsToVehicle("HeadlightH7");
		VehiclesParts.Get(0).AddAttachmentsToVehicle("HeadlightH7");
		VehiclesParts.Get(0).AddAttachmentsToVehicle("HatchbackDoors_CoDriver");
		VehiclesParts.Get(0).AddAttachmentsToVehicle("HatchbackDoors_Driver");
		VehiclesParts.Get(0).AddAttachmentsToVehicle("HatchbackHood");
		VehiclesParts.Get(0).AddAttachmentsToVehicle("HatchbackTrunk");
		VehiclesParts.Get(0).AddAttachmentsToVehicle("HatchbackWheel");
		VehiclesParts.Get(0).AddAttachmentsToVehicle("HatchbackWheel");
		VehiclesParts.Get(0).AddAttachmentsToVehicle("HatchbackWheel");
		VehiclesParts.Get(0).AddAttachmentsToVehicle("HatchbackWheel");
		VehiclesParts.Insert(new  TraderPlusVehicleParts("Hatchback_02"));
		VehiclesParts.Get(1).AddAttachmentsToVehicle("SparkPlug");
		VehiclesParts.Get(1).AddAttachmentsToVehicle("CarBattery");
		VehiclesParts.Get(1).AddAttachmentsToVehicle("CarRadiator");
		VehiclesParts.Get(1).AddAttachmentsToVehicle("HeadlightH7");
		VehiclesParts.Get(1).AddAttachmentsToVehicle("HeadlightH7");
		VehiclesParts.Get(1).AddAttachmentsToVehicle("Hatchback_02_Door_1_1");
		VehiclesParts.Get(1).AddAttachmentsToVehicle("Hatchback_02_Door_1_2");
		VehiclesParts.Get(1).AddAttachmentsToVehicle("Hatchback_02_Door_2_1");
		VehiclesParts.Get(1).AddAttachmentsToVehicle("Hatchback_02_Door_2_2");
		VehiclesParts.Get(1).AddAttachmentsToVehicle("Hatchback_02_Hood");
		VehiclesParts.Get(1).AddAttachmentsToVehicle("Hatchback_02_Trunk");
		VehiclesParts.Get(1).AddAttachmentsToVehicle("Hatchback_02_Wheel");
		VehiclesParts.Get(1).AddAttachmentsToVehicle("Hatchback_02_Wheel");
		VehiclesParts.Get(1).AddAttachmentsToVehicle("Hatchback_02_Wheel");
		VehiclesParts.Get(1).AddAttachmentsToVehicle("Hatchback_02_Wheel");
		VehiclesParts.Insert(new  TraderPlusVehicleParts("CivilianSedan"));
		VehiclesParts.Get(2).AddAttachmentsToVehicle("SparkPlug");
		VehiclesParts.Get(2).AddAttachmentsToVehicle("CarBattery");
		VehiclesParts.Get(2).AddAttachmentsToVehicle("CarRadiator");
		VehiclesParts.Get(2).AddAttachmentsToVehicle("HeadlightH7");
		VehiclesParts.Get(2).AddAttachmentsToVehicle("HeadlightH7");
		VehiclesParts.Get(2).AddAttachmentsToVehicle("CivSedanDoors_Driver");
		VehiclesParts.Get(2).AddAttachmentsToVehicle("CivSedanDoors_CoDriver");
		VehiclesParts.Get(2).AddAttachmentsToVehicle("CivSedanDoors_BackLeft");
		VehiclesParts.Get(2).AddAttachmentsToVehicle("CivSedanDoors_BackRight");
		VehiclesParts.Get(2).AddAttachmentsToVehicle("CivSedanHood");
		VehiclesParts.Get(2).AddAttachmentsToVehicle("CivSedanTrunk");
		VehiclesParts.Get(2).AddAttachmentsToVehicle("CivSedanWheel");
		VehiclesParts.Get(2).AddAttachmentsToVehicle("CivSedanWheel");
		VehiclesParts.Get(2).AddAttachmentsToVehicle("CivSedanWheel");
		VehiclesParts.Get(2).AddAttachmentsToVehicle("CivSedanWheel");
		TraderPlusJsonLoader<TraderPlusVehiclesSettings>.SaveToFile(TRADERPLUS_VEHICULE_CONFIG, this);
	}

	void AddVehicleToConfig(string name)
	{
		VehiclesParts.Insert(new  TraderPlusVehicleParts(name));
	}

	void AddAttachmentsToVehicle(int pos, string attchname)
	{
		VehiclesParts.Get(pos).AddAttachmentsToVehicle(attchname);
	}

	void CheckVersion()
	{
		if(Version != TRADERPLUS_CURRENT_VERSION)
		{
			Version = TRADERPLUS_CURRENT_VERSION;
			/*
				code here
			*/
			Save();
		}
	}

	void Save()
	{
		TraderPlusJsonLoader<TraderPlusVehiclesSettings>.SaveToFile(TRADERPLUS_VEHICULE_CONFIG, this);
	}

	static ref TraderPlusVehiclesSettings Load()	{
		TraderPlusVehiclesSettings settings = new TraderPlusVehiclesSettings();

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
		if (FileExist(TRADERPLUS_VEHICULE_CONFIG)) {
			GetTraderPlusLogger().LogInfo("TraderPlus file exist ! loading...");
			//JsonFileLoader<TraderPlusVehiclesSettings>.JsonLoadFile(TRADERPLUS_VEHICULE_CONFIG, settings);
			TraderPlusJsonLoader<TraderPlusVehiclesSettings>.LoadFromFile(TRADERPLUS_VEHICULE_CONFIG, settings);
			settings.CheckVersion();
		}
		else {
			GetTraderPlusLogger().LogInfo("TraderPlus file doesn't exist ! creating...");
			settings.DefaultTraderPlusVehiclesSettings();
		}

		return settings;
	}
}
