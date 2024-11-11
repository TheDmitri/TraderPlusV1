class TraderPlusClient
{
	ref map<string, ref map<string, ref StockItem>> m_TraderStock;
	ref TraderPlusCategories m_TraderPlusCategories;
	ref MapTraderPlusItems m_TraderPlusItems;
	ref MapTraderPlusItems m_AllTraderPlusItems;
	ref TStringArray m_StockCategories;

	ref TBoolArray m_InsuranceStatus;

	string m_lastTradeCategory = "";

	bool m_StockRequest;

	void TraderPlusClient()
	{
		m_TraderStock      = new map<string, ref map<string, ref StockItem>>;
		m_TraderPlusCategories = new TraderPlusCategories;
		m_TraderPlusItems  = new MapTraderPlusItems;
		m_AllTraderPlusItems = new MapTraderPlusItems;
		m_StockCategories  = new TStringArray;
		m_InsuranceStatus = new TBoolArray;
    }

	int GetItemInStockFromCategory(int index, out array<ref TraderPlusCategoryItems>stockItems, bool showall = false, string filter = "")
	{
		filter.ToLower();

		TStringArray categories = new TStringArray();

		if(index == 0)
		{
			GetTraderPlusItemsListFromAllCategorySA();
			categories = m_StockCategories;
		}
		else
		{
			GetTraderPlusItemsListFromCategory(m_StockCategories[index]);
			categories.Insert(m_StockCategories[index]);
		}

		stockItems.Clear();

		MapTraderPlusItems ttraderPlusItems = new MapTraderPlusItems;
		foreach(string tcategory, array<ref TraderPlusItem> products: m_TraderPlusItems.traderPlusItems)
		{
			string cat = tcategory;
			array<ref TraderPlusItem> tproducts = new array<ref TraderPlusItem>;
			TraderPlusCopyMap<array<ref TraderPlusItem>>.CopyMap(products, tproducts);
			ttraderPlusItems.traderPlusItems.Insert(cat, tproducts);
		}

		foreach(string category : categories)
		{
			
			GetTraderPlusLogger().LogDebug("foreach: "+category);
			
			array<ref TraderPlusItem> ttproducts
			if(!ttraderPlusItems.traderPlusItems.Find(category, ttproducts))continue;
			TraderPlusCategoryItems categoryItems = new TraderPlusCategoryItems(category);
			foreach(TraderPlusItem traderPlusItem: ttproducts)
			{
				bool skip = false;
				
				GetTraderPlusLogger().LogDebug("GetItemInStockFromCategory: "+traderPlusItem.ClassName);
				
				TraderPlusArticle product = new TraderPlusArticle;
				string lowClassName = traderPlusItem.ClassName;
				lowClassName.ToLower();
				if(m_TraderStock[category] && m_TraderStock[category][lowClassName] && TraderPlusHelper.CanAddProductToList(filter, traderPlusItem.ClassName))
				{
						for(int h=0;h<4;h++)
						{
							if(m_TraderStock[category][lowClassName].Health.Contains(h))
							{
								if(!m_TraderStock[category][lowClassName].Health[h])continue;
								categoryItems.AddProductToTraderPlusCategory(category,traderPlusItem.ClassName,m_TraderStock[category][lowClassName].Health[h],h,traderPlusItem.MaxStock);
								skip = true;
							}
						}
				}
				if(!skip && traderPlusItem.MaxStock == TRADEMODE_UNLIMITED && TraderPlusHelper.CanAddProductToList(filter, traderPlusItem.ClassName))
				{
					categoryItems.AddProductToTraderPlusCategory(category,traderPlusItem.ClassName,traderPlusItem.MaxStock,0,traderPlusItem.MaxStock);
					
			    GetTraderPlusLogger().LogDebug(traderPlusItem.ClassName + " added to stockItems");
			    
					skip = true;
				}
				if(!skip && showall  && TraderPlusHelper.CanAddProductToList(filter, traderPlusItem.ClassName))
					categoryItems.AddProductToTraderPlusCategory(category,traderPlusItem.ClassName,0,0,traderPlusItem.MaxStock);

			}
			stockItems.Insert(categoryItems);
		}
		return stockItems.Count();
	}

	float GetCoefficientState(int state)
	{
		switch(state)
		{
			case TraderPlusItemState.PRISTINE: return 1.0;
			break;

			case TraderPlusItemState.WORN: return GetTraderPlusConfigClient().CoefficientWorn;
			break;

			case TraderPlusItemState.DAMAGED:return GetTraderPlusConfigClient().CoefficientDamaged;
			break;

			case TraderPlusItemState.BADLY_DAMAGED:return GetTraderPlusConfigClient().CoefficientBadlyDamaged;
			break;

			default: return 0;
			break;
		}

		return 0;
	}

	//function that handle the price calculation, don't fuck with it. it's very important !!!
  int CalculatePriceForThatItem(int qtyMultiplier, int trademode, string category, string classname, int stockqty, int state, out int maxstock, out int tradeqty)
  {
    
    GetTraderPlusLogger().LogDebug("CalculatePriceForThatItem: name"+classname+"state:"+state.ToString());
    

    //We make sure that the item is accept under its state condttion
    if((!GetTraderPlusConfigClient().AcceptWorn && state == TraderPlusItemState.WORN) || (!GetTraderPlusConfigClient().AcceptDamaged && state == TraderPlusItemState.DAMAGED) || (!GetTraderPlusConfigClient().AcceptBadlyDamaged && state == TraderPlusItemState.BADLY_DAMAGED) || state == TraderPlusItemState.RUINED)
    {
      
      GetTraderPlusLogger().LogDebug("CalculatePriceForThatItem: state is not accepted");
      
      return -1,
    }

		if(!m_TraderPlusItems.traderPlusItems || !m_TraderPlusItems.traderPlusItems[category] || m_TraderPlusItems.traderPlusItems[category].Count() < 1)return -1;
    foreach(TraderPlusItem tpItem: m_TraderPlusItems.traderPlusItems[category])
    {
      if(CF_String.EqualsIgnoreCase(tpItem.ClassName,classname))
      {
        //if(!GetTraderPlusClient().IsCorrectCategory(tpItem.CategoryName))continue;
        if(trademode == TRADEMODE_SELL)
        {
          tradeqty = tpItem.Quantity*qtyMultiplier;
          maxstock = tpItem.MaxStock;
          float coefficientSell = 1;
          if(tpItem.Coeff > 0)coefficientSell=tpItem.Coeff;
          int baseSellprice = tpItem.SellPrice;
          
          GetTraderPlusLogger().LogDebug("CalculatePriceForThatItem: baseSellprice before formula:"+baseSellprice.ToString());
          GetTraderPlusLogger().LogDebug("CalculatePriceForThatItem: qtyMultiplier:"+qtyMultiplier.ToString());
          GetTraderPlusLogger().LogDebug("CalculatePriceForThatItem: tradeqty:"+tradeqty.ToString());
          
          if(baseSellprice == -1)return baseSellprice;
          if(maxstock == -1)return 1.0*baseSellprice*qtyMultiplier*GetCoefficientState(state);
          float sellprice = 0;
          for(int ps=1;ps<qtyMultiplier+1;ps++)
          {
            int n = stockqty;
            
            GetTraderPlusLogger().LogDebug("CalculatePriceForThatItem: n:"+n.ToString());
            
            if(n==0)n=1;
              sellprice += Math.Pow(coefficientSell,(n - 1))*baseSellprice*GetCoefficientState(state);
            
            GetTraderPlusLogger().LogDebug("CalculatePriceForThatItem: sellprice:"+sellprice.ToString());
            
            stockqty++;
          }
          baseSellprice = sellprice;
          if(baseSellprice==0)baseSellprice=1;
          return baseSellprice;
        }
        else //buy - TRADEMODE_BUY
        {
          
          GetTraderPlusLogger().LogDebug("CalculatePriceForThatItem: looking for buyprice:");
          
          tradeqty = tpItem.Quantity*qtyMultiplier;
          maxstock = tpItem.MaxStock;
          float coefficientBuy = 1;
          if(tpItem.Coeff>0)coefficientBuy=tpItem.Coeff;
          
          GetTraderPlusLogger().LogDebug("CalculatePriceForThatItem: BuyQty:"+tpItem.Quantity.ToString());
          GetTraderPlusLogger().LogDebug("CalculatePriceForThatItem: tradeqty:"+tradeqty.ToString());
          
          int baseBuyprice = tpItem.BuyPrice;
          if(baseBuyprice == -1)return baseBuyprice;
          if(maxstock==-1)return 1.0*baseBuyprice*qtyMultiplier*GetCoefficientState(state);;
          
          GetTraderPlusLogger().LogDebug("CalculatePriceForThatItem: baseBuyprice before formula:"+baseBuyprice.ToString());
          
          float buyprice = 0;
          for(int pb=1;pb<qtyMultiplier+1;pb++)
          {
            int m = stockqty;
            
            GetTraderPlusLogger().LogDebug("CalculatePriceForThatItem: m:"+m.ToString());
            
            buyprice += Math.Pow(coefficientBuy,(m - 1))*baseBuyprice*GetCoefficientState(state);;
            
            GetTraderPlusLogger().LogDebug("CalculatePriceForThatItem: buyprice:"+buyprice.ToString());
            
            stockqty++;
          }
          baseBuyprice = buyprice;
          if(baseBuyprice==0)baseBuyprice=1;
          
          GetTraderPlusLogger().LogDebug("CalculatePriceForThatItem: baseBuyprice after formula:"+baseBuyprice.ToString());
          
          return baseBuyprice;
        }
      }
    }
    return 0;
  }

	int GetPlayerItemsFromCategory(int pos, out  out array<ref TraderPlusArticle>m_PlayerItems, string filter = "")
	{
		filter.ToLower();
		PlayerBase player = PlayerBase.Cast(GetGame().GetPlayer());
		if(!player)return 0;

		if(pos == TraderPlusInventoryCategory.LICENCES)
		{
			m_PlayerItems.Clear();

			for(int li=0;li<GetBankAccount().Licences.Count();li++)
      		{
				TraderPlusArticle lproduct = new TraderPlusArticle;
				lproduct.AddPlayerItems("licence_section",GetBankAccount().Licences[li],1,0,0,false);
				m_PlayerItems.Insert(lproduct);
      		}
			return m_PlayerItems.Count();
		}

		if(pos == TraderPlusInventoryCategory.VEHICLES)
		{
			m_PlayerItems.Clear();

			for(int lv=0;lv<GetGarageCore().m_VehiclesName.Count();lv++)
			{
				TraderPlusArticle vproduct = new TraderPlusArticle;
				vproduct.AddPlayerItems("vehicle_section",GetGarageCore().m_VehiclesName[lv],1,1,GetGarageCore().m_VehiclesHealth[lv],true);
				m_PlayerItems.Insert(vproduct);
			}
			return m_PlayerItems.Count();
		}

		bool state = TraderPlusHelper.GetPlayersItems(player,m_PlayerItems,filter);
		if(state)
		{
			if(pos == TraderPlusInventoryCategory.ALL)
				return m_PlayerItems.Count();

			MapTraderPlusItems ttraderPlusItems = new MapTraderPlusItems;
			GetTraderPlusItemsListFromAllCategory(m_StockCategories,ttraderPlusItems);

			array<ref TraderPlusArticle> tPlayerItems = new array<ref TraderPlusArticle>;
			for(int j = 0 ; j<m_PlayerItems.Count();j++)
			{
				string playerItem = m_PlayerItems[j].ClassName;
				foreach(string category, array<ref TraderPlusItem> tpItems: ttraderPlusItems.traderPlusItems)
				{
					if(!tpItems || tpItems.Count() == 0)continue;
					foreach(TraderPlusItem traderPlusItem: tpItems)
					{
						if(CF_String.EqualsIgnoreCase(playerItem,traderPlusItem.ClassName) && traderPlusItem.SellPrice != TRADEMODE_NO_TRADE)
						{
							TraderPlusArticle pproduct = new TraderPlusArticle;
							pproduct.AddPlayerItems(category,m_PlayerItems[j].ClassName,m_PlayerItems[j].Count,m_PlayerItems[j].Quantity,m_PlayerItems[j].Health,m_PlayerItems[j].HasAttachments);
							tPlayerItems.Insert(pproduct);
						}
					}
				}
			}

			m_PlayerItems.Clear();
			m_PlayerItems = tPlayerItems;

			CombinedSpecificItems(m_PlayerItems);
			return m_PlayerItems.Count();
		}
		return 0;
	}

	void CombinedSpecificItems(out array<ref TraderPlusArticle> m_PlayerItems)
	{
		array<ref TraderPlusArticle> tPlayerItems = new array<ref TraderPlusArticle>;

		while(m_PlayerItems.Count()>0)
		{
			string category = m_PlayerItems[0].Category;
			string name = m_PlayerItems[0].ClassName;
			int count = m_PlayerItems[0].Count;
			int qty = m_PlayerItems[0].Quantity;
			int health = m_PlayerItems[0].Health;
			bool hasattch = m_PlayerItems[0].HasAttachments;

			TIntArray SpecificItemsPositions = new TIntArray;

			m_PlayerItems.RemoveOrdered(0);

			int countSpecificItem = 0;

			for(int i=0;i<m_PlayerItems.Count();i++)
			{
				if(CF_String.EqualsIgnoreCase(m_PlayerItems[i].ClassName,name) && m_PlayerItems[i].Health==health)
					countSpecificItem++;

				/*if(m_PlayerItems[i].ClassName==name)
					countSpecificItem++;*/
			}

			if(m_PlayerItems.Count() != 0)
			{
				int j=0;
				while(countSpecificItem!=0)
				{
					if(CF_String.EqualsIgnoreCase(m_PlayerItems[j].ClassName,name) && m_PlayerItems[j].Health==health)
					{
						count += m_PlayerItems[j].Count;
						qty += m_PlayerItems[j].Quantity;
						m_PlayerItems.RemoveOrdered(j);
						countSpecificItem--;
						j=0;
						continue;
					}
					j++;
				}
			}

			TraderPlusArticle pproduct = new TraderPlusArticle;
			pproduct.AddPlayerItems(category,name,count,qty,health,hasattch);
			tPlayerItems.Insert(pproduct);
		}

		m_PlayerItems.Clear();
		m_PlayerItems = tPlayerItems;
	}

	int GetSpecificItemCountOnPlayer(string classname, int health ,array<ref TraderPlusArticle> m_PlayerItems)
	{
		foreach(TraderPlusArticle article: m_PlayerItems)
		{
			if(CF_String.EqualsIgnoreCase(article.ClassName,classname) && article.Health == health)
			return article.Count;
		}

		return 0;
	}

	//To try to be efficient on the server, we try to make its life easy by giving to the server the position of the product, to help find it on server stock
	int GetStockProductFromStock(string category, string name, int health)
	{
		name.ToLower();
		if(GetTraderPlusConfigClient().StoreOnlyToPristineState)health = 0;
		if(m_TraderStock[category] && m_TraderStock[category][name])
				return m_TraderStock[category][name].Health[health];

		return 0;
	}

	string GetProductCategory(string classname)
	{
		foreach(string category, array<ref TraderPlusItem> tpItems: m_TraderPlusItems.traderPlusItems)
		{
			if(!tpItems || tpItems.Count() == 0)continue;
			foreach(TraderPlusItem traderPlusItem: tpItems)
			{
				if(CF_String.EqualsIgnoreCase(traderPlusItem.ClassName,classname))
						return category;
			}
		}

		return "";
	}

	void ResetPriceConfig()
	{
		m_TraderPlusCategories.traderCategories.Clear();
	}

	int GetCategoriesFromID(int id)
	{
		m_StockCategories.Clear();
		string str = GetTraderPlusConfigClient().IDsCategories[id];
		TStringArray strs = new TStringArray;
		str.Split(",",strs);
		int count = strs.Count();
		for(int i=0;i<count;i++)
			m_StockCategories.Insert(strs[i]);

		return count;
	}

	bool IsCorrectCategory(string categoryname)
	{
		return m_StockCategories.Find(categoryname) != -1;
	}

	void GetTraderPlusItemsListFromCategory(string categoryname)
	{
		
		GetTraderPlusLogger().LogDebug("GetTraderPlusItemsListFromCategory");
		

		//In some cases we don't want to clear the items, that's why we need that reset condition
		m_TraderPlusItems.traderPlusItems.Clear();

		if(GetBankAccount() && GetBankAccount().Licences)
		{
			if(categoryname.Contains(GetTraderPlusConfigClient().LicenceKeyWord) && GetBankAccount().Licences.Find(categoryname) == -1)
					return;
		}

		array<ref TraderPlusItem> tpItems = new array<ref TraderPlusItem>;
		m_TraderPlusItems.traderPlusItems.Set(categoryname, new array<ref TraderPlusItem>);
		if(!m_AllTraderPlusItems.traderPlusItems.Find(categoryname,tpItems))return;
		foreach(TraderPlusItem tpItem: tpItems)
		{
			m_TraderPlusItems.traderPlusItems[categoryname].Insert(tpItem);
		}
	}

	void GetTraderPlusItemsListFromAllCategorySA()
	{
		m_TraderPlusItems.traderPlusItems.Clear();
		GetTraderPlusItemsListFromAllCategory(m_StockCategories,m_TraderPlusItems);
	}

	void GetTraderPlusItemsListFromAllCategory(array<string> categories, out MapTraderPlusItems tpItemsFromAllCategory)
	{
		
		GetTraderPlusLogger().LogDebug("GetTraderPlusItemsListFromAllCategory");
		

		//In some cases we don't want to clear the items, that's why we need that reset condition
		//m_TraderPlusItems.traderPlusItems.Clear();

		for(int k=0; k<categories.Count();k++)
		{
			if(GetBankAccount() && GetBankAccount().Licences)
			{
				if(categories[k].Contains(GetTraderPlusConfigClient().LicenceKeyWord) && GetBankAccount().Licences.Find(categories[k]) == -1)
					continue;
			}

			array<ref TraderPlusItem> tpItems = new array<ref TraderPlusItem>;
			if(!m_AllTraderPlusItems.traderPlusItems.Find(categories[k],tpItems))continue;
			tpItemsFromAllCategory.traderPlusItems.Set(categories[k], new array<ref TraderPlusItem>);
			foreach(TraderPlusItem tpItem: tpItems)
			{
				tpItemsFromAllCategory.traderPlusItems[categories[k]].Insert(tpItem);
			}
		}
  }

	float CalculateTradeQty(string classname,float tradeqty, int maxqty)
	{
		//if the user fucked up the tradqty, we define it as maxqty of the item
		if(tradeqty > maxqty)return maxqty;

		//in case it's a mag and user did not define 0 or Full
		if(classname.Contains("Mag_") && tradeqty != 0.0 )return 0;

		//if our tradeqty is a % or maxqty
		if(tradeqty < 1.0 && tradeqty != 0.0 && tradeqty != -1.0 ) return tradeqty*(1.0*maxqty);

		//if we want to define maxqty of our product
		if(tradeqty == -1.0)return maxqty;

		//finally we return tradeqty if everything is ok or should be...
		return tradeqty;
	}

	void ConvertPriceConfigToTraderItems()
	{
		
		GetTraderPlusLogger().LogDebug("ConvertPriceConfigToTraderItems");
		

		m_AllTraderPlusItems.traderPlusItems.Clear();
		TraderPlusCategories cpyCategories = new TraderPlusCategories;
		TraderPlusCopyMap<TraderPlusCategories>.CopyMap(m_TraderPlusCategories,cpyCategories);
		foreach(string category, TStringArray products: cpyCategories.traderCategories)
		{
			if(!products || products.Count()==0)continue;
			array<ref TraderPlusItem>tpItems = new array<ref TraderPlusItem>;
			for(int j=0;j<products.Count();j++)
			{
					string str = products[j];
					TStringArray strs = new TStringArray;
					str.Split( ",", strs );
					float sellprice = strs[5].ToFloat();
					if( sellprice < 1.0 && sellprice != -1.0)
								sellprice = strs[4].ToFloat() * strs[5].ToFloat();
					float tradeqty = strs[3].ToFloat();
					int   maxqty   = TraderPlusHelper.GetMaxItemQuantityClient(strs[0]);
					tradeqty = CalculateTradeQty(strs[0],tradeqty,maxqty);
					tpItems.Insert(new TraderPlusItem(strs[0],strs[1].ToFloat(),strs[2].ToInt(),tradeqty,sellprice,strs[4].ToInt()));
			}
			m_AllTraderPlusItems.traderPlusItems.Insert(category, tpItems);
		}
	}

	void ConvertServerStockToClientStock(TraderPlusStock serverStock)
	{
		foreach(string category, array<string>products: serverStock.TraderPlusItems)
		{
			if(!products || products.Count() < 1)continue;
			m_TraderStock.Set(category, new map<string, ref StockItem>);
			foreach(string product: products)
			{
				TStringArray strs = new TStringArray;
				product.Split(" ",strs);
				string classname = strs[0];
				classname.ToLower();
				if(!m_TraderStock[category][classname])
						m_TraderStock[category].Set(classname, new StockItem);
				m_TraderStock[category][classname].Health.Set(strs[2].ToInt(),strs[1].ToInt());
				
				GetTraderPlusLogger().LogDebug("add to stock:"+ classname + " " + strs[2] + " " + strs[1]);
				
			}
		}
  }

	//-------------------------------------------RPC PART--------------------------------------------------------//
	void GetStockResponseBasedOnID(CallType type, ParamsReadContext ctx, PlayerIdentity sender, Object target)
  {
   	if(!GetGame().IsClient())
     return;

		 
		 GetTraderPlusLogger().LogDebug("GetStockResponseBaseOnIDCategory");
		 

     Param3<int, ref TraderPlusStock, string> data;
     if (!ctx.Read(data))
       return;

     //We get our stock based on the TraderID from the Trader Menu request
		 ConvertServerStockToClientStock(data.param2);

		 if(data.param3!= "")
		 		m_lastTradeCategory = data.param3;

		 
		 int count = m_TraderStock.Count();
		 GetTraderPlusLogger().LogDebug("Stock count: "+count.ToString());
		 

		 PlayerBase player = PlayerBase.Cast(GetGame().GetPlayer());
		 if(player && player.m_TraderPlusMenu)
			 		player.m_TraderPlusMenu.UIUpdate();

		 //We check what kind of stock response is it: Is it a sell/buy update ? or a NO_TRADE response
		 int response = data.param1;
		 if(response == TraderPlusResponse.NO_TRADE)return;

		 if(player && player.m_TraderPlusMenu)
			 		player.m_TraderPlusMenu.GetTradingResponse(response);
  }



	void GetPriceRequestFromCategory(CallType type, ParamsReadContext ctx, PlayerIdentity sender, Object target)
	{
		if (!GetGame().IsClient())
      		return;

		
		GetTraderPlusLogger().LogDebug("GetPriceRequestFromCategory");
		

    	Param1<ref TraderPlusCategories> data;
    	if (!ctx.Read(data))
        	return;

		TraderPlusCopyMap<TraderPlusCategories>.CopyMap(data.param1, m_TraderPlusCategories);
		ConvertPriceConfigToTraderItems();
	}

	void GetInsuranceResponse(CallType type, ParamsReadContext ctx, PlayerIdentity sender, Object target)
	{
		if (!GetGame().IsClient())
      		return;

		
		GetTraderPlusLogger().LogDebug("GetInsuranceResponse");
		

    	Param1<TBoolArray> data;
    	if (!ctx.Read(data))
        	return;

		m_InsuranceStatus.Clear();
		m_InsuranceStatus= data.param1;
	}
}
