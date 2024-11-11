class TraderPlusVehicleParts
{
  string VehicleName;
  int    Height;
  ref TStringArray VehicleParts;

  void TraderPlusVehicleParts(string name)
  {
    VehicleName = name;
    VehicleParts = new TStringArray;
  }

  void AddAttachmentsToVehicle(string partname)
  {
    VehicleParts.Insert(partname);
  }
}

class TraderPlusVehicleInsurance
{
  string VehicleName;
  float  InsurancePriceCoefficient;
  float  CollateralMoneyCoefficient;

  void TraderPlusVehicleInsurance(string name, float insurancePriceCoefficient, float collateralMoneyCoeff)
  {
    VehicleName = name;
    InsurancePriceCoefficient = insurancePriceCoefficient;
    CollateralMoneyCoefficient = collateralMoneyCoeff;
  }
}
