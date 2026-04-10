class StockCategoryCardComponent extends DZlrComponentBase
{
    private static ref ComponentAutoRegister s_Register = new ComponentAutoRegister("stock-category-card-component", StockCategoryCardComponent);

    ref TraderPlusCategoryItems _inputCategoryItems;

    // Bound properties
    string categoryName;
    string productCount;
    ref array<ref TraderPlusArticle> _inputProducts = new array<ref TraderPlusArticle>();

    // Widgets
    GridSpacerWidget ProductsList;

    // State
    private bool m_IsExpanded;

    override protected void RegisterMetadata()
    {
        m_Metadata = new ComponentMetadata();
        m_Metadata.selector = "stock-category-card-component";
        m_Metadata.templatePath = "TraderPlus/datasets/layouts/StockCategoryCardComponent.layout";
    }

    override void OnInit()
    {
        m_IsExpanded = false;

        if (!_inputCategoryItems)
        {
            return;
        }

        categoryName = _inputCategoryItems.CategoryName;
        productCount = _inputCategoryItems.Products.Count().ToString();

        MarkForCheck();
    }

    void OnToggleProducts(ClickEventArgs args)
    {
        if (!_inputCategoryItems)
        {
            return;
        }

        m_IsExpanded = !m_IsExpanded;

        if (m_IsExpanded)
        {
            // Populate the products array for ForEach binding
            _inputProducts = new array<ref TraderPlusArticle>();
            for (int i = 0; i < _inputCategoryItems.Products.Count(); i++)
            {
                _inputProducts.Insert(_inputCategoryItems.Products[i]);
            }

            if (ProductsList)
            {
                ProductsList.Show(true);
            }
        }
        else
        {
            _inputProducts = new array<ref TraderPlusArticle>();
            if (ProductsList)
            {
                ProductsList.Show(false);
            }
        }

        MarkForCheck();
    }

    void ExpandProducts()
    {
        if (m_IsExpanded)
        {
            return;
        }

        m_IsExpanded = true;
        _inputProducts = new array<ref TraderPlusArticle>();
        for (int i = 0; i < _inputCategoryItems.Products.Count(); i++)
        {
            _inputProducts.Insert(_inputCategoryItems.Products[i]);
        }

        if (ProductsList)
        {
            ProductsList.Show(true);
        }

        MarkForCheck();
    }
}
