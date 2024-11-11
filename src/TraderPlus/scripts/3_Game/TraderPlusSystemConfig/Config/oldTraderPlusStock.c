class oldTraderPlusStock
{
	ref array<string>TraderPlusItems;

	void oldTraderPlusStock()
	{
		TraderPlusItems = new array<string>;
	}

	void Save(int id)
	{
		string Path = TRADERPLUS_STOCK_CONFIG + "_" + id.ToString() + ".json";
		JsonFileLoader<oldTraderPlusStock>.JsonSaveFile(Path, this);
	}

	static ref oldTraderPlusStock Load(int id)	{
		oldTraderPlusStock settings = new oldTraderPlusStock;

		string Path = TRADERPLUS_STOCK_CONFIG + "_" + id.ToString() + ".json";

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
		}else
		{
			if ( !FileExist( TRADERPLUS_DB_DIR_SERVER ) )
			{
				MakeDirectory( TRADERPLUS_DB_DIR_SERVER );
			}
			if ( !FileExist( TRADERPLUS_LOGGER_DIR_SERVER ) )
			{
				MakeDirectory( TRADERPLUS_LOGGER_DIR_SERVER );
			}
		}

		if (FileExist(Path)) {
			Print("file exist ! loading...");
			//JsonFileLoader<TraderPlusStock>.JsonLoadFile(Path, settings);
			TraderPlusJsonLoader<oldTraderPlusStock>.LoadFromFile(Path, settings);
		}
		else {
			delete settings;
			return NULL;
		}

		return settings;
	}
}
