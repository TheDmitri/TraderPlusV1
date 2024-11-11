//How to access the module anywhere
//TraderPlusSafeZone groupModule = TraderPlusSafeZone.Cast(CF_ModuleCoreManager.Get(TraderPlusSafeZone));

[CF_RegisterModule(TraderPlusSafeZone)]
class TraderPlusSafeZone: CF_ModuleWorld
{
	ref SafeAreaSettings m_ActiveSafeAreaSettings;
	ref SafeZoneLocations m_SafeZoneLocations;

	ref SZSafeZoneClient m_SZSafeZoneClient;
	ref SZSafeZoneServer m_SZSafeZoneServer;

	void TraderPlusSafeZone()
	{

	}

	void ~TraderPlusSafeZone()
	{

	}

	override void OnInit()
	{
		super.OnInit();
		EnableInvokeConnect();
		EnableClientLogout();
		EnableClientLogoutCancelled();
		EnableMissionFinish();
		EnableMissionStart();
		EnableUpdate();
	}

	void OnMissionStartHandler()
  	{
    	if (GetGame().IsServer())
    	{
          GetTraderPlusLogger().LogInfo("SafeZoneCore - Started !");
		  
		  #ifdef SZDEBUG
      	    GetTraderPlusLogger().LogInfo("DEBUG MODE ENABLED");
      	  #endif

          m_ActiveSafeAreaSettings = SafeAreaSettings.Load();
          m_SafeZoneLocations = new SafeZoneLocations(m_ActiveSafeAreaSettings);
          m_SZSafeZoneServer = new SZSafeZoneServer;
    	}
    	else
    	{
          m_SZSafeZoneClient = new SZSafeZoneClient;
    	}
      InitRPC();
  	}

	void InitRPC()
	{
			if (GetGame().IsServer())	
			{
				//Server calls
				GetRPCManager().AddRPC("SZSafeZone", "GetSafeStatut", m_SZSafeZoneServer, SingeplayerExecutionType.Client);
        		GetRPCManager().AddRPC("SZSafeZone", "GetEntitiesCleanUp", m_SZSafeZoneServer, SingeplayerExecutionType.Client);
      		}
			else	
			{
				//Client calls
        		GetRPCManager().AddRPC("SZSafeZone", "GetConfigResponse", m_SZSafeZoneClient, SingeplayerExecutionType.Server);
			}
	}

	void OnPlayerConnect(PlayerBase player)
	{
		if(player.IsTraderPlusAdmin())
			player.SetAdminStatus();

		if(m_SafeZoneLocations)
			GetRPCManager().SendRPC("SZSafeZone", "GetConfigResponse", new Param1<ref SafeZoneLocations>(m_SafeZoneLocations), true, player.GetIdentity());
	}

	void OnPlayerLogout(PlayerBase player)
	{
		HideOutObject stash;
		stash = HideOutObject.Cast(player.FindAttachmentBySlotName("Armband"));
		if(stash)
		{
			#ifdef SZDEBUG
				GetTraderPlusLogger().LogInfo("disconnect player:" + player.GetIdentity().GetName() + "'s stash has been droped");
			#endif
			player.ServerDropEntity(stash);
			stash.Hide();
		}
	}

	void OnPlayerLogoutCancel(PlayerBase player)
	{
		if(m_ActiveSafeAreaSettings.IsHideOutActive)
			player.HideOutObjectHandle();
	}

	//----------------------------------------------------------------//
	//End - Legacy RPC
	//----------------------------------------------------------------//

	//----------------------------------------------------------------//
	//Start - Events CF Module
	//----------------------------------------------------------------//
	override void OnMissionStart(Class sender, CF_EventArgs args)
	{
		super.OnMissionStart(sender, args);
		OnMissionStartHandler();
	}

	override void OnMissionFinish(Class sender, CF_EventArgs args)
	{
		super.OnMissionFinish(sender, args);
	}

	override void OnClientLogout(Class sender, CF_EventArgs args)
	{
		super.OnClientLogout(sender, args);

		auto cArgs = CF_EventPlayerDisconnectedArgs.Cast(args);

		if (GetGame().IsServer() && GetGame().IsMultiplayer())
		{
			if (!cArgs.Identity)
				return;

			OnPlayerLogout(cArgs.Player);
		}
	}

	// -----------------------------------------------------------
	// ExpansionQuestModule OnClientLogout
	// -----------------------------------------------------------
	override void OnClientLogoutCancelled(Class sender, CF_EventArgs args)
	{
		super.OnClientLogoutCancelled(sender, args);

		auto cArgs = CF_EventPlayerArgs.Cast(args);

		if (GetGame().IsServer() && GetGame().IsMultiplayer())
		{
			if (!cArgs.Player)
				return;

			OnPlayerLogoutCancel(cArgs.Player);
		}
	}

	//! OnInvokeConnect is called on each connect, reconnect, and respawn
	override void OnInvokeConnect(Class sender, CF_EventArgs args)
	{
		super.OnInvokeConnect(sender, args);

		auto cArgs = CF_EventPlayerArgs.Cast(args);

		if (!IsMissionHost())
			return;

		if (!cArgs.Player || !cArgs.Identity)
			return;

		OnPlayerConnect(cArgs.Player);
 }

	override void OnUpdate(Class sender, CF_EventArgs args)
	{
		super.OnUpdate(sender, args);

		if (!IsMissionHost())
			return;

		auto update = CF_EventUpdateArgs.Cast(args);
	}
	//----------------------------------------------------------------//
	//End - Events CF Module
	//----------------------------------------------------------------//
};
