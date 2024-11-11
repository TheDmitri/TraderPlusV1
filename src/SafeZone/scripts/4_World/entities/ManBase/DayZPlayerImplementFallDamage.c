modded class DayZPlayerImplementFallDamage
{
	override void HandleFallDamage(float pHeight)
	{
		if(SZHandleFallDamageHandle(pHeight))
			return;

		super.HandleFallDamage(pHeight);
	}

	bool SZHandleFallDamageHandle(float pHeight)
	{
		if (GetGame().IsServer())
		{
			PlayerBase player = PlayerBase.Cast(m_Player);
			if (player && player.GetSafeZoneStatus() == SZ_IN_SAFEZONE)
				return true;

			return false;
		}

		return false;
	}
}
