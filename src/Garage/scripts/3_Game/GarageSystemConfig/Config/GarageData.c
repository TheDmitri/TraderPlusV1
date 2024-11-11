class VehiculeData
{
	string 	VehicleName;
	int     VehicleCargoId;
	int     VehicleID;
	int     Timestamp;
	float 	EngineHealth;
	float 	FuelAmmount;
	int 		CollateralMoney;
	ref array<ref ItemsCargo> ItemsInCargo;

	void VehiculeData(string name, int vehicleCargoId ,int collateralMoney, int vdata,float enginehealth, float fuelamount, array<ref ItemsCargo>cargos, int timestamp = 0)
	{
		#ifdef GMDEBUG
		GetGMLogger().LogInfo("new VehiculeData");
		#endif
		VehicleName = name;
		VehicleCargoId = vehicleCargoId;
		VehicleID  = vdata;
		EngineHealth = enginehealth;
		FuelAmmount = fuelamount;
		CollateralMoney = collateralMoney;
		ItemsInCargo = new array<ref ItemsCargo>;
    ItemsInCargo = cargos;
		Timestamp = timestamp;
	}

	void DeleteCargoIfExist(int playerId)
	{
		GarageItemData garageItemData = GarageItemData.Load(playerId,VehicleCargoId);
		if(garageItemData)
			garageItemData.DeleteCargo(playerId, VehicleCargoId);
	}
}

class ItemsCargo
{
	string ItemName;
	int    Amount;
  float  Health;
	int    ExtraData;

	void ItemsCargo(string name, int amount, float health, int extradata = 0)
	{
		ItemName  = name;
		Amount 	  = amount;
    Health    = health;
		ExtraData = extradata;
	}
}

class GarageData
{
	//Define your configs variables there
	string 		OwnerName;
	string  	OwnerID;
	vector    ParkingPosition = Vector(-1,-1,-1);
	vector    ParkingOrientation = Vector(-1,-1,-1);
	ref       array<ref VehiculeData>Vehicles;

	void GarageData()
	{
		Vehicles = new array<ref VehiculeData>;
	}

	void DefaultGarageData(string path, string steamid, string name)	
	{
		OwnerName = name;
		OwnerID = steamid;
		//then it saves the file
		JsonFileLoader<GarageData>.JsonSaveFile(path, this);
	}

	void AddVehicle(string name, int cargoId, int collateralMoney, int data, float engineh, float fuela, array<ref ItemsCargo>itemsincargo, int timestamp = 0)
	{
		Vehicles.Insert(new VehiculeData(name, cargoId, collateralMoney, data, engineh, fuela, itemsincargo, timestamp));
	}

	void Save(string id)
	{
		string path = GM_DATABASE + id + ".json";
		JsonFileLoader<GarageData>.JsonSaveFile(path, this);
	}

	void DeleteIt(string id)
	{
		string path = GM_DATABASE + id + ".json";
		bool attempt = DeleteFile(path);
	}

	static ref GarageData Load(string id, string name)	{
		GarageData Data = new GarageData();

		//we check if config folders exist, if not we create them
		if ( !FileExist( GM_DB_DIR_SERVER ) )
		{
			MakeDirectory( GM_DB_DIR_SERVER );
		}

		string path = GM_DATABASE + id + ".json";
		//Now we check if config exist, if yes, we load it. if no, we call function defaultTraderPlusBankingData that will initialize default value
		if (FileExist(path)) 
		{
			//Print("file exist ! loading...");
			TraderPlusJsonLoader<GarageData>.LoadFromFile(path, Data);
			//JsonFileLoader<GarageData>.JsonLoadFile(path, Data);
			Data.Save(id);
		}
		else 
		{
			//Print("file doesn't exist ! creating...");
			Data.DefaultGarageData(path,id,name);
		}
		return Data;
	}
}

class GarageItemData
{
    string item;
    int quantity;
	int quick_slots_index;
    int slot;
    int magAmmoCount;
	int liquidType = 0;
	int foodstage = 0;
	int extraData;
	float temp = 0;
	float wetness = 0;
	float health;
    float energy;

    InventoryLocationType locType;
    int locIdx;
    int locRow;
    int locCol;
    bool locFlip;

    ref array<ref GarageItemData> childrens;

		static ref GarageItemData Load(int id, int cargoId)
		{
			GarageItemData data = new GarageItemData();

			//we check if config folders exist, if not we create them
			if ( !FileExist( GM_DB_DIR_SERVER ) )
			{
				MakeDirectory( GM_DB_DIR_SERVER );
			}

			string path = string.Format(GM_DB_DIR_PLAYER + "%2.json", id, cargoId);
			//Now we check if config exist, if yes, we load it. if no, we call function defaultTraderPlusBankingData that will initialize default value
			if (FileExist(path))
			{
				TraderPlusJsonLoader<GarageItemData>.LoadFromFile(path, data);
				return data;
			}

			return null;
		}

		void Save(int id, int cargoId)
		{
			if (!FileExist(string.Format(GM_DB_DIR_PLAYER,id)))
				MakeDirectory(string.Format(GM_DB_DIR_PLAYER,id));

			string path = string.Format(GM_DB_DIR_PLAYER + "%2.json",id, cargoId);
			JsonFileLoader<GarageItemData>.JsonSaveFile(path, this);
		}

		static void DeleteCargo(int id, int cargoId)
	  {
			string path = string.Format(GM_DB_DIR_PLAYER + "%2.json",id, cargoId);
			DeleteFile(path);
		}
}
