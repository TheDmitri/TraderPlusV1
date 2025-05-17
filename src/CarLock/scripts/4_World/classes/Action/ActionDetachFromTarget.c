#ifndef CARLOCKDISABLE
modded class ActionDetachFromTarget: ActionInteractBase
{
	override bool ActionCondition( PlayerBase player, ActionTarget target, ItemBase item )
	{
		if (target)
		{
            CarScript ntarget = CarLockTargetHelper.GetTargetCar(target);
			if(!ntarget)
				ntarget = CarScript.Cast(target.GetParent());
			
			if (ntarget && ntarget.m_CarLock_IsLocked)
				return false;
		}

    return super.ActionCondition(player, target, item);
	}
};
#endif
