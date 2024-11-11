/*To make things easier for me, I use some static class that can be run from anywhere in my code,
it's very efficient to get config data or access my main class that run my code.
However it can create conflict with other mods if someone use to same name, so if you create some static function, make sure
to make the name specific as much as you can. I usually add Name of my mod in front of my function*/

/*Class that deals with my core class, where config, client and server part of my code are created*/
static ref TraderPlusCore m_TraderPlusCore;
static TraderPlusCore GetTraderPlusCore() {
	if ( !m_TraderPlusCore ) {
		m_TraderPlusCore = new TraderPlusCore;
	}
	return m_TraderPlusCore;
}

/*class that allows to access config on server side everywhere*/
static ref TraderPlusGeneralSettings GetTraderPlusConfigServer()	{
	return GetTraderPlusCore().m_TraderPlusGeneralSettings;
}

/*class that allows to access config on server side everywhere*/
static ref TraderPlusVehiclesSettings GetTraderPlusVehiculeConfig()	{
	return GetTraderPlusCore().m_TraderPlusVehiclesSettings;
}

static TraderPlusPriceSettings GetTraderPlusPriceConfig()	{
	return GetTraderPlusCore().m_TraderPlusPriceSettings;
}

/*class that allows to access config on client side everywhere*/
static ref TraderPlusGnrlConfigClient GetTraderPlusConfigClient()	{
	return GetTraderPlusCore().m_TraderPlusGnrlConfigClient;
}

/*class that allows to access config on server side everywhere*/
static ref TraderPlusIDsSettings GetTraderPlusIDConfig()	{
	return GetTraderPlusCore().m_TraderPlusIDsSettings;
}

static ref TraderPlusClient GetTraderPlusClient()	{
	return GetTraderPlusCore().m_TraderPlusClient;
}

static ref TraderPlusServer GetTraderPlusServer()	{
	return GetTraderPlusCore().m_TraderPlusServer;
}

/*Function that allows to get the player variable from its identity => used for RPC*/
static PlayerBase TraderPlusGetPlayerByIdentity(PlayerIdentity sender)
{
	int	low	 =	0;
	int	high =	0;
	GetGame().GetPlayerNetworkIDByIdentityID( sender.GetPlayerId(), low, high );
	return PlayerBase.Cast( GetGame().GetObjectByNetworkId(low, high ));
}
