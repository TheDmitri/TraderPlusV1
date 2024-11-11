class TraderPlusMenu extends UIScriptedMenu
{
  //Widget of TraderPlusUI
  private   Widget            m_Background;
            Widget            m_HighLightProduct;
  private   Widget            m_PanelStockOutline;
  private   Widget            m_LicenceWarningPanel;
  private   Widget            m_StockScrollPanel;
  private   Widget            m_PanelInventoryOutline;
  private   Widget            m_InventoryScrollPanel;
  private   Widget            m_PanelWidgetPreview;
  private   Widget            m_BuyPanelWidget;
  private   Widget            m_SellPanelWidget;
  TraderPlusCategoryMenu      m_CategoryCardSelected;
  private   MultilineTextWidget m_LicenceWarningText;

  private   ButtonWidget      m_BttnMarket;
  private   ButtonWidget      m_BttnInventory;
  private   ButtonWidget      m_BttnCar;
  private   ButtonWidget      m_BttnLicence;
  private   ButtonWidget      m_BttnInsurance;

  private   ButtonWidget      m_BttnPlayerPreview;
  private   ButtonWidget      m_LicenceCloseBttn;
  private   ButtonWidget      m_BttnReset;
  private   ButtonWidget      m_BttnRemove;
  private   ButtonWidget      m_BttnSell;
  private   ButtonWidget      m_BttnBuy;
  private   ButtonWidget      m_BttnKit;
  private   ItemPreviewWidget m_MainItemPreview;
  private   PlayerPreviewWidget m_PlayerPreview;
  private   TextWidget        m_BuyPriceText;
  private   TextWidget        m_SellPriceText;
  private   TextWidget        m_InventoryCountText;
  private   TextWidget        m_PlayerMoneyText;
  private   TextWidget        m_TraderNameText;
  private   TextWidget        m_TraderDescriptionText;
  private   TextWidget        m_InsuranceTextOuput;
  private   TextWidget        m_CargoCount;
  private   TextWidget        m_SizeCount;
  private   MultilineTextWidget m_ItemDescription;
  private   RichTextWidget    m_InformationText;
  private   EditBoxWidget     m_SearchInventoryBox;
  private   EditBoxWidget     m_SearchStockBox;
  private   XComboBoxWidget   m_StockCategories;
  private   XComboBoxWidget   m_InventoryCategories;
  private   XComboBoxWidget   m_QuantityMultiplier;
  private   CheckBoxWidget    m_ShowAllCheckBox;
            CheckBoxWidget    m_InsuranceCheckBox;

  private   ImageWidget       m_ImagePlayerPreview;

  private   GridSpacerWidget  m_GridQuantity;
  private   GridSpacerWidget  m_GridInsurance;
  private   GridSpacerWidget  m_GridCargo;
  private   GridSpacerWidget  m_GridCenter;

  private   ref array<GridSpacerWidget>	m_StockGrids;
  private   GridSpacerWidget	m_MasterStockGrid;
  private   ScrollWidget      m_StockScrollWidget;

  private   ref array<GridSpacerWidget>	m_InventoryGrids;
  private   GridSpacerWidget	m_MasterInventoryGrid;
  private   ScrollWidget      m_InventoryScrollWidget;

  //Preview item and player
  protected EntityAI          m_previewItem;
  protected PlayerBase        m_previewPlayer;

  private   int               TraderID;                  //int trader ID
  private   vector            TraderPos;                 //vector trader position

  private   int               m_SelectedPage;
  private   int               m_selectedRowStock;        //int of the current selected row in stock
  private   int               m_selectedRowInventory;    //int of the current selected row in inventory
  private   int               m_selectedCatInventory;    //int of current category selected in inventory
  private   int               m_selectedCatStock;        //int of the current selected category in stock
  private   int               m_playerMoney;             //int value of the amoubt of money in the player
            int               m_QtyMultiplier;           //int value that represent how many times when multiply default tradeqty

  //item preview part
  private   int               m_MainItemPreviewRotationX;
  private   int               m_MainItemPreviewRotationY;
  private   vector            m_MainItemPreviewOrientation;
  private   int 				      m_MainItemPreviewScale;

  //playerpreview part
  protected int               m_CharacterRotationX;
	protected int               m_CharacterRotationY;
	//protected int              m_CharacterScaleDelta;
	protected vector            m_CharacterOrientation;

  private   float             m_transactionTick;         //tick between 2 transaction - 0.5s
  private   float             m_UIUpdate;                //tick between two UI update - 1s
  private   float             m_ScrollListUpdate;        //tick between two product refresh when scrolling - 0.05s
  private   bool              m_canTradeRequest;         //bool that allow trade each m_transactionTick is reached, if not reached display NOT TOO FAST
  private   bool              m_canTrade;                //bool that allow trade if all condition are met : quantity ok, player has enough money etc...
  private   bool              m_RequestTrade;            //bool that is true when trade has been sent to server, will be true until response is back
  private   bool              m_SellMode;                //bool that is true when we're in inventory list false if we're in stock list
  private   bool              m_BuyMode;                 //bool that is true when we're in stock list false if we're in inventory list
  private   bool              m_ShowAll;                 //if check box show all is enable, we show all potential goods in the trader
  private   string            m_MissingLicences;         //string that contains all the missing licenses to show in the warning frame
  private   string            m_SearchStock;
  private   string            m_SearchInventory;
  private   string            m_ItemDescriptionText;
  private   string            m_CategoryOfLastTrade;

  private   Object            m_Trader;                  //npc object

  private   ref map<int, EntityAI> localStockEntities;
  private   ref array<EntityAI> localInventoryEntities;
            TraderPlusArticle m_ArticleSelected;
  private   int m_InventoryListPosition;
  private   int m_PreviewID;

  //array list of all potential items that can be attached to item in hand
  ref TStringArray  m_compatibleItemsFromItemInHand;

  ref array<ref TraderPlusCategoryItems> m_StockItems;
	ref array<ref TraderPlusArticle> m_PlayerItems;

  //current product that is shown in the product information panel
  ref TraderPlusProduct m_CurrentProduct;

  private ref map<Widget, int>	m_WidgetStockList;
  private ref map<Widget, int>	m_WidgetInventoryList;

  ref array<ref TraderPlusCategoryMenu> m_CategoryMenuControllers;

  void TraderPlusMenu()
  {
    m_CategoryMenuControllers     = new array<ref TraderPlusCategoryMenu>;
    m_StockGrids                  = new array<GridSpacerWidget>;
    m_InventoryGrids              = new array<GridSpacerWidget>;
    localStockEntities            = new map<int, EntityAI>;
    localInventoryEntities        = new array<EntityAI>;
    m_WidgetStockList             = new map<Widget, int>();
    m_WidgetInventoryList         = new map<Widget, int>();
    m_StockItems                  = new array<ref TraderPlusCategoryItems>;
  	m_PlayerItems                 = new array<ref TraderPlusArticle>;
    m_compatibleItemsFromItemInHand = new TStringArray;
    m_CurrentProduct              = new TraderPlusProduct;
    m_InventoryListPosition       = -1;
    m_playerMoney                 = 0;
    m_transactionTick             = 0;
    m_canTradeRequest             = false;
    m_RequestTrade                = false;
    m_BuyMode                     = false;
    m_SellMode                    = false;
    m_ShowAll                     = false;
    m_SelectedPage                = 1;
    m_ScrollListUpdate            = 0.0;
    m_UIUpdate                    = 0.0;
    m_selectedCatInventory        = 0;
    m_selectedCatStock            = 0;
    m_ArticleSelected           = NULL;
    m_MissingLicences             = "";
    m_SearchStock                 = "";
    m_SearchInventory             = "";
    m_CategoryOfLastTrade         = "";
    m_CharacterOrientation        = "0 0 0";
    m_CategoryCardSelected        = NULL;
    m_PreviewID                   = TRADERPLUS_MAINITEMPREVIEW;
  }

  void ~TraderPlusMenu()
  {
    DestroyCategoryMenuControllers();
    DestroyLocalInventoryEntities();
  }

  void DestroyCategoryMenuControllers()
  {
    foreach(TraderPlusCategoryMenu catMenu:m_CategoryMenuControllers)
    {
      delete catMenu;
    }
    m_CategoryMenuControllers.Clear();
  }

  void DestroyAllChildren(Widget w)
	{
		while(w.GetChildren())
		{
			w.RemoveChild(w.GetChildren());
		}
	}

  void ClearAllCategoryCardList()
  {
    if(m_MasterStockGrid && m_StockScrollWidget)
    {
      m_StockScrollWidget.RemoveChild(m_MasterStockGrid);
      m_StockScrollPanel.RemoveChild(m_StockScrollWidget);
    }
    m_StockGrids.Clear();
  }

  /*void ClearAllStockList()
	{
		m_WidgetStockList.Clear();
    foreach(GridSpacerWidget stockGrid:m_StockGrids)
    {
      if(!stockGrid)continue;
      DestroyAllChildren(stockGrid);
      m_MasterStockGrid.RemoveChild(stockGrid);

    }
    m_StockGrids.Clear();
		DestroyLocalStockEntities();
	}*/

  void ClearAllInventoryList()
	{
		m_WidgetInventoryList.Clear();
    for(int i=0;i<m_InventoryGrids.Count();i++)
    {
      if(m_InventoryGrids[i])
      {
        DestroyAllChildren(m_InventoryGrids[i]);
        m_MasterInventoryGrid.RemoveChild(m_InventoryGrids[i]);
      }
    }
    m_InventoryGrids.Clear();
		DestroyLocalInventoryEntities();
	}

  void DestroyLocalInventoryEntities()
	{
		if (!localInventoryEntities || !localInventoryEntities.Count()) return;
		foreach (auto ent : localInventoryEntities)
		{
			if (ent)
				ent.Delete();
		}
    delete localInventoryEntities;
	}

  override Widget Init()
  {
		    layoutRoot = GetGame().GetWorkspace().CreateWidgets( GetTraderPlusLayoutPath() );
        m_StockScrollPanel = Widget.Cast(layoutRoot.FindAnyWidget("StockScrollPanel"));
        m_InventoryScrollPanel = Widget.Cast(layoutRoot.FindAnyWidget("InventoryScrollPanel"));
        m_LicenceWarningPanel = Widget.Cast(layoutRoot.FindAnyWidget( "LicenceWarningPanel" ));
        m_LicenceWarningText = MultilineTextWidget.Cast( layoutRoot.FindAnyWidget( "LicenceWarningText" ) );
        m_LicenceCloseBttn = ButtonWidget.Cast( layoutRoot.FindAnyWidget( "LicenceCloseBttn" ) );
        m_BttnPlayerPreview = ButtonWidget.Cast( layoutRoot.FindAnyWidget( "BttnPlayerPreview" ) );
        m_ImagePlayerPreview = ImageWidget.Cast( layoutRoot.FindAnyWidget( "ImagePlayerPreview"));
        m_BttnReset  = ButtonWidget.Cast( layoutRoot.FindAnyWidget( "BttnReset" ) );
        m_BttnRemove  = ButtonWidget.Cast( layoutRoot.FindAnyWidget( "BttnRemove" ) );
        m_BttnSell = ButtonWidget.Cast( layoutRoot.FindAnyWidget( "BttnSell" ) );
        m_BuyPanelWidget = Widget.Cast(layoutRoot.FindAnyWidget("BuyPanelWidget"));
        m_SellPanelWidget= Widget.Cast(layoutRoot.FindAnyWidget("SellPanelWidget"));
        m_BttnBuy= ButtonWidget.Cast( layoutRoot.FindAnyWidget( "BttnBuy" ) );
        m_BttnMarket= ButtonWidget.Cast( layoutRoot.FindAnyWidget( "BttnMarket" ) );
        m_BttnInventory= ButtonWidget.Cast( layoutRoot.FindAnyWidget( "BttnInventory" ) );
        m_BttnCar= ButtonWidget.Cast( layoutRoot.FindAnyWidget( "BttnCar" ) );
        m_BttnLicence= ButtonWidget.Cast( layoutRoot.FindAnyWidget( "BttnLicence" ) );
        m_BttnInsurance= ButtonWidget.Cast( layoutRoot.FindAnyWidget( "BttnInsurance" ) );
        m_InventoryCountText = TextWidget.Cast(layoutRoot.FindAnyWidget( "InventoryCountText" ));
        m_SearchInventoryBox = EditBoxWidget.Cast(layoutRoot.FindAnyWidget( "SearchInventoryBox" ));
        m_SearchStockBox = EditBoxWidget.Cast(layoutRoot.FindAnyWidget( "SearchStockBox" ));
        m_StockCategories = XComboBoxWidget.Cast(layoutRoot.FindAnyWidget( "StockCategories" ));
        m_InventoryCategories = XComboBoxWidget.Cast(layoutRoot.FindAnyWidget( "InventoryCategories" ));
        m_QuantityMultiplier = XComboBoxWidget.Cast(layoutRoot.FindAnyWidget( "QuantityMultiplier" ));
        m_PlayerMoneyText = TextWidget.Cast(layoutRoot.FindAnyWidget( "PlayerMoneyText" ));
        m_TraderNameText = TextWidget.Cast(layoutRoot.FindAnyWidget( "TraderNameText" ));
        m_TraderDescriptionText = TextWidget.Cast(layoutRoot.FindAnyWidget( "TraderDescriptionText" ));
        m_BuyPriceText = TextWidget.Cast(layoutRoot.FindAnyWidget( "BttnBuy_label" ));
        m_SellPriceText = TextWidget.Cast(layoutRoot.FindAnyWidget( "BttnSell_label" ));
        m_InformationText = RichTextWidget.Cast(layoutRoot.FindAnyWidget( "InformationText" ));
        m_ItemDescription = MultilineTextWidget.Cast(layoutRoot.FindAnyWidget( "ItemDescription" ));
        m_Background = Widget.Cast(layoutRoot.FindAnyWidget("ContentPanel"));
        m_PanelInventoryOutline = Widget.Cast(layoutRoot.FindAnyWidget("PanelInventoryOutline"));
        m_PanelStockOutline= Widget.Cast(layoutRoot.FindAnyWidget("PanelStockOutline"));
        m_ShowAllCheckBox = CheckBoxWidget.Cast(layoutRoot.FindAnyWidget("ShowAllCheckBox"));
        m_InsuranceCheckBox = CheckBoxWidget.Cast(layoutRoot.FindAnyWidget("InsuranceCheckBox"));
        m_InsuranceTextOuput = TextWidget.Cast(layoutRoot.FindAnyWidget( "InsuranceTextOuput" ));
        m_BttnKit=ButtonWidget.Cast(layoutRoot.FindAnyWidget("BttnKit"));
        m_GridQuantity = GridSpacerWidget.Cast(layoutRoot.FindAnyWidget("GridQuantity"));
        m_GridInsurance = GridSpacerWidget.Cast(layoutRoot.FindAnyWidget("GridInsurance"));
        m_GridCargo = GridSpacerWidget.Cast(layoutRoot.FindAnyWidget("GridCargo"));
        m_CargoCount = TextWidget.Cast(layoutRoot.FindAnyWidget("CargoCount"));
        m_SizeCount = TextWidget.Cast(layoutRoot.FindAnyWidget("SizeCount"));
        m_GridCenter = GridSpacerWidget.Cast(layoutRoot.FindAnyWidget("GridCenter"));
        m_PanelWidgetPreview = Widget.Cast(layoutRoot.FindAnyWidget("PanelWidgetPreview"));
        m_BttnKit.Show(false);
        m_BttnReset.Show(false);
        m_BttnRemove.Show(false);
        m_GridInsurance.Show(false);
        m_BttnCar.Show(!GetTraderPlusConfigClient().HideGarageBtn);
        m_BttnLicence.Show(!GetTraderPlusConfigClient().HideLicenceBtn);
        m_BttnInsurance.Show(!GetTraderPlusConfigClient().HideInsuranceBtn);
        m_ShowAllCheckBox.Show(GetTraderPlusConfigClient().EnableShowAllCheckBox);
        layoutRoot.Show(false);
        return layoutRoot;
    }


  //Some of you may be interrested by changing the layout of the trader, so here is the function you can override to change the path:
  string GetTraderPlusLayoutPath()
  {
    return "TraderPlus/gui/TraderPlusUI.layout";
  }

  void UIHandle()
  {
      //Before anything else, we check for potential licenses set to the trader
      LicencesHandler();

      m_canTrade = true;

      //Display trader names and roles
      GetTraderNameAndRole();

      //we check if item in hand is a weapon, if so, we get the compatible items link to it
      ItemInHandHandler();

      //We initiaze the categories from player's inventory
      InitPlayerCategories();
      //Then we get the remaining item regarding that category and we fill the inventory list
      PlayerCategoryHandler();

      //We initiaze the categories
      InitStockCategories();
      //we get items in stock from that category then we fill the list
      StockCategoryHandler();

      //in case of a previous trade, we try to show the current product to sell
      InventoryListHandler();

      m_BuyPanelWidget.Show(true);
      m_SellPanelWidget.Show(false);

      //create the player preview based on the player's character
      if(m_PreviewID == TRADERPLUS_PLAYERPREVIEW)
        InitPlayerPreview();

      MoneyUpdate();
  }

  void LicencesHandler()
  {
    if(!TraderPlusHelper.HasLicences(TraderID, m_MissingLicences))
    {
      m_Background.Show(false);
      m_LicenceWarningPanel.Show(true);
      m_LicenceWarningText.SetText("#tpm_licence_msg"+ "\n" +m_MissingLicences);
    }
    else
    {
      m_Background.Show(true);
      m_LicenceWarningPanel.Show(false);
    }
  }

  void ItemInHandHandler()
  {
    PlayerBase player = PlayerBase.Cast(GetGame().GetPlayer());
    string     weaponInHand =TraderPlusHelper.GetItemInHand(player);
    if(weaponInHand!="")m_compatibleItemsFromItemInHand = TraderPlusHelper.GetCompatibleAttachmentsArray(weaponInHand);
  }

  void MoneyUpdate()
  {
    PlayerBase player = PlayerBase.Cast(GetGame().GetPlayer());
    m_playerMoney = TraderPlusHelper.GetPlayerMoney(player, TraderID);
    string stringMoney = TraderPlusHelper.IntToCurrencyString(m_playerMoney, ",");
    m_PlayerMoneyText.SetText("#tpm_money"+ " " +stringMoney);
  }

  override void Update(float timeslice)
  {
      if(m_UIUpdate > 0.0)
      {
        MoneyUpdate();
        InitPlayerCategories();
        PlayerCategoryHandler();
        m_UIUpdate -= timeslice;
      }

      if(m_ScrollListUpdate>=0.1)
      {
        SearchUpdate();

        if(m_BuyMode){
          m_BttnBuy.Enable(true);
          m_BttnSell.Enable(false);
        }
        else{
          m_BttnSell.Enable(true);
          m_BttnBuy.Enable(false);
        }

        m_ScrollListUpdate = 0.0;
      }
      else
      {
        m_ScrollListUpdate += timeslice;
      }

      if (!m_canTradeRequest)
      {
          m_transactionTick -= timeslice;
          if (m_transactionTick <= 0)
          {
                m_transactionTick = TRADERPLUS_TRANSACTION_INTERVAL;
                m_canTradeRequest = true;
          }
      }
  }

  void SearchUpdate()
  {
    SearchInventoryUpdate();
    SearchStockUpdate();
  }

  //search update function when one of the search bar changed
  void SearchInventoryUpdate()
  {
    string searchInventory = m_SearchInventoryBox.GetText();
    if(searchInventory != m_SearchInventory)
    {
      m_SearchInventory = searchInventory;
      ResetInventoryList();
      PlayerCategoryHandler();
    }
  }

  //search update function when one of the search bar changed
  void SearchStockUpdate()
  {
    string searchStock    = m_SearchStockBox.GetText();
    if(searchStock != m_SearchStock)
    {
      m_SearchStock = searchStock;
      ResetStockList();
      StockCategoryHandler();
    }
  }

  void ShowAllCheckBoxHandler()
  {
    if(m_ShowAllCheckBox.IsChecked())
      m_ShowAll = true;
    else
      m_ShowAll = false;

    InitStockCategories();
    StockCategoryHandler();
  }

  //when mousebuttondown used, start update rotation for either item preview or player preview
  bool OnMouseButtonDown(Widget w, int x, int y, int button)
  {
    super.OnMouseButtonDown(w, x, y, button);
    switch(w)
    {
      case m_MainItemPreview:
      {
        GetGame().GetDragQueue().Call(this, "UpdateRotation");
        g_Game.GetMousePos(m_MainItemPreviewRotationX, m_MainItemPreviewRotationY);
        return true;
      }
      break;

      case m_PlayerPreview:
      {
        GetGame().GetDragQueue().Call(this, "UpdatePlayerRotation");
        g_Game.GetMousePos( m_CharacterRotationX, m_CharacterRotationY );
        return true;
      }
      break;
    }

    return false;
  }

  //update rotation for item preview
  void UpdateRotation(int mouse_x, int mouse_y, bool is_dragging)
  {
    vector orientation = m_MainItemPreviewOrientation;
    orientation[0] = orientation[0] + (m_MainItemPreviewRotationY - mouse_y);
    orientation[1] = orientation[1] - (m_MainItemPreviewRotationX - mouse_x);

    m_MainItemPreview.SetModelOrientation( orientation );

    if (!is_dragging)
      m_MainItemPreviewOrientation = orientation;
  }

  //update rotation for player preview
  void UpdatePlayerRotation(int mouse_x, int mouse_y, bool is_dragging)
  {
			vector orientation = m_CharacterOrientation;
			orientation[1] = orientation[1] - ( m_CharacterRotationX - mouse_x );

			m_PlayerPreview.SetModelOrientation( orientation );

			if ( !is_dragging )
				m_CharacterOrientation = orientation;
  }

  void GetItemDescription()
  {
    if(m_previewItem)
    {
      InventoryItem iItem = InventoryItem.Cast(m_previewItem);
      if(iItem)
        m_InformationText.SetText(TraderPlusHelper.TrimUnt(iItem.GetTooltip()));
    }
  }

  override bool OnMouseEnter(Widget w, int x, int y)
  {
    super.OnMouseEnter(w, x, y);
    switch(w)
    {
      case m_MainItemPreview:GetItemDescription();
      return true;
      break;
    }

    return false;
  }

	override bool OnMouseLeave(Widget w, Widget enterW, int x, int y)
  {
    super.OnMouseLeave(w, enterW, x, y);
    switch(w)
    {
      case m_MainItemPreview:
      {
        m_InformationText.SetText("");
        if(m_BuyMode)
          StockListHandler();
        else
          InventoryListHandler();
      }
      return true;
      break;
    }

    return false;
  }

  //update item preview scale, maybe player too in the future
  override bool OnMouseWheel(Widget  w, int  x, int  y, int wheel)
  {
    super.OnMouseWheel(w, x, y, wheel);

    switch(w)
    {
      case m_MainItemPreview:
      {
        m_MainItemPreviewScale = wheel;
        UpdateScale();
        return true;
      }
      break;
    }
    return false;
  }

  //update item preview scale
  void UpdateScale()
  {
    float w, h, x, y;
    m_MainItemPreview.GetPos(x, y);
    m_MainItemPreview.GetSize(w,h);
    w = w + ( m_MainItemPreviewScale / 4);
    h = h + ( m_MainItemPreviewScale / 4 );
    if ( w > 0.5 && w < 3 )
    {
      m_MainItemPreview.SetSize( w, h );

      // align to center
      int screen_w, screen_h;
      GetScreenSize(screen_w, screen_h);
      float new_x = x - ( m_MainItemPreviewScale / 8 );
      float new_y = y - ( m_MainItemPreviewScale / 8 );
      m_MainItemPreview.SetPos( new_x, new_y );
    }
  }

  //filter inventory list based on searchfield in inv search bar
  void ShowFilteredInventoryList(string inventorycontent)
  {
    foreach(int idx, TraderPlusArticle playerItem: m_PlayerItems)
    {
      string showedName = TraderPlusHelper.GetDisplayName(playerItem.ClassName);
      showedName.ToLower();
      inventorycontent.ToLower();
      if(!showedName.Contains(inventorycontent) && !playerItem.ClassName.Contains(inventorycontent))
        m_PlayerItems.RemoveOrdered(idx);
    }
    FillInventoryList(m_PlayerItems.Count());
  }

  //filter stock list based on searchfield in inv search bar
  void ShowFilteredStockList(string stockcontent)
  {
    foreach(TraderPlusCategoryItems categoryItems: m_StockItems)
    {
      foreach(int idx, TraderPlusArticle article: categoryItems.Products)
      {
        string showedName = TraderPlusHelper.GetDisplayName(article.ClassName);
        showedName.ToLower();
        stockcontent.ToLower();
        if(!showedName.Contains(stockcontent) && !article.ClassName.Contains(stockcontent))
          categoryItems.Products.RemoveOrdered(idx);
      }
    }
    thread FillStockList();
  }

  //display trader name and role
  void GetTraderNameAndRole()
  {
    for(int i=0;i<GetTraderPlusConfigClient().TraderPos.Count();i++)
    {
      vector traderpos = GetTraderPlusConfigClient().TraderPos[i];
      float  distance  = vector.Distance(traderpos,TraderPos);
      if(distance < 1 )
      {
        m_TraderNameText.SetText(GetTraderPlusConfigClient().GivenNames[i]);
        m_TraderDescriptionText.SetText(GetTraderPlusConfigClient().Roles[i]);
      }
    }
  }

  //clear and define default categories for inventory list
  void InitPlayerCategories()
  {
    m_InventoryCategories.ClearAll();
    m_InventoryCategories.AddItem("#tpm_sellable");
    m_InventoryCategories.AddItem("#tpm_all");
    m_InventoryCategories.AddItem("#tpm_licences");
    m_InventoryCategories.AddItem("#tpm_vehicles");
    m_InventoryCategories.AddItem("#tpm_insurances");
    m_InventoryCategories.SetCurrentItem(m_selectedCatInventory);
  }

  //clear and define all categories linked to the traderID, add ALL in addition
  void InitStockCategories()
  {
    m_StockCategories.ClearAll();
    int categoryCount = GetTraderPlusClient().GetCategoriesFromID(TraderID);
    for (int i = 0; i < categoryCount; i++)
    {
      m_StockCategories.AddItem(GetTraderPlusClient().m_StockCategories[i]);
    }
    m_StockCategories.SetCurrentItem(m_selectedCatStock);
  }

  string GetStockGridLayout()
  {
    return "TraderPlus/gui/StockGrid.layout";
  }

  string GetCategoryUILayout()
  {
    return "TraderPlus/gui/CategoryUI.layout";
  }

  string GetProductCardLayout()
  {
    return "TraderPlus/gui/ProductCard.layout";
  }

  //fill inventory list based on all the arrays "m_playerItems"
  void FillInventoryList(int inventorycount)
  {
    ClearAllInventoryList();
    m_QtyMultiplier = 1;
    if(!m_InventoryScrollWidget)
    {
      m_InventoryScrollWidget = ScrollWidget.Cast(GetGame().GetWorkspace().CreateWidgets( GetStockGridLayout(), m_InventoryScrollPanel));
      m_MasterInventoryGrid = GridSpacerWidget.Cast(m_InventoryScrollWidget.FindAnyWidget("StockGrid"));
    }
    int nbgrid = inventorycount/600 + 1;
    for(int i=0;i<nbgrid;i++)
    {
      GridSpacerWidget tempGrid = GridSpacerWidget.Cast(GetGame().GetWorkspace().CreateWidgets( GetCategoryUILayout(), m_MasterInventoryGrid));
      m_InventoryGrids.Insert(tempGrid);
    }
    int count = 0;
    for(int j = 0; j<inventorycount;j++)
    {
      Widget widget;
  		ItemPreviewWidget itemPlaceholder;
  		TextWidget	itemName;
      ImageWidget	itemHealth;
      ImageWidget	itemBag;
      TextWidget	itemCount;
      TextWidget	itemCountTitle;
      TextWidget	itemPrice;

      int cA,cR,cG,cB;
      int tradeqty,maxstock, stock = 0;
      int currentGrid = j/600;
      string classname = m_PlayerItems[j].ClassName;
      int    health    = m_PlayerItems[j].Health;
      string category = GetTraderPlusClient().GetProductCategory(classname);
      stock = GetTraderPlusClient().GetStockProductFromStock(category,classname,health);
      //if stock qty exist, we define our stock var
      int    price = GetTraderPlusClient().CalculatePriceForThatItem(m_QtyMultiplier,TRADEMODE_SELL,category,classname,stock,health,maxstock, tradeqty);

      if(m_InventoryGrids[currentGrid] == NULL)return;
  		widget = GetGame().GetWorkspace().CreateWidgets( GetProductCardLayout(), m_InventoryGrids[currentGrid]);

  		if (!widget) return;

  		itemPlaceholder = ItemPreviewWidget.Cast(widget.FindAnyWidget("ProductPreview"));
  		itemName 		= TextWidget.Cast(widget.FindAnyWidget("Title"));
      itemHealth  = ImageWidget.Cast(widget.FindAnyWidget("Health"));
      itemBag     = ImageWidget.Cast(widget.FindAnyWidget("ImageBag"));
      itemCountTitle= TextWidget.Cast(widget.FindAnyWidget("StockCountTitle"));
      itemCount  = TextWidget.Cast(widget.FindAnyWidget("StockCountInput"));
      itemPrice = TextWidget.Cast(widget.FindAnyWidget("Price"));

      itemBag.Show(false);

      string itemType = classname;
      if(itemType.Contains(GetTraderPlusConfigClient().LicenceKeyWord))itemType = "Paper";
  		EntityAI localEnt = EntityAI.Cast(GetGame().CreateObject(itemType, vector.Zero, true, false));

      string displayName = TraderPlusHelper.GetDisplayName(classname);
      itemName = GetCardTitleWidget(displayName.Length(),widget);
      itemName.SetText(displayName);
      itemName.Show(true);

      #ifdef TRADERPLUSDEBUG
  		GetTraderPlusLogger().LogInfo("Product:"+classname + " health:"+health);
  		#endif
      string healthText = TraderPlusHelper.GetHealthFromLevel(health,cA,cR,cG,cB);
      itemPrice.SetText(TraderPlusHelper.IntToCurrencyString(price, ","));
      itemHealth.SetColor(ARGB(cA,cR,cG,cB));
      itemCountTitle.SetText("#tpm_count"+" "+":");
      itemCount.SetText(m_PlayerItems[j].Count.ToString());
      ItemBase temp = ItemBase.Cast(localEnt);
      if(temp)
      {
        if(temp.IsInherited(Ammunition_Base) && !classname.Contains("Mag"))
        {
          Print("Mag + Ammunition_Base");
          itemCount.SetText(m_PlayerItems[j].Quantity.ToString());
        }
      }
      m_WidgetInventoryList.Insert(widget, j);
      if (localEnt)
      {
        itemPlaceholder.SetItem(localEnt);
        itemPlaceholder.SetModelPosition( Vector( 0, 0, 0.5 ) );
        itemPlaceholder.SetModelOrientation( vector.Zero );
        itemPlaceholder.SetView( localEnt.GetViewIndex() );
        if(!localInventoryEntities)localInventoryEntities = new array<EntityAI>;
        localInventoryEntities.Insert(localEnt);
      }
    }
  }

  string GetInsuranceLayout()
  {
    return "TraderPlus/gui/InsuranceCard.layout";
  }

  //fill all the stock list based on arrays "m_StockItems"
  void FillInsuranceList()
  {
    ClearAllInventoryList();

    m_QtyMultiplier = 1;
    if(!m_InventoryScrollWidget)
    {
      m_InventoryScrollWidget = ScrollWidget.Cast(GetGame().GetWorkspace().CreateWidgets( GetStockGridLayout(), m_InventoryScrollPanel));
      m_MasterInventoryGrid = GridSpacerWidget.Cast(m_InventoryScrollWidget.FindAnyWidget("StockGrid"));
    }
    int insuranceCount = GetBankAccount().Insurances.Count();
    int nbgrid = insuranceCount/600 + 1;
    for(int j=0;j<nbgrid;j++)
    {
      GridSpacerWidget tempGrid = GridSpacerWidget.Cast(GetGame().GetWorkspace().CreateWidgets( GetCategoryUILayout(), m_MasterInventoryGrid));
      m_InventoryGrids.Insert(tempGrid);
    }

    for(int i=0;i<insuranceCount;i++)
    {
      Widget widget;
  		ItemPreviewWidget itemPlaceholder;
  		TextWidget	itemName;
      Widget	    insuranceStatut;
      TextWidget	textStatut;
      TextWidget	itemPrice;

      int cA,cR,cG,cB;
      int currentGrid = j/600;
      string classname = TraderPlusHelper.GetVehicleTypeInsuranceData(GetBankAccount().Insurances.GetElement(i));
      //if stock qty exist, we define our stock var
      int price = TraderPlusHelper.GetCollateralPriceInsuranceData(GetBankAccount().Insurances.GetElement(i));

      if(m_InventoryGrids[currentGrid] == NULL)return;
  		widget = GetGame().GetWorkspace().CreateWidgets( GetInsuranceLayout(), m_InventoryGrids[currentGrid]);

  		if (!widget) return;

  		itemPlaceholder = ItemPreviewWidget.Cast(widget.FindAnyWidget("ProductPreview"));
  		itemName 		= TextWidget.Cast(widget.FindAnyWidget("Title"));
      itemPrice = TextWidget.Cast(widget.FindAnyWidget("Price"));
      insuranceStatut  = Widget.Cast(widget.FindAnyWidget("InsuranceStatut"));
      textStatut= TextWidget.Cast(widget.FindAnyWidget("TextStatut"));

      string itemType = classname;
  		EntityAI localEnt = EntityAI.Cast(GetGame().CreateObject(itemType, vector.Zero, true, false));

      string displayName = TraderPlusHelper.GetDisplayName(classname);

      itemName = GetCardTitleWidget(displayName.Length(),widget);
      itemName.SetText(displayName);
      itemName.Show(true);

      itemPrice.SetText(TraderPlusHelper.IntToCurrencyString(price, ","));
      if(GetTraderPlusClient().m_InsuranceStatus[i])
      {
        insuranceStatut.SetColor(ARGB(255,18,143,49));
        textStatut.SetText("#tpm_vehicle_assured");
        //textStatut.SetText("#tpm_vehicle_assured");
      }
      else
      {
        textStatut.SetText("#tpm_claim_insurance_money");
        //textStatut.SetText("#tpm_claim_insurance_money");
        insuranceStatut.SetColor(ARGB(255,209,137,60));
      }

      ItemBase temp = ItemBase.Cast(localEnt);
      m_WidgetInventoryList.Insert(widget, i);
      if (localEnt)
      {
        itemPlaceholder.SetItem(localEnt);
        itemPlaceholder.SetModelPosition( Vector( 0, 0, 0.5 ) );
        itemPlaceholder.SetModelOrientation( vector.Zero );
        itemPlaceholder.SetView( localEnt.GetViewIndex() );

        if(!localInventoryEntities)
          localInventoryEntities = new array<EntityAI>;

        localInventoryEntities.Insert(localEnt);
      }
    }
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

  //fill all the stock list based on arrays "m_StockItems"
  void FillStockList()
  {
    DestroyCategoryMenuControllers();
    ClearAllCategoryCardList();

    TStringArray categories = new TStringArray;
    if(m_selectedCatStock == 0)
      categories = GetTraderPlusClient().m_StockCategories;
    else
      categories.Insert(GetTraderPlusClient().m_StockCategories[m_selectedCatStock]);

    if(!m_StockScrollWidget)
    {
      m_StockScrollWidget = ScrollWidget.Cast(GetGame().GetWorkspace().CreateWidgets( GetStockGridLayout(), m_StockScrollPanel));
      m_MasterStockGrid = GridSpacerWidget.Cast(m_StockScrollWidget.FindAnyWidget("StockGrid"));
    }

    foreach(TraderPlusCategoryItems categoryItems: m_StockItems)
    {
      TraderPlusCategoryMenu categoryMenu = new TraderPlusCategoryMenu(m_MasterStockGrid, categoryItems, this);

      if((m_CategoryOfLastTrade!="" && m_CategoryOfLastTrade == categoryItems.CategoryName) || m_StockItems.Count() == 1 || m_SearchStock != "")
      {
        m_CategoryCardSelected = categoryMenu;
        categoryMenu.ThreadFillCategoryProductList();
      }

      m_CategoryMenuControllers.Insert(categoryMenu);
    }
  }

  void InsuranceCategoryHandler()
  {
    m_selectedCatInventory = m_InventoryCategories.GetCurrentItem();
    ResetPlayerItemsArray();
    FillInsuranceList();
  }

  //Get category and show item in inv list according to the category chosen
  void PlayerCategoryHandler()
  {
    m_selectedCatInventory = m_InventoryCategories.GetCurrentItem();
    ResetPlayerItemsArray();

    if(m_selectedCatInventory == TraderPlusInventoryCategory.INSURANCES)
    {
        FillInsuranceList();
    }
    else
    {
      int inventoryCount = GetTraderPlusClient().GetPlayerItemsFromCategory(m_selectedCatInventory,m_PlayerItems,m_SearchInventory);
      FillInventoryList(inventoryCount);
    }
  }

  //Get category and show item in stock list according to the category chosen
  void StockCategoryHandler()
	{
		m_selectedCatStock = m_StockCategories.GetCurrentItem();
    ResetStockItemsArrays();
    int stockcount = GetTraderPlusClient().GetItemInStockFromCategory(m_selectedCatStock,m_StockItems, m_ShowAll, m_SearchStock);
    thread FillStockList();
	}

  //Clear the combo multiplier when it's a an item available in stock
  void RefreshAndSetQuantityMultiplierStock(int tradeqty, int maxitemqty, int itemqty = -1)
  {
    m_QuantityMultiplier.ClearAll();
    if(tradeqty == -1 || tradeqty == 0 || (tradeqty == 1 && maxitemqty <= 1 && itemqty <= 1))
    {
      m_QuantityMultiplier.AddItem("1");
      m_QuantityMultiplier.SetCurrentItem(0);
      return;
    }
    else
    {
      if(maxitemqty<itemqty)maxitemqty = itemqty;
      int count = maxitemqty/tradeqty;
      int pos   = 0;
      if(count > 50)count=50;
      for(int i = 0;i<count;i++)
      {
        string data = ((i+1)*tradeqty).ToString();
        m_QuantityMultiplier.AddItem(data);
      }
      m_QuantityMultiplier.SetCurrentItem(0);
    }
    return;
  }

  //Clear the combo multiplier when it's a an item available in inv
  void RefreshAndSetQuantityMultiplierInventory(string classname, int tradeqty, int maxitemqty, int itemqty = -1, int itemcount = 1)
  {
    m_QuantityMultiplier.ClearAll();
    int count = 1;
    if(itemqty == -1)itemqty = maxitemqty;
    if(tradeqty != 0)count = itemqty / tradeqty;
    if(classname.Contains("Mag_"))
    {
      if(tradeqty == 0)
      {
        count = itemcount;
        tradeqty = 1;
      }
      else
      {
        count = itemqty / tradeqty;
        tradeqty = 1;
      }
    }
    if(maxitemqty == 0)
    {
      count = itemcount;
      tradeqty = 1;
    }
    if(count > 50)count=50;
    for(int i = 0;i<count;i++)
    {
      string data = ((i+1)*tradeqty).ToString();
      m_QuantityMultiplier.AddItem(data);
    }
    m_QuantityMultiplier.SetCurrentItem(0);
  }

  //When clicking on the multiplier combo, we refresh the invotorylisthandler or the stockhandler
  void QuantityMultiplierHandler()
  {
    if(m_CurrentProduct.Price == -1)return;
    m_QtyMultiplier = m_QuantityMultiplier.GetCurrentItem();
    m_QtyMultiplier+=1;
    if(m_CurrentProduct.TradMode)
      InventoryListHandler();
    else
      StockListHandler();
  }

  void ResetMultiplier()
  {
    m_QtyMultiplier = 1;
    m_QuantityMultiplier.ClearAll();
    m_QuantityMultiplier.AddItem("1");
  }

  //Buy quickly when double clicking a product in the list
  void QuickBuyHandler()
  {
    ResetMultiplier();
    m_BuyMode = true;
    m_SellMode = false;
    StockListHandler();
    BuyHandler();
  }

  //Sell quickly when double clicking a product in the list
  void QuickSellHandler()
  {
    ResetMultiplier();
    m_BuyMode = false;
    m_SellMode = true;
    InventoryListHandler();
    SellHandler();
  }

  //On double click is needed so we can trade simply by double clicking
  bool OnDoubleClick(Widget w, int x, int y, int button)
  {
    foreach(TraderPlusCategoryMenu categoryMenu: m_CategoryMenuControllers)
    {
      categoryMenu.OnDoubleClick(w,x,y,button);
    }

    if(w.GetUserID() == 20062002)
    {
      #ifdef TRADERPLUSDEBUG
      GetTraderPlusLogger().LogInfo("OnClick Special list name:"+w.GetParent().GetName());
      #endif
      int j;

      if(m_WidgetInventoryList.Find(w.GetParent(),j))
      {
        if(m_HighLightProduct)
        {
          m_HighLightProduct.Show(false);
        }
        else
        {
          m_HighLightProduct = Widget.Cast(w.GetParent().FindAnyWidget("Highlight"));
          m_HighLightProduct.Show(true);
        }
        m_InventoryListPosition = j;
        QuickSellHandler();
        return true;
      }
      else
      {
        m_InventoryListPosition = -1;
      }
      return true;
    }

    return false;
  }

  void ResetStockList()
  {
    if(m_StockScrollWidget)
    {
      DestroyAllChildren(m_StockScrollWidget);
      delete m_StockScrollWidget;
      delete m_MasterStockGrid;
    }
  }

  void ResetInventoryList()
  {
    if(m_InventoryScrollWidget)
    {
      DestroyAllChildren(m_InventoryScrollWidget);
      delete m_InventoryScrollWidget;
      delete m_MasterInventoryGrid;
    }
  }

  //on click needed for a lot of things
  override bool OnClick (Widget w, int x, int y, int button)
  {
    super.OnClick(w,x,y,button);

    foreach(TraderPlusCategoryMenu categoryMenu: m_CategoryMenuControllers)
    {
      categoryMenu.OnClick(w,x,y,button);
    }

    if(w.GetUserID() == 20062002)
    {
      int j;

      if(m_WidgetInventoryList.Find(w.GetParent(),j))
      {
        if(m_HighLightProduct) m_HighLightProduct.Show(false);
        m_HighLightProduct = Widget.Cast(w.GetParent().FindAnyWidget("Highlight"));
        m_HighLightProduct.Show(true);
        m_InventoryListPosition = j;
        ResetMultiplier();
        InventoryListHandler();
        return true;
      }
      else
        m_InventoryListPosition = -1;

      return true;
    }

    switch(w)
    {
      case m_ShowAllCheckBox:ShowAllCheckBoxHandler();
      return true;
      break;

      case m_BttnMarket:BttnMenuHandler(0,TRADEMODE_BUY);
      return true;
      break;

      case m_BttnInventory:BttnMenuHandler(TraderPlusInventoryCategory.SELLABLE,TRADEMODE_SELL);
      return true;
      break;

      case m_BttnLicence:BttnMenuHandler(TraderPlusInventoryCategory.LICENCES,TRADEMODE_SELL);
      return true;
      break;

      case m_BttnCar:BttnMenuHandler(TraderPlusInventoryCategory.VEHICLES,TRADEMODE_SELL);
      return true;
      break;

      case m_BttnInsurance:BttnMenuHandler(TraderPlusInventoryCategory.INSURANCES,TRADEMODE_SELL);
      return true;
      break;

      case m_BttnPlayerPreview: SwitchPreview();
      return true;
      break;

      case m_BttnKit:KitHandler();
      return true;
      break;

      case m_LicenceCloseBttn:OnHide();
      return true;
      break;

      case m_InsuranceCheckBox:StockListHandler();
      return true;
      break;

      case m_InventoryCategories:
      {
        ResetInventoryList();
        PlayerCategoryHandler();
      }
      return true;
      break;

      case m_StockCategories:
      {
        ResetStockList();
        StockCategoryHandler();
        LicenceCheck(GetTraderPlusClient().m_StockCategories.Get(m_StockCategories.GetCurrentItem()));
        GetGame().GetCallQueue(CALL_CATEGORY_GUI).CallLater(PlayerCategoryHandler, 200);
        m_ArticleSelected = NULL;
        return true;
      }
      break;

      case m_QuantityMultiplier:QuantityMultiplierHandler();
      return true;
      break;

      case m_BttnBuy: BuyHandler();
      return true;
      break;

      case m_BttnSell: SellHandler();
      return true;
      break;

      case m_BttnRemove:RemoveAllPlayerClothes();
      return true;
      break;

      case m_BttnReset:InitPlayerPreview();
      return true;
      break;
    }
    return false;
  }

  void RemovePreview(int previewID)
  {
    if(previewID == TRADERPLUS_MAINITEMPREVIEW)
    {
      Widget wItem = m_GridCenter.FindAnyWidgetById(9889);
      if(wItem)
        m_GridCenter.RemoveChild(wItem);

      if(m_MainItemPreview){
        delete m_MainItemPreview;
      }
    }

    if(previewID == TRADERPLUS_PLAYERPREVIEW)
    {
      Widget wPlayer = m_GridCenter.FindAnyWidgetById(9888);
      if(wPlayer)
        m_GridCenter.RemoveChild(wPlayer);

        if(m_PlayerPreview){
          delete m_PlayerPreview;
        }
    }
  }

  void SwitchPreview()
  {
    if(m_PreviewID == TRADERPLUS_MAINITEMPREVIEW)
    {
      m_PreviewID = TRADERPLUS_PLAYERPREVIEW;
      RemovePreview(TRADERPLUS_MAINITEMPREVIEW);
      InitPlayerPreview();
      UpdatePlayerClothes();
      m_ItemDescription.Show(false);
      m_GridInsurance.Show(false);
      m_BttnReset.Show(true);
      m_BttnRemove.Show(true);
      m_ImagePlayerPreview.SetColor(ARGB(255,40,159,70));

      m_GridCenter.Update();
    }else if(m_PreviewID == TRADERPLUS_PLAYERPREVIEW)
    {
      m_PreviewID = TRADERPLUS_MAINITEMPREVIEW;
      m_ItemDescription.Show(true);
      RemovePreview(TRADERPLUS_PLAYERPREVIEW);
      m_GridCenter.Update();
      m_ImagePlayerPreview.SetColor(ARGB(255,255,255,255));
      m_BttnReset.Show(false);
      m_BttnRemove.Show(false);
      if(m_CurrentProduct.ClassName != "")
        UpdateItemPreview(m_CurrentProduct.ClassName);
    }
  }

  void DisplayTradePanel(bool inventory)
  {
    m_PanelStockOutline.Show(!inventory);
    m_PanelInventoryOutline.Show(inventory);
  }

  void PlaySoundEvent()
  {
    PlayerBase player = PlayerBase.Cast(GetGame().GetPlayer());
    if(player)
      player.PlayTradeSound(TraderPlusSound.QUICKEVENT);
  }

  void BttnMenuHandler(int bttnType, bool tradingMode)
  {
    PlaySoundEvent();
    DisplayTradePanel(tradingMode);

    GetTraderPlusClient().GetTraderPlusItemsListFromAllCategorySA();

    if(tradingMode)
    {
      m_InventoryCategories.SetCurrentItem(bttnType);

      if(bttnType != TraderPlusInventoryCategory.INSURANCES)
        PlayerCategoryHandler();
      else
        InsuranceCategoryHandler();
    }
    else
    {
      m_StockCategories.SetCurrentItem(bttnType);
      StockCategoryHandler();
    }
  }

  void LicenceCheck(string categoryName)
  {
    if(GetBankAccount() && GetBankAccount().Licences){
      if(categoryName.Contains(GetTraderPlusConfigClient().LicenceKeyWord) && GetBankAccount().Licences.Find(categoryName) == -1)
        m_InformationText.SetText("#tpm_licence_msg"+ "\n" +categoryName);
        return;
    }
    m_InformationText.SetText("");
  }

  //look to see if previous product trade still exist so we keep showing it in the product information panel
  int  CheckifPreviousProductExist(string productname,int health)
  {
      for(int j=0;j<m_PlayerItems.Count();j++)
      {
        if(m_PlayerItems[j].ClassName == productname && m_PlayerItems[j].Health == health)
        {
          if(TraderPlusHelper.GetQuantityOfSpecificItem(PlayerBase.Cast(GetGame().GetPlayer()),productname,health) != 0)
            return j;
        }
      }

    return -1;
  }

  void GetCargoCountHandler(string classname)
  {
    int slotcount = TraderPlusHelper.GetItemSlotCount(classname);
    if(slotcount != 0)
      m_CargoCount.SetText("#tpm_slot"+ " " +slotcount.ToString());
    else
      m_CargoCount.SetText("");
  }

  void GetSizeCountHandler(string classname)
  {
    int sizeCount = TraderPlusHelper.GetItemSizeCount(classname);
    if(sizeCount != 0)
      m_SizeCount.SetText("#tpm_size"+ " " +sizeCount.ToString());
    else
      m_SizeCount.SetText("");
  }

  //We a product is selected in stock, we'll run all the functions to check price based on stock, quantity, health, etc.. in order to make the trade ready to be reauested
  void StockListHandler()
  {
    if(!m_CategoryCardSelected)return;

    SetTradeMode(TRADEMODE_BUY);

    m_BuyPanelWidget.Show(true);
    m_SellPanelWidget.Show(false);

    m_ItemDescriptionText = "";

    TraderPlusArticle retrievedArticle = m_CategoryCardSelected.CheckifPreviousProductExist(m_CurrentProduct.ClassName,m_CurrentProduct.Health);
    if(!m_ArticleSelected)
    {
      if(retrievedArticle == NULL && m_CurrentProduct.MaxStock != -1)
      {
        ResetProductPanel();
        return;
      }
      else m_ArticleSelected = retrievedArticle;
    }

    string classname = m_ArticleSelected.ClassName;
    string category = GetTraderPlusClient().GetProductCategory(classname);
    m_CategoryOfLastTrade = category;
    int    health    = m_ArticleSelected.Health;

    UpdateItemPreview(classname);
    //Show item health status
    GetColorRegardingHealth(health);

    GetCargoCountHandler(classname);
    GetSizeCountHandler(classname);

    //Show quantity in stock
    int stock = m_ArticleSelected.Quantity;

    //Show sellprice of our item based on stock and qty desired
    int TradeQty, MaxStock, InsurancePrice = 0, CollateralMoney = 0;
    int MaxItemQty = TraderPlusHelper.GetMaxItemQuantityClient(classname);
    #ifdef TRADERPLUSDEBUG
		GetTraderPlusLogger().LogInfo("Buy Product: maxitemqty:"+MaxItemQty.ToString());
		#endif

    int Price = GetTraderPlusClient().CalculatePriceForThatItem(m_QtyMultiplier,false,category,classname,stock,health,MaxStock, TradeQty);
    if(TraderPlusHelper.HasInsurance(classname))
    {
      m_GridInsurance.Show(true);
      CollateralMoney = TraderPlusHelper.GetCollateralPriceBasedOnClassname(classname, Price);
      m_InsuranceTextOuput.SetText(CollateralMoney.ToString());

      if(m_InsuranceCheckBox.IsChecked())
        InsurancePrice = TraderPlusHelper.GetInsurancePriceBasedOnClassname(classname, Price);
      else
        CollateralMoney = 0;
    }
    else
    {
      m_GridInsurance.Show(false);
    }
    Price += InsurancePrice;
    int SellPrice = GetTraderPlusClient().CalculatePriceForThatItem(m_QtyMultiplier,true,category,classname,stock,health,MaxStock, TradeQty);
    //We check for price overflow in case server owner use ruble or chinese currency with high values
    if(Price > int.MAX)Price = -1;
    string StrPrice = TraderPlusHelper.IntToCurrencyString(Price, ",");

    float requiredQty;
    if(TradeQty == 0 || TradeQty == 1)
      requiredQty = 100;
    else
      requiredQty = ((TradeQty*100)/MaxItemQty);

    m_ItemDescriptionText += "\n" +  "#tpm_required_qty"+ " " + ":" + requiredQty.ToString()+"%";

    if(MaxStock == -1)stock = -1;
    string strstock = stock.ToString();
    if(m_QtyMultiplier > stock && MaxStock != -1 || Price == -1 || Price == 0)
    {
      if(stock == -1)
      {
        strstock="∞";
        StrPrice = "Ø";
      }
      else
      {
        m_InformationText.SetText("#tpm_tradeqty_over_stockqty");
      }
      m_canTrade = false;
    }
    else
    {
      if(stock == -1)strstock="∞";
      m_InformationText.SetText("");
      m_canTrade = true;
    }

    if(Price > m_playerMoney || StrPrice == "Ø")
    {
      m_InformationText.SetText("#tpm_cant_be_bought");
      m_BuyPriceText.SetText("#tpm_buy"+ " " +" - " +StrPrice);
      //m_ItemDescriptionText = m_ItemDescriptionText + "\n" + ;
      m_canTrade = false;
    }
    else
    {
      m_BuyPriceText.SetText("#tpm_buy" + " " + " - " +StrPrice);
      //m_ItemDescriptionText = m_ItemDescriptionText + "\n" + "#tpm_buyprice"+ " " +StrPrice;
    }

    if(classname.Contains(GetTraderPlusConfigClient().LicenceKeyWord) && GetBankAccount() && GetBankAccount().Licences.Find(classname) != -1)
    {
      m_InformationText.SetText("#tpm_already_bought");
      //m_ItemDescriptionText = m_ItemDescriptionText + "\n" + "#tpm_buyprice"+ " " +StrPrice;
      m_BuyPriceText.SetText("#tpm_buy"+ " " + " - " +StrPrice);
      m_canTrade = false;
    }

    string selltext = "#tpm_sell"+ " " + " - " +TraderPlusHelper.IntToCurrencyString(SellPrice, ",");
    if(SellPrice == -1)
    {
      selltext = "#tpm_sell"+ " " + " - " +"Ø";
    }

    m_ItemDescriptionText += "\n" + selltext;

    if(m_QuantityMultiplier.GetNumItems() == 1 && MaxItemQty != 0)
    {
      RefreshAndSetQuantityMultiplierStock(TradeQty, MaxItemQty,MaxItemQty);
    }

    m_ItemDescription.SetText(m_ItemDescriptionText);

    //we define the current product shown so we can transmit it more easely if player wants to buy it
    m_CurrentProduct.TraderID  = TraderID;
    m_CurrentProduct.Category  = category;
    m_CurrentProduct.ClassName = classname;
    m_CurrentProduct.Multiplier= m_QtyMultiplier;
    m_CurrentProduct.Quantity  = TradeQty;
    m_CurrentProduct.MaxStock  = MaxStock;
    m_CurrentProduct.Health    = health;
    m_CurrentProduct.Price     = Price;
    m_CurrentProduct.CollateralMoney = CollateralMoney;
    m_CurrentProduct.TradMode  = TRADEMODE_BUY;

    if(m_PreviewID == TRADERPLUS_PLAYERPREVIEW)
      UpdatePlayerClothes();
  }

  //reset the product var and hide the product information panel
  void ResetProductPanel()
  {
    if ( m_previewItem )
        GetGame().ObjectDelete( m_previewItem );
    m_InformationText.SetText("");
    m_BuyPriceText.SetText("#tpm_buy"+" ");
    m_SellPriceText.SetText("#tpm_sell"+" ");
    m_CategoryOfLastTrade = "";
    m_canTrade = false;
  }


  void SetTradeMode(bool trademode)
  {
      m_BuyMode = !trademode;
      m_SellMode = trademode;
  }

  void InsuranceListHandler()
  {
    SetTradeMode(TRADEMODE_SELL);
    m_BuyPanelWidget.Show(false);
    m_SellPanelWidget.Show(true);
    m_GridInsurance.Show(false);
    m_selectedRowInventory = m_InventoryListPosition;
    if(m_selectedRowInventory == -1)
    {
      ResetProductPanel();
      return;
    }

    string classname = TraderPlusHelper.GetVehicleTypeInsuranceData(GetBankAccount().Insurances.GetElement(m_selectedRowInventory));
    //if stock qty exist, we define our stock var
    int price = TraderPlusHelper.GetCollateralPriceInsuranceData(GetBankAccount().Insurances.GetElement(m_selectedRowInventory));

    if(GetTraderPlusClient().m_InsuranceStatus[m_selectedRowInventory])
      price = 0;

    UpdateItemPreview(classname);

    if(price > int.MAX)price = -1;
    string StrPrice = TraderPlusHelper.IntToCurrencyString(price, ",");;
    if(price == -1 || price == 0)StrPrice = "Ø";
    m_SellPriceText.SetText("#tpm_claim_insurance_money");
    m_BuyPriceText.SetText("Ø");
    //m_ItemDescriptionText += "\n" + "#tpm_sellprice"+ " " +StrPrice;

    /*case 1 : TradeQty > ItemQty => Itemty is too low for trading
      case 2 : (MaxStock < StockQty + TradeQty && MaxStock != -1) => Stock is to high to accept more of that specific item and stock is not unlimited
      case 3 : (MaxStock == -1 && MaxItemQty!=ItemQty && TradeQty < 0) => Stock is unlimited and ItemQty != ItemMaxQty because TradeQty is set to maxqty
    */
    if(price == -1 || price == 0)
    {
      m_InformationText.SetText("#tpm_cant_trade_item");
      m_canTrade = false;
    }
    else if(GetTraderPlusConfigClient().AuthorizedIDInsurance.Find(TraderID) == -1)
    {
      m_InformationText.SetText("#tpm_cant_claim_insurance");
      m_canTrade = false;
    }
    else
    {
      m_InformationText.SetText("");
      m_canTrade = true;
    }

    m_ItemDescription.SetText("");

    m_CurrentProduct.TraderID  = TraderID;
    m_CurrentProduct.Category  = "INSURANCE";
    m_CurrentProduct.ClassName = classname;
    m_CurrentProduct.Multiplier= 1;
    m_CurrentProduct.Quantity  = 0;
    m_CurrentProduct.Health    = TraderPlusItemState.PRISTINE;
    m_CurrentProduct.Price     = price;
    m_CurrentProduct.CollateralMoney = GetBankAccount().Insurances.GetKey(m_selectedRowInventory);
    m_CurrentProduct.MaxStock  = -1;
    m_CurrentProduct.TradMode  = TRADEMODE_SELLINSURANCE;

    #ifdef TRADERPLUSDEBUG
		GetTraderPlusLogger().LogInfo("inventory handler:"+ m_CurrentProduct.ClassName + " " + m_CurrentProduct.Quantity);
		#endif
  }

  //We a product is selected in inv, we'll run all the functions to check price based on stock, quantity, health, etc.. in order to make the trade ready to be reauested
  void InventoryListHandler()
  {
    if(m_selectedCatInventory == TraderPlusInventoryCategory.INSURANCES)
    {
      InsuranceListHandler();
      return;
    }

    SetTradeMode(TRADEMODE_SELL);

    m_BuyPanelWidget.Show(false);
    m_SellPanelWidget.Show(true);

    m_ItemDescriptionText = "";

    m_selectedRowInventory = m_InventoryListPosition;
    #ifdef TRADERPLUSDEBUG
		GetTraderPlusLogger().LogInfo("inventory handler:m_InventoryListPosition:"+m_InventoryListPosition);
		#endif
    int tempRow = CheckifPreviousProductExist(m_CurrentProduct.ClassName,m_CurrentProduct.Health);
    #ifdef TRADERPLUSDEBUG
		GetTraderPlusLogger().LogInfo("inventory handler:m_CurrentProduct.ClassName:"+m_CurrentProduct.ClassName);
		#endif
    if(m_selectedRowInventory == -1)
    {
      if(tempRow == -1)
      {
        ResetProductPanel();
        return;
      }
      else
      {
        m_selectedRowInventory = tempRow;
      }
    }

    if(!m_PlayerItems[m_selectedRowInventory])return;

    string classname = m_PlayerItems[m_selectedRowInventory].ClassName;
    string category  = GetTraderPlusClient().GetProductCategory(classname);
    int    health    = m_PlayerItems[m_selectedRowInventory].Health;
    UpdateItemPreview(classname);

    GetCargoCountHandler(classname);

    //Show item health status
    GetColorRegardingHealth(health);

    //Show quantity in stock if exist
    int stock = 0;

    //if stock qty exist, we define our stock var
    stock = GetTraderPlusClient().GetStockProductFromStock(category, classname,health);

    int ItemQty,TradeQty,MaxItemQty,MaxStock=-1;
    ItemQty = m_PlayerItems[m_selectedRowInventory].Quantity;
    MaxItemQty = TraderPlusHelper.GetMaxItemQuantityClient(classname);
    #ifdef TRADERPLUSDEBUG
		GetTraderPlusLogger().LogInfo("Sell Product: maxitemqty:"+MaxItemQty.ToString());
		#endif

    //We define the price based on StockQty and our TradeQty var
    int Price = GetTraderPlusClient().CalculatePriceForThatItem(m_QtyMultiplier,TRADEMODE_SELL,category,classname,stock,health,MaxStock, TradeQty);
    //int BuyPrice = GetTraderPlusClient().CalculatePriceForThatItem(m_QtyMultiplier,TRADEMODE_BUY,category,classname,stock,health,MaxStock, TradeQty);
    //We check for price overflow in case server owner use ruble or chinese currency with high values
    if(Price > int.MAX)Price = -1;
    string StrPrice = TraderPlusHelper.IntToCurrencyString(Price, ",");;
    if(Price == -1 || Price == 0)StrPrice = "Ø";
    m_SellPriceText.SetText("#tpm_sell"+ " " + " - " +StrPrice);
    //m_ItemDescriptionText += "\n" + "#tpm_sellprice"+ " " +StrPrice;

    //m_ItemDescriptionText+= "\n"+"#tpm_buyprice"+ " " +TraderPlusHelper.IntToCurrencyString(BuyPrice, ",");
    // m_BuyPriceText.SetText("#tpm_buy"+ " " + " - " + TraderPlusHelper.IntToCurrencyString(BuyPrice, ","));
    // if(BuyPrice == -1)
    //   m_BuyPriceText.SetText("#tpm_buy"+ " " + " - " +"Ø");

    /*case 1 : TradeQty > ItemQty => Itemty is too low for trading
      case 2 : (MaxStock < StockQty + TradeQty && MaxStock != -1) => Stock is to high to accept more of that specific item and stock is not unlimited
      case 3 : (MaxStock == -1 && MaxItemQty!=ItemQty && TradeQty < 0) => Stock is unlimited and ItemQty != ItemMaxQty because TradeQty is set to maxqty
    */
    if(Price == -1 || Price == 0 || TradeQty > ItemQty || (MaxStock < stock + m_QtyMultiplier && MaxStock != -1) || (MaxStock == -1 && MaxItemQty!=ItemQty && TradeQty < 0))
    {
      if(Price == -1 || Price == 0)m_InformationText.SetText("#tpm_cant_trade_item");
      if(TradeQty > ItemQty)m_InformationText.SetText("#tpm_tradeqty_over_itemqty");
      if(MaxStock < stock + m_QtyMultiplier && MaxStock != -1)m_InformationText.SetText("#tpm_tradeqty_over_stockqty");
      if(MaxStock == -1 && MaxItemQty!=ItemQty && TradeQty < 0)m_InformationText.SetText("#tpm_tradeqty_over_itemqty");
      m_ItemDescriptionText += "\n" + "#tpm_totalqty"+ " " +ItemQty;
      m_canTrade = false;
    }
    else
    {
      m_InformationText.SetText("");
      m_ItemDescriptionText += "\n" + "#tpm_totalqty"+ " " +ItemQty;
      m_canTrade = true;
    }

    if(m_QuantityMultiplier.GetNumItems() == 1)
      RefreshAndSetQuantityMultiplierInventory(classname,TradeQty, MaxItemQty, ItemQty,m_PlayerItems[m_selectedRowInventory].Count);

    //we define the current product shown so we can transmit it more easely if player wants to sell it
    #ifdef TRADERPLUSDEBUG
		GetTraderPlusLogger().LogInfo("inventory handler:"+ m_CurrentProduct.ClassName + " " + TradeQty + " qtymulti:"+m_QtyMultiplier);
		#endif

    float requiredQty = 0.0;
    if(TradeQty == 0 || TradeQty == 1)
      requiredQty = 100;
    else
      requiredQty = ((TradeQty*100)/MaxItemQty);

    m_ItemDescriptionText += "\n" +  "#tpm_required_qty"+ " " + ":"+ requiredQty.ToString()+"%";

    m_ItemDescription.SetText(m_ItemDescriptionText);

    m_CurrentProduct.TraderID  = TraderID;
    m_CurrentProduct.Category  = category;
    m_CurrentProduct.ClassName = classname;
    m_CurrentProduct.Multiplier= m_QtyMultiplier;
    m_CurrentProduct.Quantity  = TradeQty;
    if(m_QtyMultiplier > 1 && TradeQty == 0)m_CurrentProduct.Quantity = m_QtyMultiplier;
    m_CurrentProduct.Health    = health;
    m_CurrentProduct.Price     = Price;
    m_CurrentProduct.MaxStock  = MaxStock;
    m_CurrentProduct.TradMode  = TRADEMODE_SELL;
    if(m_selectedCatInventory == TraderPlusInventoryCategory.VEHICLES){
      m_CurrentProduct.TradMode  = TRADEMODE_SELLVEHICLE;
      m_CurrentProduct.Quantity  = m_selectedRowInventory;
    }

    #ifdef TRADERPLUSDEBUG
		GetTraderPlusLogger().LogInfo("inventory handler:"+ m_CurrentProduct.ClassName + " " + m_CurrentProduct.Quantity);
		#endif
  }

  //by pressing the sell button, we launch this function to attempt to send a trade reauest
  void SellHandler()
  {
    #ifdef TRADERPLUSDEBUG
    GetTraderPlusLogger().LogInfo("SellHandler");
    #endif
    if(m_selectedRowInventory == -1)return;
    if(!m_canTradeRequest)
    {
      m_InformationText.SetText("#tpm_not_too_fast");
      return;
    }
    if(m_CurrentProduct.Price == 0)
    {
      m_InformationText.SetText("#tpm_cant_trade_item");
      return;
    }
    if(m_canTrade)
    {
      if(m_PlayerItems[m_selectedRowInventory] && !m_PlayerItems[m_selectedRowInventory].HasAttachments)
      {
        m_InformationText.SetText("#tpm_empty_item_first");
        return;
      }
      GetRPCManager().SendRPC("TraderPlus", "GetTradeRequest",  new Param1<ref TraderPlusProduct>(m_CurrentProduct), true, NULL);
      m_RequestTrade = true;
      m_canTrade = false;
      m_canTradeRequest = false;
    }
    else m_InformationText.SetText("#tpm_cant_trade_item");
  }

  //by pressing the buy button, we launch this function to attempt to send a trade reauest
  void BuyHandler()
  {
    #ifdef TRADERPLUSDEBUG
    GetTraderPlusLogger().LogInfo("BuyHandler");
    #endif
    if(m_selectedRowStock == -1)return;
    if(!m_canTradeRequest)
    {
      m_InformationText.SetText("#tpm_not_too_fast");
      return;
    }
    if(m_CurrentProduct.Price == 0 || m_CurrentProduct.Price == -1)
    {
      m_InformationText.SetText("#tpm_cant_trade_item");
      return;
    }
    if(m_canTrade)
    {
      GetRPCManager().SendRPC("TraderPlus", "GetTradeRequest",  new Param1<ref TraderPlusProduct>(m_CurrentProduct), true, NULL);
      m_RequestTrade = true;
      m_canTrade = false;
      m_canTradeRequest = false;
      return;
    }
    else if(m_playerMoney < m_CurrentProduct.Price){
      m_InformationText.SetText("#tpm_dont_enough_money");
    }
    else
    {
      m_InformationText.SetText("#tpm_not_enough_qty_stock");
    }
  }

  //send a request to get the actual stock available for the dedicated ID
  void GetStockRequest(int id, vector pos, Object obj)
  {
    m_Trader = obj;
    TraderID = id;
    TraderPos= pos;
    GetRPCManager().SendRPC("TraderPlus", "GetStockRequestBasedOnID",  new Param1<Object>(m_Trader), true, NULL);
  }

  //once the trade reauest send, we're going to get a response afterward that will start this function based on the result
  void GetTradingResponse(int response)
  {
    //just in case they are spamming the trader, we remove a potential previous call
    GetGame().GetCallQueue(CALL_CATEGORY_GUI).Remove(InventoryListHandler);

    ResetMultiplier();
    m_UIUpdate = 1.0;
    m_canTrade = true;
    m_RequestTrade = false;

    PlayerBase player = PlayerBase.Cast(GetGame().GetPlayer());
    switch(response)
    {
      case TraderPlusResponse.SELL_SUCCESS:
      {
        m_InformationText.SetText("#tpm_sell_success");
        GetTraderPlusClient().GetTraderPlusItemsListFromAllCategorySA();
        GetGame().GetCallQueue(CALL_CATEGORY_GUI).CallLater(InventoryListHandler, 100);
        player.PlayTradeSound(TraderPlusSound.SELL);
      }
      break;

      case TraderPlusResponse.SELL_FAILED:
      {
        m_InformationText.SetText("#tpm_sell_failed");
        GetTraderPlusClient().GetTraderPlusItemsListFromAllCategorySA();
        GetGame().GetCallQueue(CALL_CATEGORY_GUI).CallLater(InventoryListHandler, 100);
      }
      break;

      case TraderPlusResponse.BUY_SUCCESS:
      {
        m_InformationText.SetText("#tpm_buy_success");
        player.PlayTradeSound(TraderPlusSound.BUY);
        StockListHandler();
      }
      break;

      case TraderPlusResponse.BUY_FAILED:
      {
        m_InformationText.SetText("#tpm_buy_failed");
        StockListHandler();
      }
      break;
    }
  }

  //update the UI once we got the tradingresponse
  void UIUpdate()
  {
    //if(response != TraderPlusResponse.BUY_SUCCESS)
    ResetStockItemsArrays();
    ResetPlayerItemsArray();
    UIHandle();
  }

  //reset the stock items arrays
  void ResetStockItemsArrays()
  {
    m_StockItems.Clear();
  }

  //reset the inv items arrays
  void ResetPlayerItemsArray()
  {
    m_PlayerItems.Clear();
  }

  //get the text and color based on the health
  void GetColorRegardingHealth(int health)
  {
    #ifdef TRADERPLUSDEBUG
    GetTraderPlusLogger().LogInfo("GetColorRegardingHealth:"+health);
    #endif
    if(!m_MainItemPreview)return;

    #ifdef TRADERPLUSDEBUG
    GetTraderPlusLogger().LogInfo("m_MainItemPreview exist");
    #endif
    ImageWidget itemHealth  = ImageWidget.Cast(m_MainItemPreview.FindAnyWidget("Health"));
    if(!itemHealth)return;
    switch(health)
    {
      case 0:
      {
        itemHealth.SetColor(ARGB(255,40,159,70));
      }
      break;
      case 1:
      {
        itemHealth.SetColor(ARGB(255,129,207,149));
      }
      break;
      case 2:
      {
        itemHealth.SetColor(ARGB(255,212,149,86));
      }
      break;
      case 3:
      {
        itemHealth.SetColor(ARGB(255,161,87,13));
      }
      break;
      case 4:
      {
        itemHealth.SetColor(ARGB(255,159,40,40));
      }
      break;
    }
  }

  //called when the current product display in the preview is a kit or contains 'kit'
  void KitHandler()
  {
    string itemType = m_CurrentProduct.ClassName;
    itemType.ToLower();
    if(itemType.Contains("_kit_"))
      return;
    else if(itemType.Contains("_kit"))
      itemType.Replace("_kit","");
    else if(itemType.Contains("kit_"))
      itemType.Replace("kit_","");
    UpdateItemPreview(itemType);
  }

  string GetItemPreviewLayout()
  {
    return "TraderPlus/gui/ItemPreview.layout";
  }

  //update item preview when item selected in the list change
  void UpdateItemPreview(string itemType)
	{
    if(m_PreviewID == TRADERPLUS_PLAYERPREVIEW)
      return;

    #ifdef TRADERPLUSDEBUG
    GetTraderPlusLogger().LogInfo("UpdateItemPreview:"+itemType);
    #endif
    //we check if item is a kit, if it is, we show the kit bttn
    string tempstr = itemType;
    tempstr.ToLower();
    if(tempstr.Contains("kit"))m_BttnKit.Show(true);
    if(itemType.Contains(GetTraderPlusConfigClient().LicenceKeyWord))itemType = "Paper";
    else m_BttnKit.Show(false);

		if ( !m_MainItemPreview )
			{
        Widget previewFrame = GetGame().GetWorkspace().CreateWidgets( GetItemPreviewLayout() );

        GridSpacerWidget gridBttn = GridSpacerWidget.Cast(layoutRoot.FindAnyWidget("GridBttn"));
        if(!gridBttn) return;
        m_GridCenter.AddChildAfter(previewFrame,m_ItemDescription);

        #ifdef TRADERPLUSDEBUG
        GetTraderPlusLogger().LogInfo("!m_MainItemPreview");
        #endif

				if ( previewFrame )
				{
          #ifdef TRADERPLUSDEBUG
          GetTraderPlusLogger().LogInfo("previewFrame");
          #endif
					float width;
					float height;
					previewFrame.GetSize(width, height);

					m_MainItemPreview = ItemPreviewWidget.Cast( GetGame().GetWorkspace().CreateWidget(ItemPreviewWidgetTypeID, 0, 0, 1, 1, WidgetFlags.VISIBLE, ARGB(255, 255, 255, 255), 10, previewFrame) );
        }
			}

			if ( m_previewItem )
				  GetGame().ObjectDelete( m_previewItem );

			m_previewItem = EntityAI.Cast(GetGame().CreateObject( itemType, "0 0 0", true, false, true ));
      if(!m_previewItem)return;

      //if(m_previewItem.IsInherited(CarScript))m_

      m_MainItemPreview.SetItem(m_previewItem);
      m_MainItemPreview.SetModelPosition( Vector( 0, 0, 0.5 ) );
      m_MainItemPreview.SetModelOrientation( vector.Zero );
      m_MainItemPreview.SetView( m_previewItem.GetViewIndex() );
	}

  void DeletePlayerPreview()
  {
    if(m_PlayerPreview)
        delete m_PlayerPreview;
    if(m_previewPlayer)
        GetGame().ObjectDelete(m_previewPlayer);
  }

  //called when pressing the remove button in the player rpeview panel
  void RemoveAllPlayerClothes()
  {
    if(!m_previewPlayer)
    {
      InitPlayerPreview();
    }
    else
    {
      CreatePlayerForPreview();
      InsertPlayerToPreview();
    }
  }

  void CreatePlayerForPreview()
  {
    if(m_previewPlayer)GetGame().ObjectDelete(m_previewPlayer);

    /*Some of you know I was having the face fucked up, thanks to Steve aka Salutesh and liquidrock, I got a fix but wtf, why ?? why should we do that for a fckg face texture !!!*/
    vector dir = GetGame().GetCurrentCameraDirection();
    dir.Normalize();
    vector pos = GetGame().GetCurrentCameraPosition() - dir * 0.5;
    pos[1] = GetGame().GetPlayer().GetPosition()[1];
    m_previewPlayer = PlayerBase.Cast(GetGame().CreateObjectEx(GetGame().GetPlayer().GetType(), pos, ECE_LOCAL|ECE_NOLIFETIME));
    m_previewPlayer.m_IsStashDisable = true;
  }

  void InsertPlayerToPreview()
  {
    if(m_PlayerPreview)
    {
      #ifdef TRADERPLUSDEBUG
      GetTraderPlusLogger().LogInfo("InsertPlayerToPreview");
      #endif
      m_PlayerPreview.SetPlayer( m_previewPlayer );
      m_PlayerPreview.UpdateItemInHands(m_previewPlayer.GetHumanInventory().GetEntityInHands());
      m_PlayerPreview.SetModelOrientation( m_CharacterOrientation );
      m_PlayerPreview.SetModelPosition( vector.Zero );
      m_PlayerPreview.SetSize( 1, 1 );  // default scale
    }
  }

  //Update the dummy clothes when we're selecting a new product in stock
  void UpdatePlayerClothes()
  {
    #ifdef TRADERPLUSDEBUG
    GetTraderPlusLogger().LogInfo("UpdatePlayerClothes");
    #endif

    if(!m_PlayerPreview)
    {
      InitPlayerPreview();
      #ifdef TRADERPLUSDEBUG
      GetTraderPlusLogger().LogInfo("!m_previewPlayer");
      #endif
    }

    if(m_CurrentProduct.ClassName == "")return;

    int id = TraderPlusHelper.GetSlotForPlayerPreview(m_CurrentProduct.ClassName);

    if(id != -1 && (TraderPlusHelper.playerAttachments[id] != "Hand" || TraderPlusHelper.playerAttachments[id] != "Shoulder" || TraderPlusHelper.playerAttachments[id] != "Melee"))
    {
      EntityAI ent = m_previewPlayer.FindAttachmentBySlotName(TraderPlusHelper.playerAttachments[id]);
      if(ent)
          GetGame().ObjectDelete(ent);
      int slotId = InventorySlots.GetSlotIdFromString(TraderPlusHelper.playerAttachments[id]);
      m_previewPlayer.GetInventory().CreateAttachmentEx(m_CurrentProduct.ClassName, slotId);
    }
    else
    {
      EntityAI current_entityInHand = m_previewPlayer.GetHumanInventory().GetEntityInHands();
      if(current_entityInHand)
          GetGame().ObjectDelete(current_entityInHand);
      EntityAI entityInHand = m_previewPlayer.GetHumanInventory().CreateInHands(m_CurrentProduct.ClassName);
    }

    InsertPlayerToPreview();
  }

  string GetPlayerPreviewLayout()
  {
    return "TraderPlus/gui/PlayerPreview.layout";
  }

  //Init the player preview to default setting, it means we display exactly the player's clothes
  void InitPlayerPreview()
	{
    if(!m_PlayerPreview)
    {
        Widget previewFrame = GetGame().GetWorkspace().CreateWidgets( GetPlayerPreviewLayout());

        m_GridCenter.AddChildAfter(previewFrame,m_ItemDescription);
        if ( previewFrame )
				{
					float width;
					float height;
					previewFrame.GetSize(width, height);

          m_PlayerPreview = PlayerPreviewWidget.Cast(GetGame().GetWorkspace().CreateWidget(PlayerPreviewWidgetTypeID, 0, 0, 1, 1, WidgetFlags.VISIBLE, ARGB(255, 255, 255, 255), 10, previewFrame) );
          m_PlayerPreview.Show(true);
        }
    }

    CreatePlayerForPreview();

    for(int i = 0; i < TraderPlusHelper.playerAttachments.Count();i++)
    {
      EntityAI ent = GetGame().GetPlayer().FindAttachmentBySlotName(TraderPlusHelper.playerAttachments[i]);
      if(ent)
      {
        if(TraderPlusHelper.playerAttachments[i] == "Hand")
        {
          EntityAI entityInHand = m_previewPlayer.GetHumanInventory().CreateInHands(ent.GetType());
          continue;
        }
        int slotId = InventorySlots.GetSlotIdFromString(TraderPlusHelper.playerAttachments[i]);
        m_previewPlayer.GetInventory().CreateAttachmentEx(ent.GetType(), slotId);
      }
    }

    InsertPlayerToPreview();
	}

  override void OnShow()
	{
		super.OnShow();
		PPEffects.SetBlurMenu(0.5);
		GetGame().GetInput().ChangeGameFocus(1);
    GetGame().GetUIManager().ShowUICursor(true);
    GetGame().GetMission().GetHud().Show(false);
    GetGame().GetMission().PlayerControlDisable(INPUT_EXCLUDE_ALL);
		SetFocus( layoutRoot );
	}

	override void OnHide()
	{
		super.OnHide();
		PPEffects.SetBlurMenu(0);
    GetGame().GetUIManager().ShowUICursor(false);
    GetGame().GetMission().GetHud().Show(true);
    GetGame().GetMission().PlayerControlEnable(false);
		GetGame().GetInput().ResetGameFocus();
    if ( m_previewItem )
        GetGame().ObjectDelete( m_previewItem );
    if (m_previewPlayer)
        GetGame().ObjectDelete( m_previewPlayer );
    m_CurrentProduct.ClassName = "";
		Close();
	}
}
