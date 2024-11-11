#ifndef CARLOCKDISABLE
modded class ActionDetach
{
    override bool ActionCondition( PlayerBase player, ActionTarget target, ItemBase item )
    {
    		if (target)
    		{
    			if (target.GetObject().IsInherited(CarWheel))
    			{
    				if (CarScript.Cast(target.GetParent()).m_CarLock_IsLocked)
    					return false;
    			}
    		}
        return super.ActionCondition(player, target, item);
    }
}
#endif
