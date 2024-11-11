#ifndef CARLOCKDISABLE
class ActionAttachCarLockCB : ActionContinuousBaseCB
{
	override void CreateActionComponent()
	{
		m_ActionData.m_ActionComponent = new CAContinuousTime(15);
	}
};

class ActionAttachCarLock: ActionContinuousBase
{
	void ActionAttachCarLock()
	{
		m_CallbackClass = ActionAttachCarLockCB;
		m_CommandUID = DayZPlayerConstants.CMD_ACTIONFB_CRAFTING;
		m_FullBody = true;
		m_StanceMask = DayZPlayerConstants.STANCEMASK_CROUCH;
		m_SpecialtyWeight = UASoftSkillsWeight.PRECISE_LOW;
	}

	override void CreateConditionComponents()
	{
    m_ConditionTarget = new CCTNonRuined(10);
		m_ConditionItem = new CCINonRuined;
	}

	override string GetText()
	{
		return "Attach CarLock";
	}

	override bool ActionCondition( PlayerBase player, ActionTarget target, ItemBase item )
	{
		if ( GetGame().IsServer() )
				return true;

		CarScript ntarget = CarScript.Cast( target.GetObject() );
		if (ntarget && ntarget.m_CarLockOwner==-1)
		{
			bool IsEmpty = true;
			Transport transport = Transport.Cast(target.GetObject());
			if (!transport)return false;

			int crewSize = transport.CrewSize();
			for (int j = 0; j < crewSize; j++)
			{
				if (transport.CrewMember(j))
					IsEmpty = false;
			}

			if (IsEmpty)return true;
		}
		return false;
	}

	override void OnFinishProgressClient( ActionData action_data )
	{
    if ( action_data.m_MainItem && action_data.m_MainItem.GetHierarchyRootPlayer() == action_data.m_Player )
		{
      CarScript vehicle = CarScript.Cast(action_data.m_Target.GetObject());
      OpenCarLockMenu(vehicle);
			//GetRPCManager().SendRPC("CarLock", "CarLockPasswordRequest",  new Param2<ref CarScript, int>(vehicle,-1), true, NULL);
		}
  }

	override void OnFinishProgressServer( ActionData action_data )
	{
    if ( action_data.m_MainItem && action_data.m_MainItem.GetHierarchyRootPlayer() == action_data.m_Player )
		{
			action_data.m_Player.DropItem(action_data.m_MainItem);
      action_data.m_MainItem.Delete();

			CarScript vehicle = CarScript.Cast(action_data.m_Target.GetObject());
			vehicle.SetCarLockOwner(action_data.m_Player.CLSteamlowID);
			vehicle.SetCarLock(true);
		}
  }

	void InitCarLockMenu(PlayerBase player, CarScript vehicle)
	{
		player.m_CarLockMenu = new CarLockMenu(vehicle);
		player.m_CarLockMenu.Init();
	}

	void OpenCarLockMenu(CarScript vehicle)
	{
			PlayerBase player = PlayerBase.Cast(GetGame().GetPlayer());
			if ( g_Game.GetUIManager().GetMenu() == NULL )
			{
				InitCarLockMenu(player,vehicle);
				GetGame().GetUIManager().ShowScriptedMenu( player.m_CarLockMenu, NULL );
			}
	}
};
#endif
