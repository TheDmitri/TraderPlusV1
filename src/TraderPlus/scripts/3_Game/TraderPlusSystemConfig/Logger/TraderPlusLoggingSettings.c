class TraderPlusLoggingSettings
{
	int logLevel = 1;
	int refreshRateInSeconds = 60;

	void Save()
	{
		JsonFileLoader<TraderPlusLoggingSettings>.JsonSaveFile(TRADERPLUS_LOGGER_CONFIG_FILE_SERVER, this);
	}

	void MakeDirectoryIfNotExists()
	{
		//we check if config folders exist, if not we create them
		if ( !FileExist( TRADERPLUS_CONFIG_ROOT_SERVER ) )
			MakeDirectory( TRADERPLUS_CONFIG_ROOT_SERVER );


		if ( !FileExist( TRADERPLUS_LOGGER_DIR_SERVER ) )
			MakeDirectory( TRADERPLUS_LOGGER_DIR_SERVER );
	}

	static ref TraderPlusLoggingSettings Load()	
	{
		TraderPlusLoggingSettings settings = new TraderPlusLoggingSettings();

		settings.MakeDirectoryIfNotExists();

		//Now we check if config exist, if yes, we load it. if no, we call function defaultVaultHeistSettings that will initialize default value
		if (FileExist(TRADERPLUS_LOGGER_CONFIG_FILE_SERVER)) 
		{
			JsonFileLoader<TraderPlusLoggingSettings>.JsonLoadFile(TRADERPLUS_LOGGER_CONFIG_FILE_SERVER, settings);
			return settings;
		}
	
	    settings.Save();
		return settings;
	}
};

