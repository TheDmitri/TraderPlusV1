#ifndef CARLOCKDISABLE
class CarLockCore
{
  ref CarLockServer m_CarLockServer;

	void CarLockCore()
  {
		GetTraderPlusLogger().LogInfo("CarLockCore - Started !");

		#ifdef CLDEBUG
		GetTraderPlusLogger().LogInfo("DEBUG MODE ENABLED");
		#endif

		/*We load every config, class that will run on server side here*/
		if (GetGame().IsServer() || !GetGame().IsMultiplayer())
		{
				m_CarLockServer = new CarLockServer;
		}
		else
		{
      InitClientFolder();
		}
		//we call our function that will initiaze our NPC
		InitRPC();
  }

	void ~CarLockCore(){
  }

	void InitClientFolder()
	{
			if ( !FileExist( CL_CONFIG_ROOT_CLIENT ) )
			{
				MakeDirectory( CL_CONFIG_ROOT_CLIENT );
				if ( !FileExist( CL_CONFIG_DIR_CLIENT ) )
				{
					MakeDirectory( CL_CONFIG_DIR_CLIENT );
				}
				if ( !FileExist( CL_LOGGER_DIR_CLIENT ) )
				{
					MakeDirectory( CL_LOGGER_DIR_CLIENT );
				}
			}
	}


	void InitRPC()
		{
			if (GetGame().IsServer())
			{
				//Server calls
				//GetRPCManager().AddRPC("ModName (try to be as specific as you can)", "NameOfYourRPCFunction", function where the RPC will be called, SingeplayerExecutionType.Client (Client when you do client to server));
				GetRPCManager().AddRPC("CarLock", "CarLockRequest", m_CarLockServer, SingeplayerExecutionType.Client);
				GetRPCManager().AddRPC("CarLock", "CarLockPasswordRequest", m_CarLockServer, SingeplayerExecutionType.Client);
			}
		}
	};
	#endif
