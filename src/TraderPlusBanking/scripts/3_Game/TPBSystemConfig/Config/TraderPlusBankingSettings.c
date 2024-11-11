
class TraderPlusBankingSettings
{
	//Define your configs variables there
	string Version="";
	bool  BankingLogs = false;
	bool  IsCreditCarNeededForTransaction;
	float TransactionFees;
	int   DefaultStartCurrency;
	int   DefaultMaxCurrency;
	ref    TStringArray CurrenciesAccepted;
	string TheAmountHasBeenTransferedToTheAccount = "The Amount has been successfully send to the account of destination !";
	string TheAmountErrorTransferAccount = "The destination account doesn't exist !";

	void TraderPlusBankingSettings()
	{
		CurrenciesAccepted = new TStringArray;
	}

	void DefaultTraderPlusBankingSettings()
	{
		IsCreditCarNeededForTransaction = false;
		TransactionFees = 0.05;
		DefaultStartCurrency = 0;
		DefaultMaxCurrency = 1000000;
	  //then it saves the file
		Save();
	}

	void Save()
	{
		JsonFileLoader<TraderPlusBankingSettings>.JsonSaveFile(TPB_CONFIG, this);
	}

	void CheckVersion()
	{
		if(Version != TRADERPLUS_CURRENT_VERSION)
		{
			Version = TRADERPLUS_CURRENT_VERSION;
			/*
				code here
			*/
			Save();
		}
	}

	static ref TraderPlusBankingSettings Load()	{
		TraderPlusBankingSettings settings = new TraderPlusBankingSettings();

		//we check if config folders exist, if not we create them
		if ( !FileExist( TPB_CONFIG_DIR ) )
		{
			MakeDirectory( TPB_CONFIG_DIR );
		}

		//Now we check if config exist, if yes, we load it. if no, we call function defaultTraderPlusBankingsettings that will initialize default value
		if (FileExist(TPB_CONFIG)) {
			#ifdef TRADERPLUSDEBUG
      GetTraderPlusLogger().LogInfo("BankingSettings file exist ! loading..."); //sender.GetName() = player.GetIdentity().GetName()
      #endif
			TraderPlusJsonLoader<TraderPlusBankingSettings>.LoadFromFile(TPB_CONFIG, settings);
		  settings.CheckVersion();
			//JsonFileLoader<TraderPlusBankingSettings>.JsonLoadFile(TPB_CONFIG, settings);
		}
		else {
			#ifdef TRADERPLUSDEBUG
      GetTraderPlusLogger().LogInfo("BankingSettings doesn't exist ! creating..."); //sender.GetName() = player.GetIdentity().GetName()
      #endif
			settings.DefaultTraderPlusBankingSettings();
		}

		return settings;
	}
}
