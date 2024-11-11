class SZSafeZoneServer
{

	void SZSafeZoneServer()
  {
		GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(InitSZServer, 1000);
  }

	void InitSZServer()
	{
		GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(CleanupTask, GetSZConfig().CleanUpTimer*1000, true);
	}

	void ~SZSafeZoneServer()
    {

    }

		void CleanupTask()
		{
			if(!GetSZConfig())return;
			for (int i = 0; i < GetSZConfig().SafeAreaLocation.Count(); i++)
			{
				SafeZoneLocation SafeZoneloc = GetSZConfig().SafeAreaLocation.Get(i);
				vector position = Vector(SafeZoneloc.X, 0, SafeZoneloc.Y);
				position = snapToGround(position);
				array<Object> nearObjects = new array<Object>;
				array<CargoBase> nearProxys = new array<CargoBase>;
				GetGame().GetObjectsAtPosition(position, SafeZoneloc.Radius, nearObjects, nearProxys);
				for (int x = 0; x < nearObjects.Count(); x++)
				{
					string type = nearObjects[x].GetType();
					for (int k = 0; k < GetSZConfig().ObjectsToDelete.Count(); k++)
					{
						string deletionObject = GetSZConfig().ObjectsToDelete.Get(k);
						if (type == deletionObject)
						{
							GetGame().ObjectDelete(nearObjects[x]);
						}
					}
				}
			}
		}

		private vector snapToGround(vector pos)
	    {
	        float pos_x = pos[0];
	        float pos_z = pos[2];
	        float pos_y = GetGame().SurfaceY( pos_x, pos_z );
	        vector tmp_pos = Vector( pos_x, pos_y, pos_z );
	        tmp_pos[1] = tmp_pos[1] + pos[1];

	        return tmp_pos;
	    }

		void GetEntitiesCleanUp(CallType type, ParamsReadContext ctx, PlayerIdentity sender, Object target)
	    {
	  	   if (!GetGame().IsServer())
	  		 return;

	       PlayerBase player = GetPlayerByIdentity(sender);
		   EntitiesCleanUp(player.GetPosition());

	       #ifdef SZDEBUG
	       GetTraderPlusLogger().LogInfo("GetEntitiesCleanUp");
	       #endif
		}

		void EntitiesCleanUp(vector pos)
	    {
	      array<Object> objs = new array<Object>;

	      GetGame().GetObjectsAtPosition3D(pos, 20, objs, NULL);
	      if (objs.Count() < 1)return;

	      for (int i = 0; i < objs.Count(); i++)
	      {
	        string tempObjId = objs.Get(i).ToString();
	        tempObjId.ToLower();			
	        if(!IsWhitelisted(objs[i]) && objs[i].IsKindOf("AnimalBase") || objs[i].IsKindOf("ZombieBase"))
	        {
	          GetGame().ObjectDelete(objs[i]);
	        }
	      }
	    }

		bool IsWhitelisted(Object obj)
		{
			foreach(string whitelisted: GetSZConfig().WhitelistEntities)
			{
				if(obj.IsKindOf(whitelisted))
					return true;
			}

			return false;
		}

		void CheckIfInsideSafeZone(PlayerBase player)
		{
			vector pos = player.GetPosition();
			for(int i=0; i< GetSZConfig().SafeAreaLocation.Count();i++)
			{
				float distance_squared = Math.Pow(GetSZConfig().SafeAreaLocation[i].X-pos[0], 2) + Math.Pow(GetSZConfig().SafeAreaLocation[i].Y-pos[2], 2);
				if( distance_squared <= Math.Pow(GetSZConfig().SafeAreaLocation[i].Radius, 2) )
				{
		      if(!player.GetSafeZoneStatus() == SZ_IN_SAFEZONE)
					{
						player.SetSafeZoneStatus(true);
						player.SetSafeZoneName(GetSZConfig().SafeAreaLocation[i].SafeZoneStatut);
						player.GetInputController().OverrideRaise(true, false);
					}
				}
				else if ( distance_squared > Math.Pow(GetSZConfig().SafeAreaLocation[i].Radius, 2) )
				{
					if(player.GetSafeZoneStatus() == SZ_IN_SAFEZONE)
					{
						player.SetSafeZoneStatus(false);
						player.SetSafeZoneName("");
						player.GetInputController().OverrideRaise(false, false);
					}
				}
			}
		}

		//for modding ability
		void GetSafeZoneStatutHandler(PlayerBase player, string safezoneName, bool state)
		{
			string prevName = player.GetSafeZoneName();
			player.SetSafeZoneName(safezoneName);
			player.SetSafeZoneStatus(state);
			if(player.GetSafeZoneStatus() == SZ_IN_SAFEZONE)
			{
				player.GetInputController().OverrideRaise(true, false);
				player.SetAllowDamage(false);
				player.m_SZ_InfluenzaEnteredSafeZone = player.m_AgentPool.GetSingleAgentCount(eAgents.INFLUENZA);
				player.m_AgentPool.SetAgentCount(eAgents.INFLUENZA, 0);
				if(player.IsRestrained())
				{
					player.SetRestrained(false);
					ItemBase item_in_hands = ItemBase.Cast(player.GetItemInHands());
					item_in_hands.AddHealth(-GameConstants.CLEAN_UNRESTRAIN_DAMAGE);
					MiscGameplayFunctions.TransformRestrainItem(item_in_hands, null, null, player);
				}
			}
			else
			{
				player.GetInputController().OverrideRaise(false, false);
				player.SetAllowDamage(true);
				player.m_AgentPool.SetAgentCount(eAgents.INFLUENZA, player.m_SZ_InfluenzaEnteredSafeZone);
			}
		}

	//ONLY Called on server
    void GetSafeStatut(CallType type, ParamsReadContext ctx, PlayerIdentity sender, Object target)
    {
  	 if (!GetGame().IsServer())
  		 return;

      Param2<string, bool> data;
    	 if (!ctx.Read(data))
    		 return;
       PlayerBase player = GetPlayerByIdentity(sender);
			 if(!player)return;

			 GetSafeZoneStatutHandler(player, data.param1, data.param2);

       #ifdef SZDEBUG
       GetTraderPlusLogger().LogInfo("GetSafeStatut");
       #endif
	  }

    PlayerBase GetPlayerByIdentity(PlayerIdentity sender)
  	{
  		int	low	 =	0;
  		int	high =	0;
  		GetGame().GetPlayerNetworkIDByIdentityID( sender.GetPlayerId(), low, high );
  		return PlayerBase.Cast( GetGame().GetObjectByNetworkId(low, high ));
  	}
}
