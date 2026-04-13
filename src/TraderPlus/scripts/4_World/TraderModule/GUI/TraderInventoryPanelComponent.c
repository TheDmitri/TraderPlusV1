class TraderInventoryPanelComponent extends DZlrComponentBase
{
    private static ref ComponentAutoRegister s_Register = new ComponentAutoRegister("trader-inventory-panel-component", TraderInventoryPanelComponent);

    static ref ScriptInvoker Event_OnInventoryCategoryChanged = new ScriptInvoker();

    // Bound properties for ForEach — regular items
    ref array<ref TraderPlusArticle> _inputPlayerItems = new array<ref TraderPlusArticle>();

    // Bound properties for ForEach — insurance items (separate ForEach in layout)
    ref array<ref TraderPlusInsuranceData> _inputInsurances = new array<ref TraderPlusInsuranceData>();

    // Widgets loaded via AutoLoadWidgets
    XComboBoxWidget InventoryCategories;
    EditBoxWidget SearchInventoryBox;
    Widget InsuranceListContainer;
    Widget InventoryListContainer;

    // Internal state
    private int m_selectedCatInventory;
    private string m_SearchInventory;
    private ref array<ref TraderPlusArticle> m_PlayerItems;

    override protected void RegisterMetadata()
    {
        m_Metadata = new ComponentMetadata();
        m_Metadata.selector = "trader-inventory-panel-component";
        m_Metadata.templatePath = "TraderPlus/datasets/layouts/TraderInventoryPanelComponent.layout";
    }

    override void OnInit()
    {
        m_selectedCatInventory = 0;
        m_SearchInventory = "";
        m_PlayerItems = new array<ref TraderPlusArticle>();

        InitPlayerCategories();
        PlayerCategoryHandler();
    }

    int GetSelectedCategory()
    {
        return m_selectedCatInventory;
    }

    ref array<ref TraderPlusArticle> GetPlayerItems()
    {
        return m_PlayerItems;
    }

    void InitPlayerCategories()
    {
        if (!InventoryCategories)
        {
            return;
        }
        InventoryCategories.ClearAll();
        InventoryCategories.AddItem("#tpm_sellable");
        InventoryCategories.AddItem("#tpm_all");
        InventoryCategories.AddItem("#tpm_licences");
        InventoryCategories.AddItem("#tpm_vehicles");
        InventoryCategories.AddItem("#tpm_insurances");
        InventoryCategories.SetCurrentItem(m_selectedCatInventory);
    }

    void PlayerCategoryHandler()
    {
        if (!InventoryCategories)
        {
            return;
        }
        m_selectedCatInventory = InventoryCategories.GetCurrentItem();
        m_PlayerItems.Clear();

        if (m_selectedCatInventory == TraderPlusInventoryCategory.INSURANCES)
        {
            // Clear regular items, populate insurance items
            _inputPlayerItems = new array<ref TraderPlusArticle>();
            BuildInsuranceList();

            // Toggle containers
            if (InventoryListContainer)
            {
                InventoryListContainer.Show(false);
            }
            if (InsuranceListContainer)
            {
                InsuranceListContainer.Show(true);
            }

            MarkForCheck();
            Event_OnInventoryCategoryChanged.Invoke();
            return;
        }

        // Regular inventory
        _inputInsurances = new array<ref TraderPlusInsuranceData>();
        if (InsuranceListContainer)
        {
            InsuranceListContainer.Show(false);
        }
        if (InventoryListContainer)
        {
            InventoryListContainer.Show(true);
        }

        GetTraderPlusClient().GetPlayerItemsFromCategory(m_selectedCatInventory, m_PlayerItems, m_SearchInventory);
        _inputPlayerItems = m_PlayerItems;
        MarkForCheck();

        Event_OnInventoryCategoryChanged.Invoke();
    }

    void BuildInsuranceList()
    {
        _inputInsurances = new array<ref TraderPlusInsuranceData>();

        if (!GetBankAccount() || !GetBankAccount().Insurances)
        {
            return;
        }

        int insuranceCount = GetBankAccount().Insurances.Count();
        for (int i = 0; i < insuranceCount; i++)
        {
            string classname = TraderPlusHelper.GetVehicleTypeInsuranceData(GetBankAccount().Insurances.GetElement(i));
            int price = TraderPlusHelper.GetCollateralPriceInsuranceData(GetBankAccount().Insurances.GetElement(i));
            bool isAssured = false;
            if (GetTraderPlusClient().m_InsuranceStatus && i < GetTraderPlusClient().m_InsuranceStatus.Count())
            {
                isAssured = GetTraderPlusClient().m_InsuranceStatus[i];
            }

            TraderPlusInsuranceData data = new TraderPlusInsuranceData(classname, price, isAssured, i);
            _inputInsurances.Insert(data);
        }
    }

    void SetCategory(int category)
    {
        m_selectedCatInventory = category;
        if (InventoryCategories)
        {
            InventoryCategories.SetCurrentItem(category);
        }
        PlayerCategoryHandler();
    }

    void RefreshInventory()
    {
        m_PlayerItems.Clear();
        InitPlayerCategories();
        PlayerCategoryHandler();
    }

    void SearchUpdate()
    {
        if (!SearchInventoryBox)
        {
            return;
        }
        string searchInventory = SearchInventoryBox.GetText();
        if (searchInventory != m_SearchInventory)
        {
            m_SearchInventory = searchInventory;
            PlayerCategoryHandler();
        }
    }

    override bool OnChange(Widget w, int x, int y, bool finished)
    {
        if (w == InventoryCategories)
        {
            PlayerCategoryHandler();
            return true;
        }
        return false;
    }
}
