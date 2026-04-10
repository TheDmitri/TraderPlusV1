class ActionTraderPlusMenu: ActionInteractBase
{
	private int TraderID;
	private vector TraderPos;
	private string m_TraderName;
	private string m_TraderDes;

	void ActionTraderPlusMenu()
	{
		m_TraderName="";
		m_TraderDes="";
		TraderID = -1;
		TraderPos = Vector(0,0,0);
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
		if (GetGame().IsServer())return "#tpm_trade";
		else return "#tpm_trade" + " " + "- " + m_TraderName +" (" +m_TraderDes + ")";
	}

	override bool ActionCondition( PlayerBase player, ActionTarget target, ItemBase item )
    {
        if ( GetGame().IsServer() )
            return true;

        PlayerBase ntarget = PlayerBase.Cast( target.GetObject() );
				//Print("ID Target:"+ntarget.TraderID.ToString()+"ntarget.MaxPlayerPerID:"+ntarget.MaxPlayerPerID.ToString());
				if(ntarget && ntarget.TraderID >= 0 && AddAditionalCondition(player, target, item))
				{
					TraderID = ntarget.TraderID;
					TraderPos = ntarget.GetPosition();
					GetTraderNameAndRole();
					return true;
				}

				BuildingBase nObject = BuildingBase.Cast( target.GetObject() );
				if (nObject && nObject.TraderID >= 0 && AddAditionalCondition(player, target, item))
				{
					TraderID = nObject.TraderID;
					TraderPos = nObject.GetPosition();
					GetTraderNameAndRole();
					return true;
				}
				return false;
    }

		void GetTraderNameAndRole()
		{
			for(int i=0;i<GetTraderPlusConfigClient().TraderPos.Count();i++)
			{
				vector traderpos = GetTraderPlusConfigClient().TraderPos.Get(i);
				float  distance  = vector.Distance(traderpos,TraderPos);
				if(distance < 1 )
				{
					m_TraderName = GetTraderPlusConfigClient().GivenNames.Get(i);
					m_TraderDes = GetTraderPlusConfigClient().Roles.Get(i);
				}
			}
		}

		bool AddAditionalCondition(PlayerBase player, ActionTarget target, ItemBase item)
		{
			return true;
		}

    override void OnStartClient(ActionData action_data)
    {
        OpenTraderMenu(action_data.m_Target.GetObject());
    }

	void OpenTraderMenu(Object obj)
	{
		if (g_Game.GetUIManager().GetMenu() == NULL)
		{
			if (GetTraderPlusUIService())
			{
				GetTraderPlusUIService().OpenTraderView(TraderID, TraderPos, obj);
			}
		}
	}
}
