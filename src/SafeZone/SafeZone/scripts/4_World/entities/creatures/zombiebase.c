modded class ZombieBase extends DayZInfected
{
	bool DeleteZombieHandler()
	{
		PlayerBase m_player = PlayerBase.Cast(m_ActualTarget);
    if(m_player && m_player.GetSafeZoneStatus() == SZ_IN_SAFEZONE)
    {
      this.Delete();
      return true;
    }

		return false;
	}

	override bool ChaseAttackLogic(int pCurrentCommandID, DayZInfectedInputController pInputController, float pDt)
	{
    if(DeleteZombieHandler())
			return false;

    return super.ChaseAttackLogic(pCurrentCommandID,pInputController,pDt);
	}

	override bool FightAttackLogic(int pCurrentCommandID, DayZInfectedInputController pInputController, float pDt)
	{
		if(DeleteZombieHandler())
			return false;

		return super.FightAttackLogic(pCurrentCommandID,pInputController,pDt);
	}
}
