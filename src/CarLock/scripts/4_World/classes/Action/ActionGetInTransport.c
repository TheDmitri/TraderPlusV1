#ifndef CARLOCKDISABLE
modded class ActionGetInTransport: ActionBase
{
	override bool ActionCondition( PlayerBase player, ActionTarget target, ItemBase item )
	{
    CarScript carScript;
    if (Class.CastTo(carScript, target.GetObject()))
    {
        if (carScript.m_CarLock_IsLocked)
        {
            return false;
        }
    }

    return super.ActionCondition(player, target, item);
	}
};
#endif
