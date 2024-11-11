/*To make things easier for me, I use some static class that can be run from anywhere in my code,
it's very efficient to get config data or access my main class that run my code.
However it can create conflict with other mods if someone use to same name, so if you create some static function, make sure
to make the name specific as much as you can. I usually add Name of my mod in front of my function*/

/*Class that deals with my core class, where config, client and server part of my code are created*/
static ref TraderPlusBankingCore m_TraderPlusBankingCore;
static TraderPlusBankingCore GetTraderPlusBankingCore() {
	if ( !m_TraderPlusBankingCore ) {
		m_TraderPlusBankingCore = new TraderPlusBankingCore;
	}
	return m_TraderPlusBankingCore;
}

static ref TraderPlusBankingServer GetTraderPlusBankingServer()
{
	return GetTraderPlusBankingCore().m_TraderPlusBankingServer;
}

/*class that allows to access config on server side everywhere*/
static ref TraderPlusBankingSettings GetTraderPlusBankConfigServer()	{
	return GetTraderPlusBankingCore().m_TraderPlusBankingSettings;
}

/*clas that allows me to access the client class */
static ref TraderPlusBankingSettings GetTraderPlusBankingConfigClient()	{
	return GetTraderPlusBankingCore().m_TraderPlusBankingConfigClient;
}

	/*clas that allows me to access the client class */
static ref TraderPlusBankingData GetBankAccount()	{
		return GetTraderPlusBankingCore().m_BankAccount;
}
