#ifndef CARLOCKDISABLE
modded class MissionGameplay extends MissionBase
{
	void MissionGameplay()
	{
		/*we create the core function on client in order to have the client side working*/
		m_CarLockCore = NULL;
		GetCarLockCore();
	}

	void TryToLockVehicleNearby(PlayerBase player)
	{
		array<Object> objs = new array<Object>;

		GetGame().GetObjectsAtPosition3D(player.GetPosition(), 5, objs, NULL);
		if (objs.Count() < 1)return;

		for (int i = 0; i < objs.Count(); i++)
		{
			CarScript car = CarScript.Cast(objs[i]);
			if(car)
			{
				if (!car.m_CarLock_IsLocked && car.m_CarLockOwner !=-1 && (car.m_CarLockOwner == player.CLSteamlowID || player.HasPassword(car.m_CarLockPassword,car.m_CarLockOwner)))
					GetRPCManager().SendRPC("CarLock", "CarLockRequest",  new Param2<CarScript, bool>(car,true), true, NULL);
				else if (car.m_CarLock_IsLocked && car.m_CarLockOwner !=-1 && (car.m_CarLockOwner == player.CLSteamlowID || player.HasPassword(car.m_CarLockPassword,car.m_CarLockOwner)))
					GetRPCManager().SendRPC("CarLock", "CarLockRequest",  new Param2<CarScript, bool>(car,false), true, NULL);

				return;
			}
		}
	}

	/*override void OnKeyRelease(int key)
	{
		super.OnKeyRelease(key);
		PlayerBase player = PlayerBase.Cast(GetGame().GetPlayer());
		if ( key == KeyCode.KC_ESCAPE )
		{
			if (player && player.m_CarLockMenu)
				player.m_CarLockMenu.OnHide();
		}

		if ( key == KeyCode.KC_U )
			TryToLockVehicleNearby(player);
	}*/

	override void OnUpdate( float timeslice )
	{
			super.OnUpdate(timeslice);
			PlayerBase player = PlayerBase.Cast(GetGame().GetPlayer());

			if (!GetUIManager().GetMenu() /*no other menu open*/ && GetUApi().GetInputByName("UACarLockToggle").LocalPress() )
					TryToLockVehicleNearby(player);
	}
};
#endif
