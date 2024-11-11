
class TraderPlusBankingData
{
	//Define your configs variables there
	string 		Version = "";
	string    SteamID64="";
	string  	Name;
	int 			MoneyAmount;
	int   		MaxAmount;
	ref       TStringArray Licences;
	ref       TIntStringMap Insurances;

	void TraderPlusBankingData()
	{
		Licences = new TStringArray;
		Insurances = new TIntStringMap;
	}

	void DefaultTraderPlusBankingData(string id, int defaultMaxCurrency, int defaultStartCurrency,string name)
	{
		Version = "";
		SteamID64 = id;
		Name = name;
		MoneyAmount = defaultStartCurrency;
		MaxAmount = defaultMaxCurrency;
		//then it saves the file

		Save(SteamID64);
	}

	void UpdateAccount(PlayerBase player)
	{
		Save(player.GetIdentity().GetPlainId());
		GetRPCManager().SendRPC("TraderPlusBanking", "GetTraderPlusBankingBankAccount",  new Param1<ref TraderPlusBankingData>(this), true, player.GetIdentity());
	}

	void CheckVersion(string id)
	{
		if(Version != TRADERPLUS_CURRENT_VERSION && Version != "-1")
		{
			Version = TRADERPLUS_CURRENT_VERSION;
			if(id != SteamID64)
				SteamID64 = id;
			Save(SteamID64);
		}
	}

	void Save(string id)
	{
		JsonFileLoader<TraderPlusBankingData>.JsonSaveFile(string.Format(TPB_DB_FILENAME, id), this);
	}

	static ref TraderPlusBankingData GetAccount(PlayerBase player)
	{
		TraderPlusBankingData acc = new TraderPlusBankingData;
		return Load(player.GetIdentity().GetPlainId(), GetTraderPlusBankConfigServer().DefaultMaxCurrency,GetTraderPlusBankConfigServer().DefaultStartCurrency,player.GetIdentity().GetName());
	}

	static ref TraderPlusBankingData Load(string id, int defaultMaxCurrency,int defaultStartCurrency, string name = "")	{
		TraderPlusBankingData data = new TraderPlusBankingData;

		//we check if config folders exist, if not we create them
		if ( !FileExist( TPB_CONFIG_DIR ) )
		{
			MakeDirectory( TPB_CONFIG_DIR );
		}

		//Now we check if config exist, if yes, we load it. if no, we call function defaultTraderPlusBankingData that will initialize default value
		if (FileExist(string.Format(TPB_DB_FILENAME, id))) {
			//Print("file exist ! loading...");
			#ifdef TRADERPLUSDEBUG
			GetTraderPlusLogger().LogInfo("BankingData file exist ! loading..."); //sender.GetName() = player.GetIdentity().GetName()
			#endif
			//TraderPlusJsonLoader<TraderPlusBankingData>.LoadFromFile(path, Data);
			JsonFileLoader<TraderPlusBankingData>.JsonLoadFile(string.Format(TPB_DB_FILENAME, id), data);
			data.CheckVersion(id);
		}
		else {
			//Print("file doesn't exist ! creating...");
			#ifdef TRADERPLUSDEBUG
			GetTraderPlusLogger().LogInfo("BankingData doesn't exist ! creating..."); //sender.GetName() = player.GetIdentity().GetName()
			#endif
			data.DefaultTraderPlusBankingData(id,defaultMaxCurrency,defaultStartCurrency,name);
		}
		return data;
	}
}
