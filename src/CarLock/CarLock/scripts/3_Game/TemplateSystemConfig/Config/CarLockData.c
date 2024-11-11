class CarLockData
{
	ref TIntArray CarLockPasswords;

	void CarLockData()
	{
		CarLockPasswords = new TIntArray;
	}

	void Save(string id)
	{
		string Path = CL_DATA + "_" + id + ".json";

		//then it saves the file
		JsonFileLoader<CarLockData>.JsonSaveFile(Path, this);
	}

	static ref CarLockData Load(string id)	{
		ref CarLockData settings = new CarLockData();

		string Path = CL_DATA + "_" + id + ".json";

		//Now we check if config exist, if yes, we load it. if no, we call function defaultCarLocksettings that will initialize default value
		if (FileExist(Path)) {
			//Print("file exist ! loading...");
			JsonFileLoader<CarLockData>.JsonLoadFile(Path, settings);
		}

		return settings;
	}
}
