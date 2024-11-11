class GarageSettings
{
	string Version;
	bool UseGarageOnlyToTrade;
	bool VehicleMustHaveLock;
	bool SaveVehicleLock;
	bool SaveVehicleCargo;
	bool SaveVehicleHealth;
	bool SaveVehicleFuel;
	int  SavedVehicleInGarageForTradeInHour = 1;
	int  MaxVehicleStored = 5;
	int  ParkInCost;
	int  ParkOutCost;
	bool PayWithBankAccount;
	ref  TStringArray WhitelistedObjects;
	ref  array<ref GarageNPC>NPCs;
	string ParkingNotAvailable = "The park area is not available!";
	string NotEnoughMoney = "You don't have enough money";
	string NotRightToPark = "You don't have the right to park in this vehicle!";
	string CarHasMember = "The car still have member inside!";
	string ParkInFail = "The car was not able to be park in correctly";
	string ParkInSuccess = "The car was successfully parked";
	string ParkOutFail = "The car was not able to spawn!";
	string ParkOutSuccess = "The car was able to spawn!";
	string MaxVehicleStoredReached = "Max number of vehicle reached!";
	string TradeVehicleWarning = "The vehicle has been stored for %1 hour, passed this point it will be lost so trade it or get it fast";
	string TradeVehicleHasBeenDeleted = "The vehicle stored for trade reached his inactivity and has been deleted!";

	void GarageSettings()
	{
		WhitelistedObjects = new TStringArray;
		NPCs = new array<ref GarageNPC>;
	}

	void DefaultGarageSettings()
	{
		VehicleMustHaveLock = true;
		SaveVehicleCargo = false;
		SaveVehicleFuel = true;
		SaveVehicleHealth = true;
		ParkInCost = 100;
		ParkOutCost= 100;
		PayWithBankAccount= false;//then it saves the file
		NPCs.Insert(new GarageNPC("TraderPlusSign",Vector(3723.84,	402.012,	5990.04), Vector(0, 0,			0), Vector(3719.71,	402.012,	5994.85),Vector(-42,		0,			0)));
		Save();
	}

	void Save()
	{
		JsonFileLoader<GarageSettings>.JsonSaveFile(GM_CONFIG, this);
	}

	void CheckVersion()
	{
		if(Version != TRADERPLUS_CURRENT_VERSION)
		{
			Version = TRADERPLUS_CURRENT_VERSION;
			Save();
		}
	}

	static ref GarageSettings Load()	{
		GarageSettings settings = new GarageSettings();

		//we check if config folders exist, if not we create them
		if ( !FileExist( GM_CONFIG_ROOT_SERVER ) )
		{
			MakeDirectory( GM_CONFIG_ROOT_SERVER );
			if ( !FileExist( GM_CONFIG_DIR_SERVER ) )
			{
				MakeDirectory( GM_CONFIG_DIR_SERVER );
			}
			if ( !FileExist( GM_LOGGER_DIR_SERVER ) )
			{
				MakeDirectory( GM_LOGGER_DIR_SERVER );
			}
		}

		//Now we check if config exist, if yes, we load it. if no, we call function defaultGaragesettings that will initialize default value
		if (FileExist(GM_CONFIG)) {
			//Print("file exist ! loading...");
			JsonFileLoader<GarageSettings>.JsonLoadFile(GM_CONFIG, settings);
			settings.CheckVersion();
		}
		else {
			//Print("file doesn't exist ! creating...");
			settings.DefaultGarageSettings();
		}

		return settings;
	}
}
