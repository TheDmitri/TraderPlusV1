modded class CarScript
{
    private int m_carUniqueId = 0;
    private static ref map<int, CarScript> m_MapAllCars = new map<int, CarScript>;

    string m_TraderPlus_LastDriverID = "";

    void CarScript()
	  {
      RegisterNetSyncVariableInt("m_carUniqueId");

      if(GetGame().IsClient())
        return;
        
      HandleCarDamage();
	  }

    void HandleCarDamage()
    {
      SetAllowDamage(false);

      if(IsServerAllowDamage())
      {
        GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(DisableGodMode, 30000);
      }
    }

    bool IsServerAllowDamage()
    {
      return true;
    }

    void ~CarScript()
    {
      if(m_MapAllCars)
   		m_MapAllCars.Remove(m_carUniqueId);
    }

    static map<int, CarScript> GetMapAll()
   	{
   		return m_MapAllCars;
   	}

    int GetCarUniqueId()
    {
      return m_carUniqueId;
    }

    void SetCarUniqueId(int id)
    {
      m_carUniqueId = id;
      m_MapAllCars.Set(id, this);
    }

    override void OnEngineStart()
 	  {
        super.OnEngineStart();
        PlayerBase player = PlayerBase.Cast(CrewMember(DayZPlayerConstants.VEHICLESEAT_DRIVER));
        if (player)
          m_TraderPlus_LastDriverID = player.GetIdentity().GetPlainId();
 	  }

    void DisableGodMode()
    {
      SetAllowDamage(true);
    }

    override void OnStoreSave( ParamsWriteContext ctx )
     {
        super.OnStoreSave(ctx);
        Param1<int> data = new Param1<int>(m_carUniqueId);
        ctx.Write(data);
     }

    override bool OnStoreLoad( ParamsReadContext ctx, int version )
      {
        if ( !super.OnStoreLoad( ctx, version ) )
          return false;

            Param1<int>data = new Param1<int>(0);
            if (ctx.Read(data))
            {
              m_carUniqueId = data.param1;
              SetCarUniqueId(m_carUniqueId);
            }
            return true;
      }
}
