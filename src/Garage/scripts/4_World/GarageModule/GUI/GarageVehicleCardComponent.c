class GarageVehicleCardComponent extends DZlrComponentBase
{
    private static ref ComponentAutoRegister s_Register = new ComponentAutoRegister("garage-vehicle-card-component", GarageVehicleCardComponent);

    static ref ScriptInvoker Event_OnVehicleSelected = new ScriptInvoker();

    ref GarageVehicleData _inputVehicle;

    string vehicleDisplayName;

    ButtonWidget VehicleButton;

    override protected void RegisterMetadata()
    {
        m_Metadata = new ComponentMetadata();
        m_Metadata.selector = "garage-vehicle-card-component";
        m_Metadata.templatePath = "Garage/datasets/layouts/GarageVehicleCardComponent.layout";
    }

    override void OnInit()
    {
        if (_inputVehicle)
        {
            vehicleDisplayName = _inputVehicle.vehicleName;

            if (VehicleButton && _inputVehicle.isParkedAtSpot)
            {
                VehicleButton.SetTextColor(ARGB(255, 191, 48, 48));
            }
        }

        MarkForCheck();
    }

    void OnVehicleClicked(ClickEventArgs args)
    {
        if (_inputVehicle)
        {
            Event_OnVehicleSelected.Invoke(_inputVehicle);
        }
    }
}
