#ifndef CARLOCKDISABLE
class  ActionDetachCarLockCB : ActionContinuousBaseCB
{
	override void CreateActionComponent()
	{
		m_ActionData.m_ActionComponent = new CAContinuousTime(15);
	}
};

class  ActionDetachCarLock: ActionContinuousBase
{
	void  ActionDetachCarLock()
	{
		m_CallbackClass =  ActionDetachCarLockCB;
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
		return "Detach CarLock";
	}

	override bool ActionCondition( PlayerBase player, ActionTarget target, ItemBase item )
	{
		if ( GetGame().IsServer() )
				return true;

		CarScript ntarget = CarScript.Cast( target.GetObject() );
		if (ntarget && (ntarget.m_CarLockOwner == player.CLSteamlowID && ntarget.m_CarLockPassword != -1 ) || player.GetAdminStatus() == SZ_IS_ADMIN ||player.HasPassword(ntarget.m_CarLockPassword,ntarget.m_CarLockOwner))
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

	/*override void OnFinishProgressClient( ActionData action_data )
	{
    if ( action_data.m_MainItem && action_data.m_MainItem.IsKindOf("CarLockPick") && action_data.m_MainItem.GetHierarchyRootPlayer() == action_data.m_Player )
		{
			CarScript car = CarScript.Cast(action_data.m_Target.GetObject());
			if(car)
			{
				car.PlayCarLockAlarm();
			}
		}
  }*/

	override void OnFinishProgressServer( ActionData action_data )
	{
    if ( action_data.m_MainItem && action_data.m_MainItem.GetHierarchyRootPlayer() == action_data.m_Player )
		{
			CarScript car = CarScript.Cast(action_data.m_Target.GetObject());
			if(car)
			{
				car.SetCarLockPassword(-1);
				car.SetCarLockOwner(-1);
				car.SetCarLock(false);
				ItemBase carlock = ItemBase.Cast( GetGame().CreateObjectEx("CarLock", action_data.m_Player.GetPosition(), ECE_PLACE_ON_SURFACE) );
				action_data.m_Player.DropItem(action_data.m_MainItem);
	      action_data.m_MainItem.Delete();
				GetTraderPlusLogger().LogInfo("CARLOCK HAS BEEN DETACHED BY:"+action_data.m_Player.GetIdentity().GetName());
			}
		}
  }
};
#endif
