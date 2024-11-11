#ifndef CARLOCKDISABLE
class ActionCarUnLock: ActionContinuousBase
{
	void ActionCarUnLock()
	{
		m_CallbackClass =  ActionCarLockCB;
		m_CommandUID = DayZPlayerConstants.CMD_ACTIONMOD_INTERACTONCE;
		m_StanceMask = DayZPlayerConstants.STANCEMASK_ERECT | DayZPlayerConstants.STANCEMASK_CROUCH;
	}

  override void CreateConditionComponents()
	{
		m_ConditionTarget = new CCTObject(7);//CCTMan(10);
		m_ConditionItem = new CCINone;
	}

	override string GetText()
	{
		return "UnLock Car";
	}

	override bool ActionCondition( PlayerBase player, ActionTarget target, ItemBase item )
    {
        if ( GetGame().IsServer() )
            return true;

        CarScript ntarget = CarScript.Cast( target.GetObject() );
	      //GetTraderPlusLogger().LogInfo("ActionLock: islocked:"+ntarget.m_CarLock_IsLocked.ToString()+" car_owner:"+ntarget.m_CarLockOwner.ToString()); //sender.GetName() = player.GetIdentity().GetName()
        if (ntarget && ntarget.m_CarLockPassword != -1 && ntarget.m_CarLock_IsLocked && ntarget.m_CarLockOwner !=-1 && (ntarget.m_CarLockOwner == player.CLSteamlowID || player.GetAdminStatus() == SZ_IS_ADMIN ||player.HasPassword(ntarget.m_CarLockPassword,ntarget.m_CarLockOwner)))
						return true;
				
				return false;
    }

    override void OnStartServer(ActionData action_data)
    {
			CarScript car = CarScript.Cast(action_data.m_Target.GetObject());
			if(car)
			{
				car.SetSoundToPlay(2);
				car.SetCarLock(false);
			}
		}
}
#endif
