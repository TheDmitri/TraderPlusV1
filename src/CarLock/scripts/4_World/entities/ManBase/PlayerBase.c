#ifndef CARLOCKDISABLE
modded class PlayerBase extends ManBase
{
	int CLSteamlowID=-1;

	ref CarLockMenu m_CarLockMenu;

	override void Init()
	{
		super.Init();
		RegisterNetSyncVariableInt("CLSteamlowID");
	}

	void SetLowSteamUID(string playerUID)
	{
		string temp_ID="";
		for(int j = 8; j<17; j++)
		{
			temp_ID+=playerUID.Get(j);
		}
		CLSteamlowID = temp_ID.ToInt();
		//GetTraderPlusLogger().LogInfo("UID:"+playerUID+" CLSteamlowID:"+CLSteamlowID.ToString());
		SetSynchDirty();
	}

	bool HasPassword(int pwd, int id)
	{
		if(GetGame().IsClient())
		{
			CarLockData carLockData = CarLockData.Load(id.ToString());
			if(carLockData.CarLockPasswords.Count()>0)
			{
				for(int i=0;i<carLockData.CarLockPasswords.Count();i++)
				{
					if(carLockData.CarLockPasswords[i] == -1)continue;
					if(carLockData.CarLockPasswords[i] == pwd)
					{
						//GetTraderPlusLogger().LogInfo("HasPassword:true");
						return true;
					}
				}
			}
			//GetTraderPlusLogger().LogInfo("HasPassword:false");
			return false;
		}
		return false;
	}

	override void SetActions()
	{
		super.SetActions();
		AddAction(ActionCarLockMenu);
		AddAction(ActionCarLock);
		AddAction(ActionCarUnLock);
	}
};
#endif
