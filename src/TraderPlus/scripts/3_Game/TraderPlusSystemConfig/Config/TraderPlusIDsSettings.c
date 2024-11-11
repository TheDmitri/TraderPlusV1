class TraderPlusIDsSettings
{
	string Version = "";
	ref array<ref TraderPlusIDs>IDs;

	void TraderPlusIDsSettings()
	{
		IDs   = new array<ref TraderPlusIDs>;
	}

	void AddID()
	{
		IDs.Insert(new TraderPlusIDs);
	}

	void AddCategoryToID(int pos, string categoryname)
	{
		IDs.Get(pos).AddCategoryToId(categoryname);
	}

	void Save()
	{
		TraderPlusJsonLoader<TraderPlusIDsSettings>.SaveToFile(TRADERPLUS_ID_CONFIG, this);
	}

	void CheckVersion()
	{
		if(Version != TRADERPLUS_CURRENT_VERSION)
		{
			Version = TRADERPLUS_CURRENT_VERSION;
			for(int i=0;i<IDs.Count();i++)
			{
				if(IDs[i].Id == -1)
					IDs[i].Id = i;
			}
			Save();
		}
	}

	static ref TraderPlusIDsSettings Load()	{
		TraderPlusIDsSettings settings = new TraderPlusIDsSettings();

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
		if (FileExist(TRADERPLUS_ID_CONFIG)) {
			GetTraderPlusLogger().LogInfo("TraderPlus file exist ! loading...");
			//JsonFileLoader<TraderPlusIDsSettings>.JsonLoadFile(TRADERPLUS_ID_CONFIG, settings);
			TraderPlusJsonLoader<TraderPlusIDsSettings>.LoadFromFile(TRADERPLUS_ID_CONFIG, settings);
			settings.CheckVersion();
		}
		return settings;
	}
}
