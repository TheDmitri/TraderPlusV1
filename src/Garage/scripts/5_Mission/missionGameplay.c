modded class MissionGameplay extends MissionBase
{
	void MissionGameplay()
	{
		/*we create the core function on client in order to have the client side working*/
		m_GarageCore = NULL;
		GetGarageCore();
	}

	override void OnKeyRelease(int key)
	{
		super.OnKeyRelease(key);

		if (key == KeyCode.KC_ESCAPE)
		{
			if (GetTraderPlusUIService() && GetTraderPlusUIService().IsMenuOpen())
			{
				GetTraderPlusUIService().CloseView();
			}
		}
	}
};
