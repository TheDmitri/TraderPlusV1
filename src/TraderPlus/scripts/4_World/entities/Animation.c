modded class ModItemRegisterCallbacks
{
	override void RegisterOneHanded( DayZPlayerType pType, DayzPlayerItemBehaviorCfg pBehavior )
	  {
			super.RegisterOneHanded( pType, pBehavior );
			pType.AddItemInHandsProfileIK("Money_Base", "dz/anims/workspaces/player/player_main/props/player_main_1h_compass.asi", pBehavior, 			"dz/anims/anm/player/ik/gear/compass.anm");
      pType.AddItemInHandsProfileIK("Coin_Base", "dz/anims/workspaces/player/player_main/props/player_main_1h_compass.asi", pBehavior, 			"dz/anims/anm/player/ik/gear/compass.anm");
			pType.AddItemInHandsProfileIK("TraderPlus_Wallet", "dz/anims/workspaces/player/player_main/player_main_1h.asi", pBehavior, 			"dz/anims/anm/player/ik/gear/Bark_oak.anm");
		}
}
