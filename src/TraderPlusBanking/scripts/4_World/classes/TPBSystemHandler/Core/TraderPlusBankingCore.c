
class TraderPlusBankingCore
{
	ref TraderPlusBankingSettings m_TraderPlusBankingSettings;
	ref TraderPlusBankingSettings m_TraderPlusBankingConfigClient;

  ref TraderPlusBankingServer 	m_TraderPlusBankingServer;

	ref TraderPlusBankingData   	m_BankAccount;

	void TraderPlusBankingCore()
  {
		GetTraderPlusLogger().LogInfo("TraderPlusBankingCore - Started !");
		#ifdef TRADERPLUSDEBUG
		GetTraderPlusLogger().LogInfo("DEBUG MODE ENABLED");
		#endif

		/*We load every config, class that will run on server side here*/
		if (GetGame().IsServer() || !GetGame().IsMultiplayer())
		{
			//LOADING CONFIG FILES...
			m_TraderPlusBankingSettings = m_TraderPlusBankingSettings.Load();

			//Creating server side part
			m_TraderPlusBankingServer = new TraderPlusBankingServer;
		}
		else
		{
			/*Then we run every config and class that will run on client only*/
			m_TraderPlusBankingConfigClient = new TraderPlusBankingSettings;
			m_BankAccount = new TraderPlusBankingData;
		}
		//we call our function that will initiaze our NPC
		InitRPC();
  }

	void ~TraderPlusBankingCore(){
  }

	void InitRPC()
		{
			if (GetGame().IsServer())
			{
				//Server calls
				GetRPCManager().AddRPC("TraderPlusBanking", "TraderPlusBankingRequest", m_TraderPlusBankingServer, SingeplayerExecutionType.Client);
				GetRPCManager().AddRPC("TraderPlusBanking", "TraderPlusBankingWithdraw", m_TraderPlusBankingServer, SingeplayerExecutionType.Client);
				GetRPCManager().AddRPC("TraderPlusBanking", "TraderPlusBankingDeposit", m_TraderPlusBankingServer, SingeplayerExecutionType.Client);
				GetRPCManager().AddRPC("TraderPlusBanking", "TraderPlusTransfertRequest", m_TraderPlusBankingServer, SingeplayerExecutionType.Client);
			}
			else
			{
				//Client calls
				GetRPCManager().AddRPC("TraderPlusBanking", "GetTraderPlusBankingClientConfig", this, SingeplayerExecutionType.Server);
				GetRPCManager().AddRPC("TraderPlusBanking", "GetTraderPlusBankingBankAccount", this, SingeplayerExecutionType.Server);
			}
		}


		void TransfertTraderPlusBankingConfigToClient(PlayerIdentity sender = NULL)
		{
				GetRPCManager().SendRPC("TraderPlusBanking", "GetTraderPlusBankingClientConfig",  new Param1<ref TraderPlusBankingSettings>(m_TraderPlusBankingSettings), true, sender);
				#ifdef TRADERPLUSDEBUG
	      GetTraderPlusLogger().LogInfo("TransfertTraderPlusBankingConfigToClient");
	      #endif
		}

		/*RPC Function called on client*/
		void GetTraderPlusBankingClientConfig(CallType type, ParamsReadContext ctx, PlayerIdentity sender, Object target)
		{
			if(!GetGame().IsClient())return;

			Param1<ref TraderPlusBankingSettings> data;
	     if (!ctx.Read(data))
	       return;

			#ifdef TRADERPLUSDEBUG
	 		GetTraderPlusLogger().LogInfo("GetTraderPlusBankingClientConfig");
	 		#endif

		  m_TraderPlusBankingConfigClient = data.param1;
		}

		/*RPC Function called on client*/
		void GetTraderPlusBankingBankAccount(CallType type, ParamsReadContext ctx, PlayerIdentity sender, Object target)
		{
			if(!GetGame().IsClient())return;

			Param1<ref TraderPlusBankingData> data;
			 if (!ctx.Read(data))
				 return;

			#ifdef TRADERPLUSDEBUG
			GetTraderPlusLogger().LogInfo("GetTraderPlusBankingBankAccount");
			#endif

			m_BankAccount = data.param1;

			PlayerBase player = PlayerBase.Cast(GetGame().GetPlayer());

			if(player && player.m_TraderPlusBankingMenu)
				player.m_TraderPlusBankingMenu.UIHandle();

			if(player && player.m_TraderPlusMenu)
				player.m_TraderPlusMenu.UIHandle();

			if(player && player.m_GarageMenu)
				player.m_GarageMenu.UIHandle();
		}
	};
