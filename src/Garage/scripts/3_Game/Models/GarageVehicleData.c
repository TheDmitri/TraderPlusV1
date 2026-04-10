class GarageVehicleData
{
    string vehicleName;
    bool isParkedAtSpot;
    int index;

    void GarageVehicleData(string name, bool parked, int idx)
    {
        vehicleName = name;
        isParkedAtSpot = parked;
        index = idx;
    }
}
