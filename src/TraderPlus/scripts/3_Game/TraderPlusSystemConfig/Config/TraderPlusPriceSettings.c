class TraderPlusPriceSettings
{
	string Version = "";
	bool EnableAutoCalculation;
	bool EnableAutoDestockAtRestart;
	int EnableDefaultTraderStock;
	ref array<ref TraderPlusCategory>TraderCategories;

	void TraderPlusPriceSettings()
	{
		TraderCategories = new array<ref TraderPlusCategory>;
	}

	void AddCategoryToConfig(string name)
	{
		TraderCategories.Insert(new  TraderPlusCategory(name));
	}

	void AddCategoryToConfigClient( TraderPlusCategory category)
	{
		TraderCategories.Insert(category);
	}

	void AddProductToCategory(int pos, string data)
	{
		TraderCategories.Get(pos).AddProductToTraderPlusCategory(data);
	}

	void Save()
	{
		TraderPlusJsonLoader<TraderPlusPriceSettings>.SaveToFile(TRADERPLUS_PRICE_CONFIG, this);
	}

	void CheckForSpacesToRemove()
	{
		GetTraderPlusLogger().LogInfo("CheckForSpacesToRemove - Started");
		for(int i=0; i<TraderCategories.Count();i++)
		{
			for(int j=0; j<TraderCategories.Get(i).Products.Count();j++)
			{
				string line = TraderCategories.Get(i).Products.Get(j);
				line = TraderPlusTrimSpaces(line);
				TraderCategories.Get(i).Products.Set(j, line);
			}
		}
		GetTraderPlusLogger().LogInfo("CheckForSpacesToRemove - Ended");
	}

	void CheckVersion()
	{
		if(Version != TRADERPLUS_CURRENT_VERSION)
		{
			Version = TRADERPLUS_CURRENT_VERSION;
			Save();
		}
	}

	static ref TraderPlusPriceSettings Load()	{
		TraderPlusPriceSettings settings = new TraderPlusPriceSettings();

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
		}
		//Now we check if config exist, if yes, we load it. if no, we call function defaultTraderPlussettings that will initialize default value
		if (FileExist(TRADERPLUS_PRICE_CONFIG)) {
			GetTraderPlusLogger().LogInfo("TraderPlus file exist ! loading...");
			//JsonFileLoader<TraderPlusPriceSettingsOld>.JsonLoadFile(path, settings);
			TraderPlusJsonLoader<TraderPlusPriceSettings>.LoadFromFile(TRADERPLUS_PRICE_CONFIG, settings);
			settings.CheckVersion();
		}
		return settings;
	}
}
