modded class PlayerBase extends ManBase
{
	ref GarageMenu m_GarageMenu;

	override void Init()
	{
		super.Init();
	}

	void SetupGarageNPC(GarageNPC data)
	{
		SetTraderID(TP_ID_GARAGE);
		SetPosition(data.Position);
		SetOrientation(data.Orientation);
		SetAllowDamage(false);
		DressTrader(data.Clothes);

		//Safezone function so zombies get removed if chase NPC
		SetSafeZoneStatus(true);
	}

	override void SetActions()
	{
		super.SetActions();
		AddAction(ActionGarageMenu);
	}
};
