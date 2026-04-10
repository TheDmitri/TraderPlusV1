class StockProductCardComponent extends DZlrComponentBase
{
    private static ref ComponentAutoRegister s_Register = new ComponentAutoRegister("stock-product-card-component", StockProductCardComponent);

    static ref ScriptInvoker Event_OnProductSelected = new ScriptInvoker();
    static ref ScriptInvoker Event_OnProductDoubleClicked = new ScriptInvoker();

    ref TraderPlusArticle _inputArticle;

    // Bound properties
    string displayName;
    int priceValue;
    string stockText;
    string countOnYouText;
    EntityAI preview;

    // Widgets loaded via AutoLoadWidgets
    Widget Highlight;
    ImageWidget Health;
    ImageWidget ImageBag;

    // Internal
    private EntityAI m_localEntity;

    override protected void RegisterMetadata()
    {
        m_Metadata = new ComponentMetadata();
        m_Metadata.selector = "stock-product-card-component";
        m_Metadata.templatePath = "TraderPlus/datasets/layouts/StockProductCardComponent.layout";
    }

    override void OnInit()
    {
        if (!_inputArticle)
        {
            return;
        }

        string classname = _inputArticle.ClassName;
        string category = GetTraderPlusClient().GetProductCategory(classname);
        int quantity = _inputArticle.Quantity;
        int health = _inputArticle.Health;
        int maxstock = _inputArticle.MaxStock;
        int tradeqty;
        int price = GetTraderPlusClient().CalculatePriceForThatItem(1, false, category, classname, quantity, health, maxstock, tradeqty);

        // Display name
        displayName = TraderPlusHelper.GetDisplayName(classname);

        // Price — piped in layout as {{ priceValue | tradercurrency }}
        priceValue = price;

        // Stock text
        bool isUnlimited = quantity == -1 || maxstock == -1;
        stockText = TernaryString(isUnlimited, "∞", quantity.ToString() + "/" + maxstock.ToString());

        // Health color
        int cA, cR, cG, cB;
        TraderPlusHelper.GetHealthFromLevel(health, cA, cR, cG, cB);
        if (Health)
        {
            Health.SetColor(ARGB(cA, cR, cG, cB));
        }

        // Full stock color override
        if (quantity == maxstock && quantity != -1)
        {
            if (Health)
            {
                Health.SetColor(ARGB(255, 191, 48, 48));
            }
        }

        // Count on player
        countOnYouText = "";
        if (ImageBag)
        {
            ImageBag.Show(false);
        }

        // Create preview entity
        string itemType = classname;
        if (itemType.Contains(GetTraderPlusConfigClient().LicenceKeyWord))
        {
            itemType = "Paper";
        }
        m_localEntity = EntityAI.Cast(GetGame().CreateObjectEx(itemType, vector.Zero, ECE_LOCAL | ECE_NOLIFETIME));
        if (m_localEntity)
        {
            preview = m_localEntity;
        }

        MarkForCheck();
    }

    override void OnDestroy()
    {
        if (m_localEntity)
        {
            m_localEntity.Delete();
            m_localEntity = null;
        }
    }

    override void OnChanges()
    {
        // Re-run init logic when _inputArticle is updated by ForEach
        if (m_localEntity)
        {
            m_localEntity.Delete();
            m_localEntity = null;
        }
        OnInit();
    }

    void OnProductClicked(ClickEventArgs args)
    {
        if (_inputArticle)
        {
            Event_OnProductSelected.Invoke(_inputArticle);
        }
    }

    void SetCountOnYou(int count)
    {
        if (count > 0)
        {
            countOnYouText = count.ToString();
            if (ImageBag)
            {
                ImageBag.Show(true);
            }
        }
        MarkForCheck();
    }

    void SetCompatibleHighlight()
    {
        if (Highlight)
        {
            Highlight.Show(true);
            Highlight.SetColor(ARGB(250, 216, 89, 1));
        }
    }
}
