modded class ActionGetOutTransport: ActionBase
{
	override void OnEndServer( ActionData action_data )
	{
		super.OnEndServer( action_data );
		if(GetSZConfig().IsHideOutActive && action_data.m_Player.GetSafeZoneStatus() != SZ_IN_SAFEZONE)
			action_data.m_Player.DetachHideOutObjectFromPlayer();
	}
};
