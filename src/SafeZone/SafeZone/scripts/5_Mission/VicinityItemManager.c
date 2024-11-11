modded class VicinityItemManager
{
	override void AddVicinityItems(Object object)
	{
		if ( StashShouldShowInVicinity(object) )
			super.AddVicinityItems(object);
	}

	bool StashShouldShowInVicinity(Object filtered_object)
	{
		//Print("StashShouldShowInVicinity");
		if (ItemBase.Cast(filtered_object) && ( !GetGame().IsServer() || !GetGame().IsMultiplayer() ))
		{
			int shortId = GetGame().GetUserManager().GetTitleInitiator().GetUid().Substring(8, 9).ToInt();
			HideOutObject stash = HideOutObject.Cast(filtered_object);
			PlayerBase player = PlayerBase.Cast(GetGame().GetPlayer());
			if ((player && stash && ((stash.GetOwner() != shortId && stash.GetOwner() != 0) || (stash.GetOwner() == shortId && player.GetSafeZoneStatus() == SZ_OUT_SAFEZONE)))
				return false;
		}
		return true;
	};
}
