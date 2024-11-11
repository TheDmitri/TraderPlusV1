modded class DayZPlayerMeleeFightLogic_LightHeavy
{
    override bool HandleFightLogic(int pCurrentCommandID, HumanInputController pInputs, EntityAI pEntityInHands, HumanMovementState pMovementState, out bool pContinueAttack)
	  {
        if (SZHandleFightLogic(pCurrentCommandID, pInputs, pEntityInHands, pMovementState, pContinueAttack))
            return false;

        return super.HandleFightLogic(pCurrentCommandID, pInputs, pEntityInHands, pMovementState, pContinueAttack);
    }

    bool SZHandleFightLogic(int pCurrentCommandID, HumanInputController pInputs, EntityAI pEntityInHands, HumanMovementState pMovementState, out bool pContinueAttack)
    {
      PlayerBase playerSZClone = PlayerBase.Cast(m_DZPlayer);

      if (playerSZClone && playerSZClone.GetSafeZoneStatus() == SZ_IN_SAFEZONE)
          return true;

      return false;
    }
}
