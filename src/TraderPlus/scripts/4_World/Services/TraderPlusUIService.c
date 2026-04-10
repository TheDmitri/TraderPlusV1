static ref TraderPlusUIService g_TraderPlusUIService;

TraderPlusUIService GetTraderPlusUIService()
{
    if (!g_TraderPlusUIService)
    {
        g_TraderPlusUIService = TraderPlusUIService.Cast(DZlrServiceRegistry.GetInstance().GetService(TraderPlusUIService));
    }
    return g_TraderPlusUIService;
}

class TraderPlusUIService : DayZularService
{
    private ref ComponentMenu m_Menu;
    private ref TraderAppComponent m_TraderApp;
    private ref BankingAppComponent m_BankingApp;
    private ref GarageAppComponent m_GarageApp;
    private int m_ActiveView; // 0=none, 1=trader, 2=banking, 3=garage

    void TraderPlusUIService()
    {
        g_TraderPlusUIService = this;
        m_ActiveView = 0;

        if (!GetGame().IsClient())
        {
            return;
        }

        GetDayZularEvents().OnMissionStart.Insert(Initialize);
        GetDayZularEvents().OnMissionFinish.Insert(Shutdown);
    }

    void Initialize()
    {
        DZLog().LogInfo("TraderPlusUIService initialized");
    }

    void Shutdown()
    {
        CloseView();
        DZLog().LogInfo("TraderPlusUIService shutdown");
    }

    // --- Menu lifecycle ---

    bool IsMenuOpen()
    {
        if (m_Menu)
        {
            return m_Menu.IsVisible();
        }
        return false;
    }

    void CloseView()
    {
        if (m_Menu)
        {
            m_Menu.HideMenu();
            delete m_Menu;
            m_Menu = null;
        }

        if (m_TraderApp)
        {
            m_TraderApp.CleanupPreview();
            delete m_TraderApp;
            m_TraderApp = null;
        }

        if (m_BankingApp)
        {
            delete m_BankingApp;
            m_BankingApp = null;
        }

        if (m_GarageApp)
        {
            m_GarageApp.CleanupPreview();
            delete m_GarageApp;
            m_GarageApp = null;
        }

        m_ActiveView = 0;
    }

    // --- Trader View ---

    void OpenTraderView(int traderID, vector traderPos, Object traderObj)
    {
        if (m_Menu && m_Menu.IsVisible())
        {
            return;
        }

        CloseView();

        m_Menu = new ComponentMenu();
        m_Menu.SetUseMouse(true);
        m_Menu.SetUseKeyboard(false);

        m_TraderApp = new TraderAppComponent();
        m_TraderApp.SetTraderData(traderID, traderPos, traderObj);
        m_TraderApp.InitComponent(null);

        m_Menu.SetRootComponent(m_TraderApp);
        m_Menu.ShowMenu();
        m_ActiveView = 1;
    }

    void OnStockResponse(int response, TraderPlusStock stock, string category)
    {
        if (m_TraderApp)
        {
            m_TraderApp.OnStockDataReceived(response, stock, category);
        }
    }

    void OnTradeResponse(int response)
    {
        if (m_TraderApp)
        {
            m_TraderApp.GetTradingResponse(response);
        }
    }

    TraderAppComponent GetTraderApp()
    {
        return m_TraderApp;
    }

    // --- Banking View ---

    void OpenBankingView()
    {
        if (m_Menu && m_Menu.IsVisible())
        {
            return;
        }

        CloseView();

        m_Menu = new ComponentMenu();
        m_Menu.SetUseMouse(true);
        m_Menu.SetUseKeyboard(false);

        m_BankingApp = new BankingAppComponent();
        m_BankingApp.InitComponent(null);

        m_Menu.SetRootComponent(m_BankingApp);
        m_Menu.ShowMenu();
        m_ActiveView = 2;
    }

    void OnBankingResponse()
    {
        if (m_BankingApp)
        {
            m_BankingApp.UIHandle();
        }
    }

    BankingAppComponent GetBankingApp()
    {
        return m_BankingApp;
    }

    // --- Garage View ---

    void OpenGarageView(int lowUID, vector position, bool isFlag)
    {
        if (m_Menu && m_Menu.IsVisible())
        {
            return;
        }

        CloseView();

        m_Menu = new ComponentMenu();
        m_Menu.SetUseMouse(true);
        m_Menu.SetUseKeyboard(false);

        m_GarageApp = new GarageAppComponent();
        m_GarageApp.SetGarageData(lowUID, position, isFlag);
        m_GarageApp.InitComponent(null);

        m_Menu.SetRootComponent(m_GarageApp);
        m_Menu.ShowMenu();
        m_ActiveView = 3;
    }

    void OnGarageResponse(TStringArray vehicles, vector parkpos)
    {
        if (m_GarageApp)
        {
            m_GarageApp.SetResponseData(vehicles, parkpos);
            m_GarageApp.UIHandle();
        }
    }

    GarageAppComponent GetGarageApp()
    {
        return m_GarageApp;
    }
}
