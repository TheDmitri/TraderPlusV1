class GarageCore
{
	ref GarageSettings m_GarageSettings;

  ref GarageServer m_GarageServer;

	ref TStringArray m_VehiclesName;
	ref TIntArray m_VehiclesHealth;

	void GarageCore()
  {
		GetGMLogger().LogInfo("GarageCore - Started !");
		#ifdef TPDEBUG
		GetGMLogger().LogInfo("DEBUG MODE ENABLED");
		#endif

		/*We load every config, class that will run on server side here*/
		if (GetGame().IsServer() || !GetGame().IsMultiplayer())
		{
			//LOADING CONFIG FILES...
			m_GarageSettings = GarageSettings.Load();

			//Creating server side part
			m_GarageServer = new GarageServer;
		}
		else
		{
			/*Then we run every config and class that will run on client only*/
			m_GarageSettings = new GarageSettings;
			m_VehiclesName = new TStringArray;
			m_VehiclesHealth = new TIntArray;
		}
		//we call our function that will initiaze our NPC
		InitRPC();
  }

	void ~GarageCore(){
  }

	void InitRPC()
		{
			if (GetGame().IsServer())
			{
				//Server calls
				//GetRPCManager().AddRPC("ModName (try to be as specific as you can)", "NameOfYourRPCFunction", function where the RPC will be called, SingeplayerExecutionType.Client (Client when you do client to server));
				GetRPCManager().AddRPC("Garage", "GarageRequest", m_GarageServer, SingeplayerExecutionType.Client);
				GetRPCManager().AddRPC("Garage", "ParkInRequest", m_GarageServer, SingeplayerExecutionType.Client);
				GetRPCManager().AddRPC("Garage", "ParkOutRequest", m_GarageServer, SingeplayerExecutionType.Client);
			}
			else
			{
				//Client calls
				//GetRPCManager().AddRPC("ModName (try to be as specific as you can)", "NameOfYourRPCFunction", function where the RPC will be called, SingeplayerExecutionType.Server (Server when you do Server to Client));
				GetRPCManager().AddRPC("Garage", "GarageResponse", this, SingeplayerExecutionType.Server);
				GetRPCManager().AddRPC("Garage", "GetGarageClientConfig", this, SingeplayerExecutionType.Server);
			}
		}


		void TransfertGarageConfigToClient(PlayerIdentity sender = NULL)
		{
				/*then we send our npc that will send the config to clien
				GetRPCManager().SendRPC("Same Name than mentioned above", "function that will be called on client(defined in InitRPC())",  (parameters that you will send to client) => new Param1<ref GarageConfigClient>(temp_config), true, sender (if sender = NULL, RPC send to all client, if not specific player));
				*/
				GetRPCManager().SendRPC("Garage", "GetGarageClientConfig",  new Param1<ref GarageSettings>(m_GarageSettings), true, sender);
		}

		/*RPC Function called on client*/
		void GarageResponse(CallType type, ParamsReadContext ctx,  PlayerIdentity sender,  Object target)
		{
			if(!GetGame().IsClient())return;

			Param4<ref TStringArray, ref TIntArray ,vector, int> data;

			if (!ctx.Read(data))
				return;

			#ifdef GMDEBUG
		 	GetGMLogger().LogInfo("GarageResponse data:"+data.param1.Count() + " " +  data.param2);
		 	#endif

			PlayerBase player = PlayerBase.Cast(GetGame().GetPlayer());

			m_VehiclesName.Clear();
			m_VehiclesHealth.Clear();

			m_VehiclesName = data.param1;
			m_VehiclesHealth = data.param2;

			if(player && player.m_GarageMenu)
			{
				player.m_GarageMenu.SetResponseData(data.param1,data.param3);
				GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(player.m_GarageMenu.UIHandle, 50);

				switch(data.param4)
				{
					case TraderPlusResponse.SELL_SUCCESS:player.PlayTradeSound(true);
					break;

					case TraderPlusResponse.BUY_SUCCESS:player.PlayTradeSound(false);
					break;

					default:
					break;
				}
			}

			#ifdef GMDEBUG
	 		GetGMLogger().LogInfo("GarageResponse");
	 		#endif
		}

		void GetGarageClientConfig(CallType type, ParamsReadContext ctx,  PlayerIdentity sender,  Object target)
		{
			if(!GetGame().IsClient())return;

			Param1<ref GarageSettings> data;

			if (!ctx.Read(data))
				return;

			m_GarageSettings = data.param1;

			#ifdef GMDEBUG
	 		GetGMLogger().LogInfo("GetGarageClientConfig");
	 		#endif
		}
	};
