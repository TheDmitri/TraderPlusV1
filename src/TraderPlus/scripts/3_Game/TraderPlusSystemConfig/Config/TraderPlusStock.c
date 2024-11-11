class StockItem
{
	ref map<int, int>Health;

	void StockItem()
	{
		Health = new map<int, int>;
	}
};

class TraderPlusStock
{
	string DummyString="DONT_ASK_WHY_IM_HERE";
	ref map<string, ref TStringArray>TraderPlusItems;

	void TraderPlusStock()
	{
		TraderPlusItems = new map<string, ref TStringArray>();
	}

	void DefaultTraderPlusStock(int id,array<string> categories)
	{
		string categoryPath = TRADERPLUS_DB_DIR_SERVER +"ID_"+id.ToString()+"\\";
		foreach(string category: categories)
		{
			string path = categoryPath + category + ".json";
			TraderPlusCategory tpStockCategory = new TraderPlusCategory(category);
			GetTraderPlusLogger().LogInfo("save category to path:"+path);
			TraderPlusJsonLoader<TraderPlusCategory>.SaveToFile(path, tpStockCategory);
		}
	}

	void LoadAllCategories(int id,array<string> categories)
	{
		string categoryPath = TRADERPLUS_DB_DIR_SERVER +"ID_"+id.ToString()+"\\";
		foreach(string category: categories)
		{
			string path = categoryPath + category + ".json";
			TraderPlusCategory tpStockCategory = new TraderPlusCategory;
			JsonFileLoader<TraderPlusCategory>.JsonLoadFile(path, tpStockCategory);
			//TraderPlusJsonLoader<TraderPlusCategory>.LoadFromFile(path, tpStockCategory);
			TraderPlusItems.Insert(category, tpStockCategory.Products);
		}
	}

	static TraderPlusStock GetStockFromID(int id, TStringArray categories)
	{
		TraderPlusStock tpStock = new TraderPlusStock;
		string categoryPath = TRADERPLUS_DB_DIR_SERVER +"ID_"+id.ToString()+"\\";
		foreach(string category: categories)
		{
			string path = categoryPath + category + ".json";
			TraderPlusCategory tpStockCategory = new TraderPlusCategory;
			JsonFileLoader<TraderPlusCategory>.JsonLoadFile(path, tpStockCategory);
			//TraderPlusJsonLoader<TraderPlusCategory>.LoadFromFile(path, tpStockCategory);
			tpStock.TraderPlusItems.Insert(category, tpStockCategory.Products);
		}
		return tpStock;
	}

	void Save(int id)
	{
		string Path = TRADERPLUS_STOCK_CONFIG + "_" + id.ToString() + ".json";
		TraderPlusJsonLoader<TraderPlusStock>.SaveToFile(Path, this);
	}

	static ref TraderPlusStock Load(int id, array<string>categories = NULL)	{
		TraderPlusStock settings = new TraderPlusStock;

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
		else
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

		string categoryPath = TRADERPLUS_DB_DIR_SERVER +"ID_"+id.ToString()+"\\";
		if (FileExist(categoryPath))
		{
				settings.LoadAllCategories(id, categories);
		}
		else {
			Print("file doesn't exist ! creating...");
			MakeDirectory(categoryPath);
			settings.DefaultTraderPlusStock(id, categories);
		}

		return settings;
	}
}
