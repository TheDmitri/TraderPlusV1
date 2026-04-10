class GarageAppComponent extends DZlrComponentBase
{
    private static ref ComponentAutoRegister s_Register = new ComponentAutoRegister("garage-app-component", GarageAppComponent);

    // Bound properties
    string costParkInText;
    string costParkOutText;
    string moneyText;
    EntityAI vehiclePreview;
    ref array<ref GarageVehicleData> _inputVehicles = new array<ref GarageVehicleData>();

    // Widgets loaded via AutoLoadWidgets
    ImageWidget ParkingStatut;

    // Internal state
    bool m_IsFlag;
    vector m_Position;
    vector m_ParkingPos;
    int m_LowUID;
    bool m_PayWithBankAccount;

    private ref TStringArray m_VehiclesName;
    private ref TStringArray m_VehiclesListName;
    private int m_MoneyAmountOnPlayer;
    private int m_ParkInFee;
    private int m_ParkOutFee;
    private ref CarScript m_CarInPark;
    private int m_SelectedVehicle;
    private EntityAI m_previewItem;
    private ref TransactionThrottle m_Throttle;

    override protected void RegisterMetadata()
    {
        m_Metadata = new ComponentMetadata();
        m_Metadata.selector = "garage-app-component";
        m_Metadata.templatePath = "Garage/datasets/layouts/GarageAppComponent.layout";
    }

    override void OnInit()
    {
        m_MoneyAmountOnPlayer = 0;
        m_CarInPark = NULL;
        m_VehiclesName = new TStringArray;
        m_VehiclesListName = new TStringArray;
        m_SelectedVehicle = -1;
        m_Throttle = new TransactionThrottle(TRADERPLUS_BANK_TRANSACTION_INTERVAL);

        GarageVehicleCardComponent.Event_OnVehicleSelected.Insert(OnVehicleSelected);

        GetGame().GetCallQueue(CALL_CATEGORY_GUI).CallLater(PeriodicUpdate, 500, true);
    }

    override void OnDestroy()
    {
        GarageVehicleCardComponent.Event_OnVehicleSelected.Remove(OnVehicleSelected);
        GetGame().GetCallQueue(CALL_CATEGORY_GUI).Remove(PeriodicUpdate);
        CleanupPreview();
    }

    void CleanupPreview()
    {
        if (m_previewItem)
        {
            GetGame().ObjectDelete(m_previewItem);
            m_previewItem = null;
        }
    }

    void SetGarageData(int lowUID, vector position, bool isFlag)
    {
        m_LowUID = lowUID;
        m_Position = position;
        m_IsFlag = isFlag;
    }

    void SetResponseData(TStringArray vehicles, vector parkpos)
    {
        m_VehiclesName = new TStringArray;
        if (vehicles)
        {
            for (int i = 0; i < vehicles.Count(); i++)
            {
                m_VehiclesName.Insert(vehicles[i]);
            }
        }
        m_ParkingPos = parkpos;
        m_PayWithBankAccount = GetGarageConfig().PayWithBankAccount;
        if (m_PayWithBankAccount)
        {
            GetRPCManager().SendRPC("TraderPlusBanking", "TraderPlusBankingRequest", NULL, true, NULL);
        }
    }

    void PeriodicUpdate()
    {
        m_Throttle.Update(0.5);
    }

    void UIHandle()
    {
        m_SelectedVehicle = -1;
        m_VehiclesListName = new TStringArray;
        _inputVehicles = new array<ref GarageVehicleData>();

        m_CarInPark = GarageHelpers.CheckForCarAtPos(m_ParkingPos);
        if (m_CarInPark)
        {
            GarageVehicleData parkedData = new GarageVehicleData(m_CarInPark.GetType(), true, 0);
            _inputVehicles.Insert(parkedData);
            m_VehiclesListName.Insert(m_CarInPark.GetType());
        }

        for (int i = 0; i < m_VehiclesName.Count(); i++)
        {
            int idx = i;
            if (m_CarInPark)
            {
                idx = idx + 1;
            }
            GarageVehicleData storedData = new GarageVehicleData(m_VehiclesName[i], false, idx);
            _inputVehicles.Insert(storedData);
            m_VehiclesListName.Insert(m_VehiclesName[i]);
        }

        MoneyUpdate();

        m_ParkInFee = GetGarageConfig().ParkInCost;
        m_ParkOutFee = GetGarageConfig().ParkOutCost;
        costParkInText = "#gm_cost_park_in " + m_ParkInFee.ToString();
        costParkOutText = "#gm_cost_park_out " + m_ParkOutFee.ToString();

        if (ParkingStatut)
        {
            if (m_ParkingPos == Vector(-1, -1, -1) || m_ParkingPos == Vector(0, 0, 0) || m_CarInPark)
            {
                ParkingStatut.SetColor(ARGB(255, 191, 48, 48));
            }
            else
            {
                ParkingStatut.SetColor(ARGB(255, 0, 255, 0));
            }
        }

        if (m_VehiclesListName.Count() > 0)
        {
            m_SelectedVehicle = 0;
            UpdateVehiclePreview(m_VehiclesListName[m_SelectedVehicle]);
        }

        MarkForCheck();
    }

    void MoneyUpdate()
    {
        bool hasBankData = m_PayWithBankAccount && GetBankAccount() != null;
        m_MoneyAmountOnPlayer = TernaryInt(hasBankData, GetBankAccount().MoneyAmount, TraderPlusMoneyHelper.GetPlayerMoneyFromInventory());
        moneyText = "#tpm_money " + TraderPlusHelper.IntToCurrencyString(m_MoneyAmountOnPlayer, ",");
    }

    void OnVehicleSelected(GarageVehicleData data)
    {
        if (!data)
        {
            return;
        }
        m_SelectedVehicle = data.index;
        UpdateVehiclePreview(data.vehicleName);
    }

    void UpdateVehiclePreview(string itemType)
    {
        CleanupPreview();

        m_previewItem = EntityAI.Cast(GetGame().CreateObjectEx(itemType, vector.Zero, ECE_LOCAL | ECE_NOLIFETIME));
        if (!m_previewItem)
        {
            return;
        }

        vehiclePreview = m_previewItem;
        MarkForCheck();
    }

    void OnParkInClicked(ClickEventArgs args)
    {
        if (!m_CarInPark)
        {
            return;
        }
        if (GetGarageConfig().VehicleMustHaveLock)
        {
            if (!GarageHelpers.CanParkVehicle(m_CarInPark))
            {
                return;
            }
        }
        if (m_VehiclesName.Count() + 1 > GetGarageConfig().MaxVehicleStored)
        {
            NotificationSystem.AddNotificationExtended(2, "Garage", GetGarageConfig().MaxVehicleStoredReached, "Garage/image/CarLogo.paa");
            return;
        }
        if (m_MoneyAmountOnPlayer < m_ParkInFee)
        {
            NotificationSystem.AddNotificationExtended(2, "Garage", GetGarageConfig().NotEnoughMoney, "Garage/image/CarLogo.paa");
            return;
        }
        if (!m_Throttle.CanRequest())
        {
            return;
        }
        GetRPCManager().SendRPC("Garage", "ParkInRequest", new Param3<int, CarScript, vector>(m_LowUID, m_CarInPark, m_ParkingPos), true, NULL);
        m_Throttle.OnRequestSent();
    }

    void OnParkOutClicked(ClickEventArgs args)
    {
        if (m_CarInPark)
        {
            NotificationSystem.AddNotificationExtended(2, "Garage", GetGarageConfig().ParkingNotAvailable, "Garage/image/CarLogo.paa");
            return;
        }
        if (m_MoneyAmountOnPlayer < m_ParkInFee)
        {
            NotificationSystem.AddNotificationExtended(2, "Garage", GetGarageConfig().NotEnoughMoney, "Garage/image/CarLogo.paa");
            return;
        }
        if (!m_Throttle.CanRequest())
        {
            return;
        }
        if (m_SelectedVehicle < 0 || m_SelectedVehicle >= m_VehiclesName.Count())
        {
            return;
        }
        GetRPCManager().SendRPC("Garage", "ParkOutRequest", new Param4<int, int, string, vector>(m_LowUID, m_SelectedVehicle, m_VehiclesName[m_SelectedVehicle], m_ParkingPos), true, NULL);
        m_Throttle.OnRequestSent();
    }
}
