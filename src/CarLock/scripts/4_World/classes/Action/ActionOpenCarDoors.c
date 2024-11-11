#ifndef CARLOCKDISABLE
modded class ActionCarDoors: ActionInteractBase
{
    override bool ActionCondition( PlayerBase player, ActionTarget target, ItemBase item )
	   {
        CarScript carScript;
        if (Class.CastTo(carScript, target.GetParent()))
        {
            if (carScript.m_CarLock_IsLocked)
            {
                return false;
            }
        }

		    return super.ActionCondition(player, target, item);
	 }
}

modded class ActionCarDoorsOutside: ActionInteractBase
{
    override bool ActionCondition( PlayerBase player, ActionTarget target, ItemBase item )
	{
        CarScript carScript;
        if (Class.CastTo(carScript, target.GetParent()))
        {
            if (carScript.m_CarLock_IsLocked)
            {
                return false;
            }
        }

		return super.ActionCondition(player, target, item);
	}
}
#endif
/*modded class ActionCarDoors: ActionInteractBase
{

  override bool ActionCondition( PlayerBase player, ActionTarget target, ItemBase item )
   {
      CarScript carScript;
      if (Class.CastTo(carScript, target.GetParent()))
      {
          GetTraderPlusLogger().LogInfo("carScript.m_CarLock_IsLocked:"+carScript.m_CarLock_IsLocked.ToString());
          if (carScript.m_CarLock_IsLocked && !player.IsInVehicle())
          {
              return false;
          }
      }
      return super.ActionCondition(player, target, item);
   }
};*/
