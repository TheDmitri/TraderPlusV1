#ifndef CARLOCKDISABLE
modded class MissionServer extends MissionBase
{
	void MissionServer()
	{

		m_CarLockCore = NULL;
		GetCarLockCore();

	}

	void ~MissionServer() {
	}

	override void InvokeOnConnect(PlayerBase player, PlayerIdentity identity)
	{
		super.InvokeOnConnect(player,identity);
		player.SetLowSteamUID(player.GetIdentity().GetPlainId());
	}
};
#endif
