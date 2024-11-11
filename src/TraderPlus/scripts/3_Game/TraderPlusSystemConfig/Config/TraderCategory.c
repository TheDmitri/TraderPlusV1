class TraderPlusCategories
{
	string DummyString="DONT_ASK_WHY_IM_HERE";
	ref map<string, ref TStringArray> traderCategories;

	void TraderPlusCategories()
	{
		traderCategories = new map<string, ref TStringArray>;
	}

	void ConvertArrayCategoryToMap()
	{
		TraderPlusPriceSettings priceConfig = TraderPlusPriceSettings.Load();
		foreach(TraderPlusCategory tpCategory: priceConfig.TraderCategories)
		{
			traderCategories.Insert(tpCategory.CategoryName, tpCategory.Products);
		}
	}
}

class TraderPlusVariantAndCompatibles
{
	string ProductName;
	ref TStringArray productVariants;
	ref TStringArray compatiblesProducts;

	void TraderPlusVariantAndCompatibles(string productName)
	{
		ProductName = productName;
		productVariants = new TStringArray;
		compatiblesProducts = compatiblesProducts;
	}
}

class TraderPlusCategory
{
	string CategoryName;
	ref array<string> Products;

  void TraderPlusCategory(string categoryname = "")
  {
    CategoryName = categoryname;
    Products 		 = new array<string>;
  }

  void AddProductToTraderPlusCategory(string product)
  {
    Products.Insert(product);
  }

	void Save(int id)
	{
		string idPath = TRADERPLUS_DB_DIR_SERVER +"ID_"+id.ToString()+"\\";
		string categoryPath = idPath + CategoryName + ".json";
		JsonFileLoader<TraderPlusCategory>.JsonSaveFile(categoryPath, this);
		//TraderPlusJsonLoader<TraderPlusCategory>.SaveToFile(categoryPath, this);
	}
};

class TraderPlusCategoryItems
{
	string CategoryName;
	ref array<ref TraderPlusArticle> Products;

  void TraderPlusCategoryItems(string categoryname = "")
  {
    CategoryName = categoryname;
    Products 		 = new array<ref TraderPlusArticle>;
  }

	void Copy(TraderPlusCategoryItems toCopy)
	{
		CategoryName = toCopy.CategoryName;
		foreach(TraderPlusArticle a: toCopy.Products)
		{
			Products.Insert(a);
		}
	}

  void AddProductToTraderPlusCategory(string categoryname, string classname, int quantity, int health, int maxstock)
  {
		TraderPlusArticle article = new TraderPlusArticle;
		article.AddStockItems(categoryname,classname,quantity,health,maxstock);
    Products.Insert(article);
  }
};

class TraderPlusIDs
{
	int               Id=-1;
	ref array<string> Categories;
	ref TStringArray  LicencesRequired;
	ref TStringArray  CurrenciesAccepted;

  void TraderPlusIDs()
  {
    Categories = new array<string>;
		LicencesRequired = new TStringArray;
		CurrenciesAccepted = new TStringArray;
  }

  void AddCategoryToId(string categoryname)
  {
    Categories.Insert(categoryname);
  }
};
