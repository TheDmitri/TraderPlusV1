class TraderStockPanelComponent extends DZlrComponentBase
{
    private static ref ComponentAutoRegister s_Register = new ComponentAutoRegister("trader-stock-panel-component", TraderStockPanelComponent);

    // Bound properties for ForEach
    ref array<ref TraderPlusCategoryItems> _inputStockItems = new array<ref TraderPlusCategoryItems>();

    // Widgets loaded via AutoLoadWidgets
    XComboBoxWidget StockCategories;
    EditBoxWidget SearchStockBox;
    CheckBoxWidget ShowAllCheckBox;

    // Internal state
    private int m_TraderID;
    private int m_selectedCatStock;
    private bool m_ShowAll;
    private string m_SearchStock;
    private ref array<ref TraderPlusCategoryItems> m_StockItems;

    override protected void RegisterMetadata()
    {
        m_Metadata = new ComponentMetadata();
        m_Metadata.selector = "trader-stock-panel-component";
        m_Metadata.templatePath = "TraderPlus/datasets/layouts/TraderStockPanelComponent.layout";
    }

    void SetTraderID(int traderID)
    {
        m_TraderID = traderID;
    }

    override void OnInit()
    {
        m_selectedCatStock = 0;
        m_ShowAll = false;
        m_SearchStock = "";
        m_StockItems = new array<ref TraderPlusCategoryItems>();

        if (ShowAllCheckBox)
        {
            ShowAllCheckBox.Show(GetTraderPlusConfigClient().EnableShowAllCheckBox);
        }

        InitStockCategories();
        StockCategoryHandler();
    }

    void InitStockCategories()
    {
        if (!StockCategories)
        {
            return;
        }
        StockCategories.ClearAll();
        int categoryCount = GetTraderPlusClient().GetCategoriesFromID(m_TraderID);
        for (int i = 0; i < categoryCount; i++)
        {
            StockCategories.AddItem(GetTraderPlusClient().m_StockCategories[i]);
        }
        StockCategories.SetCurrentItem(m_selectedCatStock);
    }

    void StockCategoryHandler()
    {
        if (!StockCategories)
        {
            return;
        }
        m_selectedCatStock = StockCategories.GetCurrentItem();
        m_StockItems.Clear();
        GetTraderPlusClient().GetItemInStockFromCategory(m_selectedCatStock, m_StockItems, m_ShowAll, m_SearchStock);
        _inputStockItems = m_StockItems;
        MarkForCheck();
    }

    void RefreshStock()
    {
        m_StockItems.Clear();
        InitStockCategories();
        StockCategoryHandler();
    }

    void SearchUpdate()
    {
        if (!SearchStockBox)
        {
            return;
        }
        string searchStock = SearchStockBox.GetText();
        if (searchStock != m_SearchStock)
        {
            m_SearchStock = searchStock;
            StockCategoryHandler();
        }
    }

    override bool OnClick(Widget w, int x, int y, int button)
    {
        if (w == ShowAllCheckBox)
        {
            if (ShowAllCheckBox.IsChecked())
            {
                m_ShowAll = true;
            }
            else
            {
                m_ShowAll = false;
            }
            InitStockCategories();
            StockCategoryHandler();
            return true;
        }

        return false;
    }

    override bool OnChange(Widget w, int x, int y, bool finished)
    {
        if (w == StockCategories)
        {
            StockCategoryHandler();
            return true;
        }

        return false;
    }
}
