class TraderNavComponent extends DZlrComponentBase
{
    private static ref ComponentAutoRegister s_Register = new ComponentAutoRegister("trader-nav-component", TraderNavComponent);

    static ref ScriptInvoker Event_OnTabSelected = new ScriptInvoker();

    // Widgets loaded via AutoLoadWidgets
    ButtonWidget BttnCar;
    ButtonWidget BttnLicence;
    ButtonWidget BttnInsurance;

    override protected void RegisterMetadata()
    {
        m_Metadata = new ComponentMetadata();
        m_Metadata.selector = "trader-nav-component";
        m_Metadata.templatePath = "TraderPlus/datasets/layouts/TraderNavComponent.layout";
    }

    override void OnInit()
    {
        if (BttnCar)
        {
            BttnCar.Show(!GetTraderPlusConfigClient().HideGarageBtn);
        }
        if (BttnLicence)
        {
            BttnLicence.Show(!GetTraderPlusConfigClient().HideLicenceBtn);
        }
        if (BttnInsurance)
        {
            BttnInsurance.Show(!GetTraderPlusConfigClient().HideInsuranceBtn);
        }
    }

    void OnMarketClicked(ClickEventArgs args)
    {
        Event_OnTabSelected.Invoke(0, TRADEMODE_BUY);
    }

    void OnInventoryClicked(ClickEventArgs args)
    {
        Event_OnTabSelected.Invoke(TraderPlusInventoryCategory.SELLABLE, TRADEMODE_SELL);
    }

    void OnCarClicked(ClickEventArgs args)
    {
        Event_OnTabSelected.Invoke(TraderPlusInventoryCategory.VEHICLES, TRADEMODE_SELL);
    }

    void OnLicenceClicked(ClickEventArgs args)
    {
        Event_OnTabSelected.Invoke(TraderPlusInventoryCategory.LICENCES, TRADEMODE_SELL);
    }

    void OnInsuranceClicked(ClickEventArgs args)
    {
        Event_OnTabSelected.Invoke(TraderPlusInventoryCategory.INSURANCES, TRADEMODE_SELL);
    }
}
