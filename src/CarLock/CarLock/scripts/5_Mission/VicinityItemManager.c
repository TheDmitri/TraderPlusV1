#ifndef CARLOCKDISABLE
modded class VicinityItemManager
{
	override void AddVicinityItems(Object object)
	{
		if ( CarShouldShowInVicinity(object) )
			super.AddVicinityItems(object);
	}

	bool CarShouldShowInVicinity(Object filtered_object)
	{
		//Print("StashShouldShowInVicinity");
		if (CarScript.Cast(filtered_object) && ( !GetGame().IsServer() || !GetGame().IsMultiplayer() ))
		{
			CarScript car = CarScript.Cast(filtered_object);
			if(car.m_CarLock_IsLocked)
				return false;
		}
		return true;
	};
}
#endif
