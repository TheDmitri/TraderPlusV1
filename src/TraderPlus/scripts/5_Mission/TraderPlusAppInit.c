class TraderPlusAppInit
{
    void TraderPlusAppInit()
    {
        AppModule.GetOnRegisterServices().Insert(RegisterServices);
    }

    static void RegisterServices()
    {
        // Register custom pipes
        PipeRegistry.RegisterPipe("tradercurrency", new TraderCurrencyPipe());

        // Register services
        auto registry = DZlrServiceRegistry.GetInstance();
        registry.RegisterService(TraderPlusUIService, new TraderPlusUIService());
    }
}

ref TraderPlusAppInit g_TraderPlusAppInit = new TraderPlusAppInit();
