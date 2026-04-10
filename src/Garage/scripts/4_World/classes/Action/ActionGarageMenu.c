class ActionGarageMenu: ActionInteractBase
{
	private int    TraderID;
	private vector Pos;
	private bool   IsFlag;
	private int    GarageID;

	void ActionGarageMenu()
	{
		TraderID = -1;
		IsFlag = false;
		Pos = Vector(0,0,0);
		m_CommandUID = DayZPlayerConstants.CMD_ACTIONMOD_INTERACTONCE;
		m_StanceMask = DayZPlayerConstants.STANCEMASK_ERECT | DayZPlayerConstants.STANCEMASK_CROUCH;
		m_HUDCursorIcon = CursorIcons.CloseHood;
	}

    override void CreateConditionComponents()
		{
			m_ConditionTarget = new CCTObject(8);
			m_ConditionItem = new CCINone;
		}

	override string GetText()
	{
		return "#gm_garage_menu";
	}

	override bool ActionCondition( PlayerBase player, ActionTarget target, ItemBase item )
  {
      if ( GetGame().IsServer() )
          return true;

			// TerritoryFlag flag = TerritoryFlag.Cast(target.GetObject())
			// if(flag)
			// {
			// 	Pos = flag.GetPosition();
			// 	IsFlag    = true;
			// }

      PlayerBase ntarget = PlayerBase.Cast( target.GetObject() );
			//Print("ID Target:"+ntarget.TraderID.ToString()+"ntarget.MaxPlayerPerID:"+ntarget.MaxPlayerPerID.ToString());
			if(ntarget && ntarget.TraderID == -3)
			{
				TraderID = ntarget.TraderID;
				Pos = ntarget.GetPosition();
				return true;
			}

			BuildingBase nObject = BuildingBase.Cast( target.GetObject() );
			if (nObject && nObject.TraderID == -3)
			{
				TraderID = nObject.TraderID;
				Pos = nObject.GetPosition();
				return true;
			}
			return false;
  }

  override void OnStartClient(ActionData action_data)
  {
    OpenGarageMenu();
  }

	void OpenGarageMenu()
	{
		if (g_Game.GetUIManager().GetMenu() == NULL)
		{
			int LowUID = GarageHelpers.GetLowSteamID(GetGame().GetUserManager().GetTitleInitiator().GetUid());
			GetRPCManager().SendRPC("Garage", "GarageRequest", new Param3<int, vector, bool>(LowUID, Pos, IsFlag), true, NULL);
			if (GetTraderPlusUIService())
			{
				GetTraderPlusUIService().OpenGarageView(LowUID, Pos, IsFlag);
			}
		}
	}
}
