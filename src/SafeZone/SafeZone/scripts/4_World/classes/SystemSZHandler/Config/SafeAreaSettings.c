class SafeAreaSettings
{
	string    Version = "";
	bool      IsHideOutActive = true;
	bool      EnableAfkDisconnect;
	int       KickAfterDelay;
	string 	  MsgEnterZone;
	string 	  MsgExitZone;
	string    MsgOnLeavingZone;
	string    MustRemoveArmband = "You must remove your armband in order to make your stash appear !";
	ref array<ref SafeZoneLocation> SafeAreaLocation;
	int CleanUpTimer;
	ref array<string>WhitelistEntities;
	ref array<string>ObjectsToDelete;
	ref array<string>SZSteamUIDs;
	ref array<string>BlackListedItemInStash;

	void SafeAreaSettings() {
		SZSteamUIDs =  new array<string>;
		WhitelistEntities = new array<string>;
		ObjectsToDelete = new array<string>;
		BlackListedItemInStash = new array<string>;
		SafeAreaLocation = new array<ref SafeZoneLocation>;
	}

	void SafeAreaDefaultSettings()	{
		EnableAfkDisconnect = false;
		KickAfterDelay = 30;
    	CleanUpTimer = 1200;
		MsgEnterZone="You Have Entered The Safe Zone!";
		MsgExitZone="You Have Left The Safe Zone!";
		MsgOnLeavingZone="You're leaving the safezone in:";
		SafeAreaLocation.Insert(new SafeZoneLocation("Trader", 11595, 14392, 150, 30));
		WhitelistEntities.Insert("Animal_CervusElaphus");
		ObjectsToDelete.Insert("Paper");
		SZSteamUIDs.Insert("76561198047475641");
		BlackListedItemInStash.Insert("Apple");
		Save();
	}

	void Save()
	{
		JsonFileLoader<SafeAreaSettings>.JsonSaveFile(SZ_CONFIG, this);
	}

	void CheckVersion()
	{
		if(Version != TRADERPLUS_CURRENT_VERSION)
		{
			Version = TRADERPLUS_CURRENT_VERSION;
			Save();
		}
	}

	static ref SafeAreaSettings Load()	{
		SafeAreaSettings settings = new SafeAreaSettings();

		if ( !FileExist( SZ_CONFIG_ROOT_SERVER ) )
		{
			MakeDirectory( SZ_CONFIG_ROOT_SERVER );
			if ( !FileExist( SZ_CONFIG_DIR_SERVER ) )
			{
				MakeDirectory( SZ_CONFIG_DIR_SERVER );
			}
		}

		if (FileExist(SZ_CONFIG)) {
			JsonFileLoader<SafeAreaSettings>.JsonLoadFile(SZ_CONFIG, settings);
			settings.CheckVersion();
		}
		else {
			settings.SafeAreaDefaultSettings();
		}

		return settings;
	}
}
