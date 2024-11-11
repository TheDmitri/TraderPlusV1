modded class BuildingBase
{
	override void SetActions()
	{
		super.SetActions();
	}

	void SetupGarageNPC(GarageNPC data)
	{
		SetTraderID(TP_ID_GARAGE);
		SetPosition(data.Position);
		SetOrientation(data.Orientation);
		SetAllowDamage(false);
	}
}
