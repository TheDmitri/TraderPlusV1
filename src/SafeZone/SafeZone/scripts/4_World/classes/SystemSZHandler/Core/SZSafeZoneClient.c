class SZSafeZoneClient
{
  bool            HasStarted = false;
  bool			      SZStatut = false;
  string          SZName="";
  bool            OnLeaving = false;
  bool            kickrequest = false;
  vector          LastPlayerPos = Vector(0,10,0);
  float           DtDelay = 0.0;


  ref SafeZoneLocations m_SafeZoneLocations;

  ref SafeZoneNotificationUI m_SafeZoneNotificationUI;

	void SZSafeZoneClient()
  {

  }

  void StartSafeZone()
  {
      HasStarted = true;
      GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(SafeZoneCheck, 2000, true);
      GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(WhileInside, 20000,true);
  }

  void WhileInside()
  {
      if(SZStatut)
      {
        PlayerBase m_player = PlayerBase.Cast( GetGame().GetPlayer() );
        if(m_player)
        {
          if(m_SafeZoneLocations.EnableAfkDisconnect)
          {
            AfkKickerHandler(m_player);
          }
          if(CheckForEntities(m_player.GetPosition()))
          {
            SendEntitiesCleanUpRequest();
          }
        }
      }
  }

  void AfkKickerHandler(PlayerBase player)
  {
    if(kickrequest)return;

    if(DtDelay > m_SafeZoneLocations.KickAfterDelay)
    {
      float distance = vector.Distance(player.GetPosition(), LastPlayerPos);
      //player.MessageAction("DtDelay > m_SafeZoneLocations.KickAfterDelay : distance:"+distance);
      if(distance < 1)
      {
        NotificationSystem.AddNotificationExtended( 15, "#tpm_afk", "#tpm_afk_text", "SafeZone/images/SafeZone.paa" );
        kickrequest = true;
        GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(KickPlayer, 30000);
        return;
      }
      LastPlayerPos = player.GetPosition();
      DtDelay = 0.0;
    }
    else
    {
      DtDelay += 0.33;
      //player.MessageAction("DtDelay:"+DtDelay);
    }
  }

  void KickPlayer()
  {
      PlayerBase player = PlayerBase.Cast( GetGame().GetPlayer() );
      float distance = vector.Distance(player.GetPosition(), LastPlayerPos);
      //player.MessageAction("DtDeKickPlayerlay: "+distance);
      if(distance < 1)
      {
        GetGame().DisconnectSessionForce();
      }
      LastPlayerPos = Vector(0,10,0);
      kickrequest = false;
  }

  bool CheckForEntities(vector pos)
  {
      array<Object> objs = new array<Object>;

      GetGame().GetObjectsAtPosition3D(pos, 15, objs, NULL);
      if (objs.Count() < 1)return false;

      for (int i = 0; i < objs.Count(); i++)
      {
        string tempObjId = objs.Get(i).ToString();
        tempObjId.ToLower();
        if(objs.Get(i).IsKindOf("AnimalBase") || objs.Get(i).IsKindOf("ZombieBase"))
        {
          return true;
        }
      }

      return false;
    }

  void SafeZoneCheck()
	{
          PlayerBase m_player = PlayerBase.Cast( GetGame().GetPlayer() );
          if(m_player)
          {
          		for(int i = 0 ; i <= -1 + m_SafeZoneLocations.X.Count() ; i++)
          		{
               vector pos = m_player.GetPosition();
               SafeZoneRound(pos,m_SafeZoneLocations.X.Get(i),m_SafeZoneLocations.Y.Get(i),m_SafeZoneLocations.Radius.Get(i),m_SafeZoneLocations.Statut.Get(i), m_SafeZoneLocations.Countdowns.Get(i));
  				    }
              return;
          }
          else
          {
            DtDelay = 0.0;
            LastPlayerPos = Vector(0,10,0);
            SZStatut = false;
            SZName = "";
            m_SafeZoneLocations = NULL;
            HasStarted = false;
            GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).Remove(SafeZoneCheck);
            GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).Remove(WhileInside);
          }
  }

  void SafeZoneRound(vector pos,float m_X, float m_Y, float Zone_Radius,string Status, int countdown)
	{
    PlayerBase m_player = PlayerBase.Cast(GetGame().GetPlayer());
		float distance_squared = Math.Pow(m_X-pos[0], 2) + Math.Pow(m_Y-pos[2], 2);
		if ( distance_squared <= Math.Pow(Zone_Radius, 2) )
		{
      m_player.GetInputController().OverrideRaise(true, false);
      DisableOnLeaving();
			if (SZName!=Status && !SZStatut)
			{
				SZName=Status;
				SZStatut=true;
        SendSafeZoneStatut(Status, true);
        LastPlayerPos = m_player.GetPosition();
        NotificationSystem.AddNotificationExtended( 3, SZName, m_SafeZoneLocations.MsgEnterZone, "SafeZone/images/SafeZone.paa" );
			}
		}
		else if ( distance_squared > Math.Pow(Zone_Radius, 2) )
		{
				if (SZName==Status && SZStatut && OnLeaving == false)
				{
          StartOnLeaving(countdown);
        }
		}
	}

  void DisableOnLeaving()
  {
    OnLeaving = false;
    GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).Remove(EndOnLeaving);
    if(m_SafeZoneNotificationUI){
      m_SafeZoneNotificationUI.Delete();
    }
  }

  void StartOnLeaving(int countdown)
  {
    PlayerBase player = PlayerBase.Cast(GetGame().GetPlayer());
    if(!player.IsAlive())return;
    OnLeaving = true;
    m_SafeZoneNotificationUI = new SafeZoneNotificationUI;
    m_SafeZoneNotificationUI.StartCountdown(countdown, m_SafeZoneLocations.MsgOnLeavingZone);
    GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(EndOnLeaving, countdown*1000);
  }

  void EndOnLeaving()
  {
    PlayerBase m_player = PlayerBase.Cast(GetGame().GetPlayer());
    m_player.GetInputController().OverrideRaise(false, false);
    SendSafeZoneStatut("", false);
    NotificationSystem.AddNotificationExtended( 3, SZName, m_SafeZoneLocations.MsgExitZone, "SafeZone/images/SafeZone.paa" );
    SZName="";
    SZStatut=false;
    DtDelay = 0.0;
    LastPlayerPos = Vector(0,10,0);
  }

	void ~SZSafeZoneClient(){

  }

    //Only called on client
  void GetConfigResponse(CallType type, ParamsReadContext ctx, PlayerIdentity sender, Object target)	
  {
     if (!GetGame().IsClient())
         return;
     Param1<ref SafeZoneLocations> data;
     if (!ctx.Read(data))
         return;

     m_SafeZoneLocations = data.param1;

     if(!HasStarted)
     StartSafeZone();
    }

    void SendSafeZoneStatut(string name, bool rep)
    {
      //Print("SendRadZoneStatut");
      GetRPCManager().SendRPC("SZSafeZone", "GetSafeStatut",  new Param2<string,bool>(name,rep), true, NULL);
    }

    void SendEntitiesCleanUpRequest()
    {
      GetRPCManager().SendRPC("SZSafeZone", "GetEntitiesCleanUp",  NULL, true, NULL);
    }
}
