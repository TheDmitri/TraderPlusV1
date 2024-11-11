class TraderPlusCategoryMenu extends UIScriptedMenu
{
  Widget CategoryCard;
  TextWidget CategoryNameText;
  TextWidget ProductCount;
  GridSpacerWidget ProductList;

  ref TraderPlusCategoryItems CategoryItems;

  private ref array<EntityAI> localCategoryItems;
  private ref map<Widget, int> m_WidgetStockList;

  ref TraderPlusMenu ParentMenu;

  void TraderPlusCategoryMenu(Widget layoutRoot, TraderPlusCategoryItems categoryItems, TraderPlusMenu parentMenu)
   {
     MenuInit(layoutRoot, categoryItems, parentMenu);
   }

   string GetCategoryCardLayout()
   {
     return "TraderPlus/gui/CategoryCard.layout";
   }

   void MenuInit(Widget layoutRoot, TraderPlusCategoryItems categoryItems, TraderPlusMenu parentMenu)
   {
     CategoryItems = new TraderPlusCategoryItems;
     CategoryItems.Copy(categoryItems);

     localCategoryItems = new array<EntityAI>;
     m_WidgetStockList = new map<Widget, int>;

     ParentMenu = parentMenu;

     CategoryCard = GetGame().GetWorkspace().CreateWidgets( GetCategoryCardLayout(), layoutRoot);
     ProductList = GridSpacerWidget.Cast(CategoryCard.FindAnyWidget("ProductsList"));
     CategoryNameText = TextWidget.Cast(CategoryCard.FindAnyWidget("Title"));
     ProductCount = TextWidget.Cast(CategoryCard.FindAnyWidget("ProductsCount"));

     ProductCount.SetText("");
     CategoryNameText.SetText(CategoryItems.CategoryName);
   }

  void ~TraderPlusCategoryMenu()
  {
    if(CategoryCard)
    {
      ClearAllProductsInCardList(CategoryCard);
      DestroyAllChildren(CategoryCard);
    }

    DestroylocalCategoryItems();

    GetGame().GetCallQueue(CALL_CATEGORY_GUI).Remove(FillCategoryProductList);
  }

  void ClearAllProductsInCardList(Widget w)
  {
    DestroyAllChildren(w);
    DestroylocalCategoryItems();
  }

  void DestroyAllChildren(Widget w)
  {
    while(w.GetChildren())
    {
      w.RemoveChild(w.GetChildren());
    }
  }

  void DestroylocalCategoryItems()
	{
    if (!localCategoryItems || !localCategoryItems.Count()) return;
		foreach (auto ent : localCategoryItems)
		{
			if (ent)
				ent.Delete();
		}
    delete localCategoryItems;
	}

  TextWidget GetCardTitleWidget(int length, Widget widget)
  {
    if(length <= 12)
      return TextWidget.Cast(widget.FindAnyWidget("Title"));
    else if(length <= 16)
      return TextWidget.Cast(widget.FindAnyWidget("TitleSmall"));
    else if(length <= 22)
      return TextWidget.Cast(widget.FindAnyWidget("TitleVerySmall"));
    else
      return TextWidget.Cast(widget.FindAnyWidget("TitleExtraSmall"));
  }

  void ThreadFillCategoryProductList()
  {
    FillCategoryProductList();
  }

  void FillCategoryProductList(int index = 0)
  {
    if(index == 0)
      ClearAllProductsInCardList(ProductList);

    for(int i = index; i<CategoryItems.Products.Count();i++)
    {
      if(!this || !ProductList || !ParentMenu)return;
      Widget widget;
  		ItemPreviewWidget itemPlaceholder;
  		TextWidget	itemName;
      ImageWidget	itemHealth;
      ImageWidget	itemBag;
      TextWidget	itemStock;
      TextWidget	itemStockTitle;
      TextWidget	itemPrice;
      TextWidget  itemCountOnYou;

      int cA,cR,cG,cB;
      string classname = CategoryItems.Products[i].ClassName;
      string category = GetTraderPlusClient().GetProductCategory(classname);
      int    quantity = CategoryItems.Products[i].Quantity;
      string strquantity = CategoryItems.Products[i].Quantity.ToString();
      int    health = CategoryItems.Products[i].Health;
      int    maxstock= CategoryItems.Products[i].MaxStock;
      int    tradeqty;
      int    price = GetTraderPlusClient().CalculatePriceForThatItem(ParentMenu.m_QtyMultiplier,false,category,classname,quantity,health,maxstock, tradeqty);

      ProductList.Show(true);
      widget = GetGame().GetWorkspace().CreateWidgets( ParentMenu.GetProductCardLayout(), ProductList);
      if (!widget) return;
      itemPlaceholder = ItemPreviewWidget.Cast(widget.FindAnyWidget("ProductPreview"));
      string itemType = classname;
      if(itemType.Contains(GetTraderPlusConfigClient().LicenceKeyWord))itemType = "Paper";
    	EntityAI localEnt = EntityAI.Cast(GetGame().CreateObject(itemType, vector.Zero, true, false));
      if (localEnt)
      {
          itemPlaceholder.SetItem(localEnt);
          itemPlaceholder.SetModelPosition( Vector( 0, 0, 0.5 ) );
          itemPlaceholder.SetModelOrientation( vector.Zero );
          itemPlaceholder.SetView( localEnt.GetViewIndex() );
          if(!localCategoryItems)localCategoryItems = new array<EntityAI>;
          localCategoryItems.Insert(localEnt);
      }

      itemHealth  = ImageWidget.Cast(widget.FindAnyWidget("Health"));
      itemBag = ImageWidget.Cast(widget.FindAnyWidget("ImageBag"));
      itemStockTitle= TextWidget.Cast(widget.FindAnyWidget("StockCountTitle"));
      itemStock  = TextWidget.Cast(widget.FindAnyWidget("StockCountInput"));
      itemPrice = TextWidget.Cast(widget.FindAnyWidget("Price"));
      itemCountOnYou= TextWidget.Cast(widget.FindAnyWidget("CountOnYou"));

      int countOnYou = GetTraderPlusClient().GetSpecificItemCountOnPlayer(classname, health, ParentMenu.m_PlayerItems);
      if(countOnYou > 0)
        itemCountOnYou.SetText(countOnYou.ToString());
      else itemBag.Show(false);

      string displayName = TraderPlusHelper.GetDisplayName(classname);
      itemName = GetCardTitleWidget(displayName.Length(), widget);
      itemName.SetText(displayName);
      itemName.Show(true);
      string healthText = TraderPlusHelper.GetHealthFromLevel(health,cA,cR,cG,cB);
      itemPrice.SetText(TraderPlusHelper.IntToCurrencyString(price, ","));
      itemHealth.SetColor(ARGB(cA,cR,cG,cB));
      if(strquantity=="-1" || maxstock == -1)
        strquantity="∞";
      else
        strquantity = strquantity + "/" + maxstock.ToString();
      itemStockTitle.SetText("#tpm_stock"+" "+":");
      itemStock.SetText(strquantity);
      if(quantity == maxstock && quantity != -1)
      {
        itemName.SetColor(ARGB(255,191,48,48) );
        itemHealth.SetColor(ARGB(255,191,48,48) );
        itemStock.SetColor(ARGB(255,191,48,48) );
      }
      if(TraderPlusHelper.IsItemCompatible(classname, ParentMenu.m_compatibleItemsFromItemInHand))
      {
        Widget highLightProduct = Widget.Cast(widget.FindAnyWidget("Highlight"));
        highLightProduct.Show(true);
        highLightProduct.SetColor(ARGB(250,216,89,1));
        itemName.SetColor(ARGB(250,216,89,1));
      }
      m_WidgetStockList.Insert(widget, i);

      if(i - index > 25)
      {
        GetGame().GetCallQueue(CALL_CATEGORY_GUI).CallLater(FillCategoryProductList, 10, false, ++i);
        return;
      }
    }
  }

  void ToggleProductList()
  {
      if(ProductList && ProductList.IsVisible())
        ProductList.Show(false);
      else
        FillCategoryProductList();
  }

  //look to see if previous product trade still exist so we keep showing it in the product information panel
  TraderPlusArticle  CheckifPreviousProductExist(string productname,int health)
  {
    for(int i=0;i<CategoryItems.Products.Count();i++)
    {
      if(CategoryItems.Products[i].ClassName==productname && CategoryItems.Products[i].Health == health)
        return CategoryItems.Products[i];
    }
    return NULL;
  }

  bool OnDoubleClick(Widget w, int x, int y, int button)
  {
    if(w.GetUserID() == 20062002)
    {
      #ifdef TRADERPLUSDEBUG
      GetTraderPlusLogger().LogInfo("OnClick Special list name:"+w.GetParent().GetName());
      #endif
      int i;
      if(m_WidgetStockList.Find(w.GetParent(),i))
      {
        if(ParentMenu.m_HighLightProduct)
        {
          ParentMenu.m_HighLightProduct.Show(false);
        }
        else
        {
          ParentMenu.m_HighLightProduct = Widget.Cast(w.GetParent().FindAnyWidget("Highlight"));
          ParentMenu.m_HighLightProduct.Show(true);
        }
        ParentMenu.m_ArticleSelected = CategoryItems.Products[i];
        ParentMenu.QuickBuyHandler();
        return true;
      }
      else
      {
          ParentMenu.m_ArticleSelected = NULL;
      }
    }

    return false;
  }

  bool OnClick (Widget w, int x, int y, int button)
  {
    if(w.GetParent().GetParent() == CategoryCard)
    {
      #ifdef TRADERPLUSDEBUG
      GetTraderPlusLogger().LogInfo("CategoryCard clicked");
      #endif
      ParentMenu.m_CategoryCardSelected = this;
      ParentMenu.PlaySoundEvent();
      ToggleProductList();
      return true;
    }

    if(w.GetUserID() == 20062002)
    {
      int i;
      if(m_WidgetStockList.Find(w.GetParent(),i))
      {
        if(ParentMenu.m_HighLightProduct)ParentMenu.m_HighLightProduct.Show(false);
        ParentMenu.m_HighLightProduct = Widget.Cast(w.GetParent().FindAnyWidget("Highlight"));
        ParentMenu.m_HighLightProduct.Show(true);
        ParentMenu.m_InsuranceCheckBox.SetChecked(false);
        ParentMenu.m_ArticleSelected = CategoryItems.Products[i];
        ParentMenu.ResetMultiplier();
        ParentMenu.StockListHandler();
        return true;
      }
    }
    return true;
  }
}
