/*To make things easier for me, I use some static class that can be run from anywhere in my code,
it's very efficient to get config data or access my main class that run my code.
However it can create conflict with other mods if someone use to same name, so if you create some static function, make sure
to make the name specific as much as you can. I usually add Name of my mod in front of my function*/


/*Class that deals with my core class, where config, client and server part of my code are created*/
#ifndef CARLOCKDISABLE
static ref CarLockCore m_CarLockCore;
static CarLockCore GetCarLockCore() {
	if ( !m_CarLockCore ) {
		m_CarLockCore = new CarLockCore;
	}
	return m_CarLockCore;
}
#endif

/*Function that allows to get the player variable from its identity => used for RPC*/
static PlayerBase CLGetPlayerByIdentity(PlayerIdentity sender)
{
	int	low	 =	0;
	int	high =	0;
	GetGame().GetPlayerNetworkIDByIdentityID( sender.GetPlayerId(), low, high );
	return PlayerBase.Cast( GetGame().GetObjectByNetworkId(low, high ));
}
