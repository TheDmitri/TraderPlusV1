class TraderPlusInsuranceSettings
{
	string Version = "";
	ref array<int> AuthorizedIDInsurance;
	ref array<ref TraderPlusVehicleInsurance>Insurances;

	void TraderPlusInsuranceSettings()
	{
		AuthorizedIDInsurance = new array<int>;
		Insurances         = new array<ref TraderPlusVehicleInsurance>;
	}

	void DefaultTraderPlusInsuranceSettings()
	{
		AuthorizedIDInsurance.Insert(5);
		Insurances.Insert(new TraderPlusVehicleInsurance("OffroadHatchback",1.25,0.75));
		Insurances.Insert(new TraderPlusVehicleInsurance("Hatchback_02",1.25,0.75));
		Insurances.Insert(new TraderPlusVehicleInsurance("CivilianSedan",1.25,0.75));
		TraderPlusJsonLoader<TraderPlusInsuranceSettings>.SaveToFile(TRADERPLUS_INSURANCE_CONFIG, this);
	}

	void Save()
	{
		TraderPlusJsonLoader<TraderPlusInsuranceSettings>.SaveToFile(TRADERPLUS_INSURANCE_CONFIG, this);
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

	static ref TraderPlusInsuranceSettings Load()	{
		TraderPlusInsuranceSettings settings = new TraderPlusInsuranceSettings();

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
		if (FileExist(TRADERPLUS_INSURANCE_CONFIG)) {
			GetTraderPlusLogger().LogInfo("TraderPlus file exist ! loading...");
			//JsonFileLoader<TraderPlussSettings>.JsonLoadFile(TRADERPLUS_VEHICULE_CONFIG, settings);
			TraderPlusJsonLoader<TraderPlusInsuranceSettings>.LoadFromFile(TRADERPLUS_INSURANCE_CONFIG, settings);
			settings.CheckVersion();
		}
		else {
			GetTraderPlusLogger().LogInfo("TraderPlus file doesn't exist ! creating...");
			settings.DefaultTraderPlusInsuranceSettings();
		}

		return settings;
	}
}
