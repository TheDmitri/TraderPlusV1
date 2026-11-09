#ifndef CARLOCKDISABLE
modded class ActionDetach
{
    override bool ActionCondition( PlayerBase player, ActionTarget target, ItemBase item )
    {
        if (target && target.GetObject() && target.GetObject().IsInherited(CarWheel))
        {
            CarScript car = CarScript.Cast(target.GetParent());
            if (car && car.m_CarLock_IsLocked)
                return false;
        }
        return super.ActionCondition(player, target, item);
    }
}
#endif
