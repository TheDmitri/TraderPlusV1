modded class CarScript extends Car
{
    float GarageGetFuelAmmount()
    {
        return m_FuelAmmount;
    }

    void DifferOrientation(vector orientation)
    {
      GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(SetOrientation, 500, false,  orientation);
    }
}
