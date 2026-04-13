class TraderAppComponent extends DZlrComponentBase
{
    private static ref ComponentAutoRegister s_Register = new ComponentAutoRegister("trader-app-component", TraderAppComponent);

    // Bound properties (shell-level only)
    string traderName;
    string traderDescription;
    string playerMoneyText;
    string informationText;

    // Widgets loaded via AutoLoadWidgets
    Widget ContentPanel;
    Widget LicenceWarningPanel;
    Widget PanelStockOutline;
    Widget PanelInventoryOutline;
    Widget PanelIconsOutline;
    Widget ProductInformationOutline;
    MultilineTextWidget LicenceWarningText;
    RichTextWidget InformationText;

    // Child components
    private ref TraderNavComponent m_NavComponent;
    private ref TraderStockPanelComponent m_StockPanel;
    private ref TraderInventoryPanelComponent m_InventoryPanel;
    private ref TraderPreviewPanelComponent m_PreviewPanel;

    // Internal state
    private int m_TraderID;
    private vector m_TraderPos;
    private Object m_Trader;
    private int m_playerMoney;
    private bool m_canTradeRequest;
    private bool m_canTrade;
    private bool m_RequestTrade;
    private float m_transactionTick;
    private float m_UIUpdate;
    private float m_ScrollListUpdate;
    private string m_MissingLicences;
    private string m_CategoryOfLastTrade;
    private TraderPlusArticle m_ArticleSelected;
    private int m_InventoryListPosition;
    private ref TStringArray m_compatibleItemsFromItemInHand;

    override protected void RegisterMetadata()
    {
        m_Metadata = new ComponentMetadata();
        m_Metadata.selector = "trader-app-component";
        m_Metadata.templatePath = "TraderPlus/datasets/layouts/TraderAppComponent.layout";
    }

    void SetTraderData(int traderID, vector traderPos, Object traderObj)
    {
        m_TraderID = traderID;
        m_TraderPos = traderPos;
        m_Trader = traderObj;
    }

    int GetTraderID()
    {
        return m_TraderID;
    }

    override void OnInit()
    {
        m_compatibleItemsFromItemInHand = new TStringArray;
        m_InventoryListPosition = -1;
        m_playerMoney = 0;
        m_transactionTick = 0;
        m_canTradeRequest = false;
        m_RequestTrade = false;
        m_canTrade = true;
        m_MissingLicences = "";
        m_CategoryOfLastTrade = "";
        m_ScrollListUpdate = 0.0;
        m_UIUpdate = 0.0;
        m_ArticleSelected = NULL;

        // Create child components into their placeholder panels
        CreateChildComponents();

        // Subscribe to child events
        TraderNavComponent.Event_OnTabSelected.Insert(OnTabSelected);
        StockProductCardComponent.Event_OnProductSelected.Insert(OnStockProductSelected);
        InventoryProductCardComponent.Event_OnInventoryProductSelected.Insert(OnInventoryProductSelected);
        InsuranceCardComponent.Event_OnInsuranceSelected.Insert(OnInsuranceSelected);
        TraderPreviewPanelComponent.Event_OnBuyRequested.Insert(BuyHandler);
        TraderPreviewPanelComponent.Event_OnSellRequested.Insert(SellHandler);
        TraderPreviewPanelComponent.Event_OnQuantityChanged.Insert(OnQuantityChanged);
        TraderPreviewPanelComponent.Event_OnPreviewSwitchRequested.Insert(OnPreviewSwitch);

        // Send initial RPCs
        GetRPCManager().SendRPC("TraderPlus", "GetReceiptCarNameRequest", NULL, true, NULL);
        GetRPCManager().SendRPC("TraderPlus", "GetInsuranceRequest", NULL, true, NULL);
        GetRPCManager().SendRPC("TraderPlusBanking", "TraderPlusBankingRequest", NULL, true, NULL);
        int LowUID = GarageHelpers.GetLowSteamID(GetGame().GetUserManager().GetTitleInitiator().GetUid());
        GetRPCManager().SendRPC("Garage", "GarageRequest", new Param3<int, vector, bool>(LowUID, m_TraderPos, false), true, NULL);
        GetRPCManager().SendRPC("TraderPlus", "GetStockRequestBasedOnID", new Param1<Object>(m_Trader), true, NULL);

        // Start periodic update
        GetGame().GetCallQueue(CALL_CATEGORY_GUI).CallLater(PeriodicUpdate, 100, true);

        MarkForCheck();
    }

    void CreateChildComponents()
    {
        // Nav sidebar — create from layout, parent into placeholder
        if (PanelIconsOutline)
        {
            m_NavComponent = new TraderNavComponent();
            m_NavComponent.InitComponent(null);
            if (m_NavComponent.GetRoot())
            {
                PanelIconsOutline.AddChild(m_NavComponent.GetRoot());
            }
        }

        // Stock panel
        if (PanelStockOutline)
        {
            m_StockPanel = new TraderStockPanelComponent();
            m_StockPanel.SetTraderID(m_TraderID);
            m_StockPanel.InitComponent(null);
            if (m_StockPanel.GetRoot())
            {
                PanelStockOutline.AddChild(m_StockPanel.GetRoot());
            }
        }

        // Preview panel
        if (ProductInformationOutline)
        {
            m_PreviewPanel = new TraderPreviewPanelComponent();
            m_PreviewPanel.InitComponent(null);
            if (m_PreviewPanel.GetRoot())
            {
                ProductInformationOutline.AddChild(m_PreviewPanel.GetRoot());
            }
        }

        // Inventory panel
        if (PanelInventoryOutline)
        {
            m_InventoryPanel = new TraderInventoryPanelComponent();
            m_InventoryPanel.InitComponent(null);
            if (m_InventoryPanel.GetRoot())
            {
                PanelInventoryOutline.AddChild(m_InventoryPanel.GetRoot());
            }
        }
    }

    override void OnDestroy()
    {
        TraderNavComponent.Event_OnTabSelected.Remove(OnTabSelected);
        StockProductCardComponent.Event_OnProductSelected.Remove(OnStockProductSelected);
        InventoryProductCardComponent.Event_OnInventoryProductSelected.Remove(OnInventoryProductSelected);
        InsuranceCardComponent.Event_OnInsuranceSelected.Remove(OnInsuranceSelected);
        TraderPreviewPanelComponent.Event_OnBuyRequested.Remove(BuyHandler);
        TraderPreviewPanelComponent.Event_OnSellRequested.Remove(SellHandler);
        TraderPreviewPanelComponent.Event_OnQuantityChanged.Remove(OnQuantityChanged);
        TraderPreviewPanelComponent.Event_OnPreviewSwitchRequested.Remove(OnPreviewSwitch);
        GetGame().GetCallQueue(CALL_CATEGORY_GUI).Remove(PeriodicUpdate);
    }

    void CleanupPreview()
    {
        if (m_PreviewPanel)
        {
            m_PreviewPanel.CleanupPreview();
        }
    }

    // --- Periodic Update ---

    void PeriodicUpdate()
    {
        float timeslice = 0.1;

        if (m_UIUpdate > 0.0)
        {
            MoneyUpdate();
            if (m_InventoryPanel)
            {
                m_InventoryPanel.RefreshInventory();
            }
            m_UIUpdate -= timeslice;
        }

        if (m_ScrollListUpdate >= 0.1)
        {
            if (m_StockPanel)
            {
                m_StockPanel.SearchUpdate();
            }
            if (m_InventoryPanel)
            {
                m_InventoryPanel.SearchUpdate();
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

    // --- UIHandle ---

    void UIHandle()
    {
        LicencesHandler();
        m_canTrade = true;
        GetTraderNameAndRole();
        ItemInHandHandler();

        if (m_InventoryPanel)
        {
            m_InventoryPanel.RefreshInventory();
        }
        if (m_StockPanel)
        {
            m_StockPanel.RefreshStock();
        }
        if (m_PreviewPanel)
        {
            m_PreviewPanel.ShowBuyPanel();
        }

        MoneyUpdate();
    }

    void UIUpdate()
    {
        UIHandle();
    }

    // --- Data response handlers ---

    void OnStockDataReceived(int response, TraderPlusStock stock, string category)
    {
        UIUpdate();
    }

    void GetTradingResponse(int response)
    {
        if (m_PreviewPanel)
        {
            m_PreviewPanel.ResetMultiplier();
        }
        m_UIUpdate = 1.0;
        m_canTrade = true;
        m_RequestTrade = false;

        PlayerBase player = PlayerBase.Cast(GetGame().GetPlayer());
        switch (response)
        {
            case TraderPlusResponse.SELL_SUCCESS:
            {
                SetInfoText("#tpm_sell_success");
                GetTraderPlusClient().GetTraderPlusItemsListFromAllCategorySA();
                if (player)
                {
                    player.PlayTradeSound(TraderPlusSound.SELL);
                }
            }
            break;

            case TraderPlusResponse.SELL_FAILED:
            {
                SetInfoText("#tpm_sell_failed");
                GetTraderPlusClient().GetTraderPlusItemsListFromAllCategorySA();
            }
            break;

            case TraderPlusResponse.BUY_SUCCESS:
            {
                SetInfoText("#tpm_buy_success");
                if (player)
                {
                    player.PlayTradeSound(TraderPlusSound.BUY);
                }
                StockListHandler();
            }
            break;

            case TraderPlusResponse.BUY_FAILED:
            {
                SetInfoText("#tpm_buy_failed");
                StockListHandler();
            }
            break;
        }
    }

    // --- Trader info ---

    void GetTraderNameAndRole()
    {
        for (int i = 0; i < GetTraderPlusConfigClient().TraderPos.Count(); i++)
        {
            vector traderpos = GetTraderPlusConfigClient().TraderPos[i];
            float distance = vector.Distance(traderpos, m_TraderPos);
            if (distance < 1)
            {
                traderName = GetTraderPlusConfigClient().GivenNames[i];
                traderDescription = GetTraderPlusConfigClient().Roles[i];
            }
        }
        MarkForCheck();
    }

    // --- Licence handling ---

    void LicencesHandler()
    {
        if (!TraderPlusHelper.HasLicences(m_TraderID, m_MissingLicences))
        {
            if (ContentPanel)
            {
                ContentPanel.Show(false);
            }
            if (LicenceWarningPanel)
            {
                LicenceWarningPanel.Show(true);
            }
            if (LicenceWarningText)
            {
                LicenceWarningText.SetText("#tpm_licence_msg\n" + m_MissingLicences);
            }
        }
        else
        {
            if (ContentPanel)
            {
                ContentPanel.Show(true);
            }
            if (LicenceWarningPanel)
            {
                LicenceWarningPanel.Show(false);
            }
        }
    }

    void ItemInHandHandler()
    {
        PlayerBase player = PlayerBase.Cast(GetGame().GetPlayer());
        string weaponInHand = TraderPlusHelper.GetItemInHand(player);
        if (weaponInHand != "")
        {
            m_compatibleItemsFromItemInHand = TraderPlusHelper.GetCompatibleAttachmentsArray(weaponInHand);
        }
    }

    void MoneyUpdate()
    {
        PlayerBase player = PlayerBase.Cast(GetGame().GetPlayer());
        m_playerMoney = TraderPlusHelper.GetPlayerMoney(player, m_TraderID);
        string stringMoney = TraderPlusHelper.IntToCurrencyString(m_playerMoney, ",");
        playerMoneyText = "#tpm_money " + stringMoney;
        MarkForCheck();
    }

    void SetInfoText(string text)
    {
        informationText = text;
        if (InformationText)
        {
            InformationText.SetText(text);
        }
        if (m_PreviewPanel)
        {
            m_PreviewPanel.SetInformationText(text);
        }
        MarkForCheck();
    }

    // --- Tab navigation ---

    void OnTabSelected(int bttnType, bool tradingMode)
    {
        PlayerBase player = PlayerBase.Cast(GetGame().GetPlayer());
        if (player)
        {
            player.PlayTradeSound(TraderPlusSound.QUICKEVENT);
        }

        if (PanelStockOutline)
        {
            PanelStockOutline.Show(!tradingMode);
        }
        if (PanelInventoryOutline)
        {
            PanelInventoryOutline.Show(tradingMode);
        }

        GetTraderPlusClient().GetTraderPlusItemsListFromAllCategorySA();

        if (tradingMode)
        {
            if (m_InventoryPanel)
            {
                m_InventoryPanel.SetCategory(bttnType);
            }
        }
        else
        {
            if (m_StockPanel)
            {
                m_StockPanel.StockCategoryHandler();
            }
        }
    }

    void OnLicenceCloseClicked(ClickEventArgs args)
    {
        if (GetTraderPlusUIService())
        {
            GetTraderPlusUIService().CloseView();
        }
    }

    // --- Product selection ---

    void OnStockProductSelected(TraderPlusArticle article)
    {
        m_ArticleSelected = article;
        if (m_PreviewPanel)
        {
            m_PreviewPanel.ResetMultiplier();
        }
        StockListHandler();
    }

    void OnInventoryProductSelected(TraderPlusArticle article, int index)
    {
        m_InventoryListPosition = index;
        if (m_PreviewPanel)
        {
            m_PreviewPanel.ResetMultiplier();
        }
        InventoryListHandler();
    }

    void OnInsuranceSelected(int index)
    {
        m_InventoryListPosition = index;
        InsuranceListHandler();
    }

    // --- StockListHandler ---

    void StockListHandler()
    {
        if (!m_PreviewPanel)
        {
            return;
        }

        m_PreviewPanel.ShowBuyPanel();

        if (!m_ArticleSelected)
        {
            m_PreviewPanel.ResetPreview();
            m_canTrade = false;
            return;
        }

        string classname = m_ArticleSelected.ClassName;
        string category = GetTraderPlusClient().GetProductCategory(classname);
        m_CategoryOfLastTrade = category;
        int health = m_ArticleSelected.Health;
        int qtyMul = m_PreviewPanel.GetQtyMultiplier();

        m_PreviewPanel.UpdateItemPreview(classname);

        int stock = m_ArticleSelected.Quantity;
        int TradeQty, MaxStock, InsurancePrice, CollateralMoney;
        InsurancePrice = 0;
        CollateralMoney = 0;

        int Price = GetTraderPlusClient().CalculatePriceForThatItem(qtyMul, false, category, classname, stock, health, MaxStock, TradeQty);

        if (TraderPlusHelper.HasInsurance(classname))
        {
            m_PreviewPanel.SetInsuranceVisible(true);
            CollateralMoney = TraderPlusHelper.GetCollateralPriceBasedOnClassname(classname, Price);
            m_PreviewPanel.SetInsuranceText(CollateralMoney.ToString());
            if (m_PreviewPanel.IsInsuranceChecked())
            {
                InsurancePrice = TraderPlusHelper.GetInsurancePriceBasedOnClassname(classname, Price);
            }
            else
            {
                CollateralMoney = 0;
            }
        }
        else
        {
            m_PreviewPanel.SetInsuranceVisible(false);
        }

        Price += InsurancePrice;
        int SellPrice = GetTraderPlusClient().CalculatePriceForThatItem(qtyMul, true, category, classname, stock, health, MaxStock, TradeQty);

        if (Price > int.MAX)
        {
            Price = -1;
        }
        string StrPrice = TraderPlusHelper.IntToCurrencyString(Price, ",");

        if (MaxStock == -1)
        {
            stock = -1;
        }

        if (qtyMul > stock && MaxStock != -1 || Price == -1 || Price == 0)
        {
            if (stock == -1)
            {
                StrPrice = "Ø";
            }
            else
            {
                SetInfoText("#tpm_tradeqty_over_stockqty");
            }
            m_canTrade = false;
        }
        else
        {
            SetInfoText("");
            m_canTrade = true;
        }

        if (Price > m_playerMoney || StrPrice == "Ø")
        {
            SetInfoText("#tpm_cant_be_bought");
            m_canTrade = false;
        }

        m_PreviewPanel.SetBuyPriceText("#tpm_buy  - " + StrPrice);

        if (classname.Contains(GetTraderPlusConfigClient().LicenceKeyWord) && GetBankAccount() && GetBankAccount().Licences.Find(classname) != -1)
        {
            SetInfoText("#tpm_already_bought");
            m_canTrade = false;
        }

        string selltext = "#tpm_sell  - " + TraderPlusHelper.IntToCurrencyString(SellPrice, ",");
        if (SellPrice == -1)
        {
            selltext = "#tpm_sell  - Ø";
        }
        m_PreviewPanel.SetDescriptionText(selltext);

        TraderPlusProduct product = m_PreviewPanel.GetCurrentProduct();
        product.TraderID = m_TraderID;
        product.Category = category;
        product.ClassName = classname;
        product.Multiplier = qtyMul;
        product.Quantity = TradeQty;
        product.MaxStock = MaxStock;
        product.Health = health;
        product.Price = Price;
        product.CollateralMoney = CollateralMoney;
        product.TradMode = TRADEMODE_BUY;
    }

    // --- InventoryListHandler ---

    void InventoryListHandler()
    {
        if (!m_PreviewPanel || !m_InventoryPanel)
        {
            return;
        }

        int selectedCat = m_InventoryPanel.GetSelectedCategory();
        if (selectedCat == TraderPlusInventoryCategory.INSURANCES)
        {
            InsuranceListHandler();
            return;
        }

        m_PreviewPanel.ShowSellPanel();

        ref array<ref TraderPlusArticle> playerItems = m_InventoryPanel.GetPlayerItems();
        int selectedRow = m_InventoryListPosition;
        if (selectedRow == -1 || selectedRow >= playerItems.Count())
        {
            m_PreviewPanel.ResetPreview();
            m_canTrade = false;
            return;
        }

        if (!playerItems[selectedRow])
        {
            return;
        }

        string classname = playerItems[selectedRow].ClassName;
        string category = GetTraderPlusClient().GetProductCategory(classname);
        int health = playerItems[selectedRow].Health;
        int qtyMul = m_PreviewPanel.GetQtyMultiplier();

        m_PreviewPanel.UpdateItemPreview(classname);

        int stock = GetTraderPlusClient().GetStockProductFromStock(category, classname, health);
        int ItemQty, TradeQty, MaxItemQty, MaxStock;
        MaxStock = -1;
        ItemQty = playerItems[selectedRow].Quantity;
        MaxItemQty = TraderPlusHelper.GetMaxItemQuantityClient(classname);

        int Price = GetTraderPlusClient().CalculatePriceForThatItem(qtyMul, TRADEMODE_SELL, category, classname, stock, health, MaxStock, TradeQty);

        if (Price > int.MAX)
        {
            Price = -1;
        }
        string StrPrice = TraderPlusHelper.IntToCurrencyString(Price, ",");
        if (Price == -1 || Price == 0)
        {
            StrPrice = "Ø";
        }
        m_PreviewPanel.SetSellPriceText("#tpm_sell  - " + StrPrice);

        if (Price == -1 || Price == 0 || TradeQty > ItemQty || (MaxStock < stock + qtyMul && MaxStock != -1) || (MaxStock == -1 && MaxItemQty != ItemQty && TradeQty < 0))
        {
            if (Price == -1 || Price == 0)
            {
                SetInfoText("#tpm_cant_trade_item");
            }
            if (TradeQty > ItemQty)
            {
                SetInfoText("#tpm_tradeqty_over_itemqty");
            }
            if (MaxStock < stock + qtyMul && MaxStock != -1)
            {
                SetInfoText("#tpm_tradeqty_over_stockqty");
            }
            m_canTrade = false;
        }
        else
        {
            SetInfoText("");
            m_canTrade = true;
        }

        m_PreviewPanel.SetDescriptionText("#tpm_totalqty " + ItemQty);

        TraderPlusProduct product = m_PreviewPanel.GetCurrentProduct();
        product.TraderID = m_TraderID;
        product.Category = category;
        product.ClassName = classname;
        product.Multiplier = qtyMul;
        product.Quantity = TradeQty;
        product.Health = health;
        product.Price = Price;
        product.MaxStock = MaxStock;
        product.TradMode = TRADEMODE_SELL;
        if (selectedCat == TraderPlusInventoryCategory.VEHICLES)
        {
            product.TradMode = TRADEMODE_SELLVEHICLE;
            product.Quantity = selectedRow;
        }
    }

    void InsuranceListHandler()
    {
        if (!m_PreviewPanel)
        {
            return;
        }

        m_PreviewPanel.ShowSellPanel();
        m_PreviewPanel.SetInsuranceVisible(false);

        int selectedRow = m_InventoryListPosition;
        if (selectedRow == -1 || !GetBankAccount() || selectedRow >= GetBankAccount().Insurances.Count())
        {
            m_PreviewPanel.ResetPreview();
            m_canTrade = false;
            return;
        }

        string classname = TraderPlusHelper.GetVehicleTypeInsuranceData(GetBankAccount().Insurances.GetElement(selectedRow));
        int price = TraderPlusHelper.GetCollateralPriceInsuranceData(GetBankAccount().Insurances.GetElement(selectedRow));

        if (GetTraderPlusClient().m_InsuranceStatus[selectedRow])
        {
            price = 0;
        }

        m_PreviewPanel.UpdateItemPreview(classname);

        if (price == -1 || price == 0)
        {
            SetInfoText("#tpm_cant_trade_item");
            m_canTrade = false;
        }
        else if (GetTraderPlusConfigClient().AuthorizedIDInsurance.Find(m_TraderID) == -1)
        {
            SetInfoText("#tpm_cant_claim_insurance");
            m_canTrade = false;
        }
        else
        {
            SetInfoText("");
            m_canTrade = true;
        }

        m_PreviewPanel.SetSellPriceText("#tpm_claim_insurance_money");
        m_PreviewPanel.SetBuyPriceText("Ø");

        TraderPlusProduct product = m_PreviewPanel.GetCurrentProduct();
        product.TraderID = m_TraderID;
        product.Category = "INSURANCE";
        product.ClassName = classname;
        product.Multiplier = 1;
        product.Quantity = 0;
        product.Health = TraderPlusItemState.PRISTINE;
        product.Price = price;
        product.CollateralMoney = GetBankAccount().Insurances.GetKey(selectedRow);
        product.MaxStock = -1;
        product.TradMode = TRADEMODE_SELLINSURANCE;
    }

    // --- Buy/Sell ---

    void BuyHandler()
    {
        if (!m_PreviewPanel)
        {
            return;
        }
        if (!m_canTradeRequest)
        {
            SetInfoText("#tpm_not_too_fast");
            return;
        }
        TraderPlusProduct currentProduct = m_PreviewPanel.GetCurrentProduct();
        if (currentProduct.Price == 0 || currentProduct.Price == -1)
        {
            SetInfoText("#tpm_cant_trade_item");
            return;
        }
        if (m_canTrade)
        {
            GetRPCManager().SendRPC("TraderPlus", "GetTradeRequest", new Param1<ref TraderPlusProduct>(currentProduct), true, NULL);
            m_RequestTrade = true;
            m_canTrade = false;
            m_canTradeRequest = false;
        }
        else if (m_playerMoney < currentProduct.Price)
        {
            SetInfoText("#tpm_dont_enough_money");
        }
        else
        {
            SetInfoText("#tpm_not_enough_qty_stock");
        }
    }

    void SellHandler()
    {
        if (!m_PreviewPanel || !m_InventoryPanel)
        {
            return;
        }
        if (m_InventoryListPosition == -1)
        {
            return;
        }
        if (!m_canTradeRequest)
        {
            SetInfoText("#tpm_not_too_fast");
            return;
        }
        TraderPlusProduct currentProduct = m_PreviewPanel.GetCurrentProduct();
        if (currentProduct.Price == 0)
        {
            SetInfoText("#tpm_cant_trade_item");
            return;
        }
        if (m_canTrade)
        {
            int selectedCat = m_InventoryPanel.GetSelectedCategory();
            ref array<ref TraderPlusArticle> playerItems = m_InventoryPanel.GetPlayerItems();
            if (selectedCat != TraderPlusInventoryCategory.INSURANCES && playerItems[m_InventoryListPosition] && !playerItems[m_InventoryListPosition].HasAttachments)
            {
                SetInfoText("#tpm_empty_item_first");
                return;
            }
            GetRPCManager().SendRPC("TraderPlus", "GetTradeRequest", new Param1<ref TraderPlusProduct>(currentProduct), true, NULL);
            m_RequestTrade = true;
            m_canTrade = false;
            m_canTradeRequest = false;
        }
        else
        {
            SetInfoText("#tpm_cant_trade_item");
        }
    }

    // --- Quantity changed handler ---

    void OnQuantityChanged(int newQty)
    {
        if (!m_PreviewPanel)
        {
            return;
        }

        TraderPlusProduct currentProduct = m_PreviewPanel.GetCurrentProduct();
        if (currentProduct.TradMode == TRADEMODE_BUY)
        {
            StockListHandler();
        }
        else
        {
            InventoryListHandler();
        }
    }

    // --- Preview switch handler ---

    void OnPreviewSwitch()
    {
        if (!m_PreviewPanel)
        {
            return;
        }

        m_PreviewPanel.SwitchPreview();
    }
}
