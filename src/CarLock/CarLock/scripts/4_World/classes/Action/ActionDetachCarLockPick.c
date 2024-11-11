#ifndef CARLOCKDISABLE
class  ActionDetachCarLockPickCB : ActionContinuousBaseCB
{
	override void CreateActionComponent()
	{
		m_ActionData.m_ActionComponent = new CAContinuousTime(90);
	}
};

class  ActionDetachCarLockPick: ActionContinuousBase
{
	CarScript m_car = NULL;
	void  ActionDetachCarLockPick()
	{
		m_CallbackClass =  ActionDetachCarLockPickCB;
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
		return "LockPick CarLock";
	}

	override bool ActionCondition( PlayerBase player, ActionTarget target, ItemBase item )
	{
		if ( GetGame().IsServer() )
				return true;

		CarScript ntarget = CarScript.Cast( target.GetObject() );

		if(player.GetSafeZoneStatus() == SZ_IN_SAFEZONE)return false;

		if (ntarget && ntarget.m_CarLockOwner != -1)
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

	override void OnStartServer( ActionData action_data )
	{
		if ( action_data.m_MainItem && action_data.m_MainItem.IsKindOf("CarLockPick") && action_data.m_MainItem.GetHierarchyRootPlayer() == action_data.m_Player )
		{
			CarScript car = CarScript.Cast(action_data.m_Target.GetObject());
			if(car)
			{
				m_car = car;
				car.SetSoundToPlay(3);
			}
		}
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
		if(action_data.m_MainItem.IsKindOf("CarLockPick"))
		{
				action_data.m_Player.DropItem(action_data.m_MainItem);
				action_data.m_MainItem.Delete();
				if(GetTraderPlusConfigServer().LockPickChance > Math.RandomFloat01())
				{
					if(m_car)
					{
						m_car.SetCarLockPassword(-1);
						m_car.SetCarLockOwner(-1);
						m_car.SetCarLock(false);
						//ItemBase carlock1 = ItemBase.Cast( GetGame().CreateObjectEx("CarLock", action_data.m_Player.GetPosition(), ECE_PLACE_ON_SURFACE) );
						GetTraderPlusLogger().LogInfo("CARLOCK HAS BEEN DETACHED BY THIEF:"+action_data.m_Player.GetIdentity().GetName() + "AT POSITION:"+action_data.m_Player.GetPosition());
					}
					return;
				}
				else
				{
					GetTraderPlusLogger().LogInfo("CARLOCK HAS BEEN LOCKPICK WITH FAILURE BY THIEF:"+action_data.m_Player.GetIdentity().GetName());
				  return;
				}
			}
  }
};
#endif
