class InventoryProductCardComponent extends DZlrComponentBase
{
    private static ref ComponentAutoRegister s_Register = new ComponentAutoRegister("inventory-product-card-component", InventoryProductCardComponent);

    static ref ScriptInvoker Event_OnInventoryProductSelected = new ScriptInvoker();
    static ref ScriptInvoker Event_OnInventoryProductDoubleClicked = new ScriptInvoker();

    ref TraderPlusArticle _inputArticle;

    // Bound properties
    string displayName;
    int priceValue;
    string countText;
    EntityAI preview;

    // Widgets loaded via AutoLoadWidgets
    Widget Highlight;
    ImageWidget Health;

    // Internal
    private EntityAI m_localEntity;
    private int m_Index;

    override protected void RegisterMetadata()
    {
        m_Metadata = new ComponentMetadata();
        m_Metadata.selector = "inventory-product-card-component";
        m_Metadata.templatePath = "TraderPlus/datasets/layouts/InventoryProductCardComponent.layout";
    }

    void SetIndex(int index)
    {
        m_Index = index;
    }

    override void OnInit()
    {
        if (!_inputArticle)
        {
            return;
        }

        string classname = _inputArticle.ClassName;
        int health = _inputArticle.Health;
        string category = GetTraderPlusClient().GetProductCategory(classname);
        int stock = GetTraderPlusClient().GetStockProductFromStock(category, classname, health);
        int maxstock;
        int tradeqty;
        int price = GetTraderPlusClient().CalculatePriceForThatItem(1, TRADEMODE_SELL, category, classname, stock, health, maxstock, tradeqty);

        // Display name
        displayName = TraderPlusHelper.GetDisplayName(classname);

        // Price
        priceValue = price;

        // Count
        countText = _inputArticle.Count.ToString();

        // Health color
        int cA, cR, cG, cB;
        TraderPlusHelper.GetHealthFromLevel(health, cA, cR, cG, cB);
        if (Health)
        {
            Health.SetColor(ARGB(cA, cR, cG, cB));
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

            // Check for ammo count
            ItemBase temp = ItemBase.Cast(m_localEntity);
            if (temp)
            {
                if (temp.IsInherited(Ammunition_Base) && !classname.Contains("Mag"))
                {
                    countText = _inputArticle.Quantity.ToString();
                }
            }
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
        if (m_localEntity)
        {
            m_localEntity.Delete();
            m_localEntity = null;
        }
        OnInit();
    }

    void OnInventoryProductClicked(ClickEventArgs args)
    {
        if (_inputArticle)
        {
            Event_OnInventoryProductSelected.Invoke(_inputArticle, m_Index);
        }
    }
}
