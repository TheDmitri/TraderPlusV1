modded class MissionGameplay extends MissionBase
{
	void MissionGameplay()
	{
		/*we create the core function on client in order to have the client side working*/
		m_TraderPlusCore = NULL;
		GetTraderPlusCore();
	}

	override void OnKeyRelease(int key)
	{
		super.OnKeyRelease(key);
		PlayerBase player = PlayerBase.Cast(GetGame().GetPlayer());

		if ( key == KeyCode.KC_ESCAPE )
				if (player && player.m_TraderPlusMenu)
		  		player.m_TraderPlusMenu.OnHide();
	}
};
