modded class ActionInjectTarget
{
	bool TPAdditionalCondition()
	{
		return true;
	}

	bool SZActionConditionHandler(PlayerBase player, ActionTarget target, ItemBase item)
	{
		if (player.GetSafeZoneStatus() == SZ_IN_SAFEZONE && player.GetAdminStatus() != SZ_IS_ADMIN && TPAdditionalCondition())
	       return true;

		return false;
	}

	override bool ActionCondition( PlayerBase player, ActionTarget target, ItemBase item )
	{
		if(SZActionConditionHandler(player, target, item))
			return false;

		return super.ActionCondition(player, target, item);
	}
}

modded class ActionDisinfectTarget
{
	bool TPAdditionalCondition()
	{
		return true;
	}

	bool SZActionConditionHandler(PlayerBase player, ActionTarget target, ItemBase item)
	{
		if (player.GetSafeZoneStatus() == SZ_IN_SAFEZONE && player.GetAdminStatus() != SZ_IS_ADMIN && TPAdditionalCondition())
	       return true;
		return false;
	}

	override bool ActionCondition( PlayerBase player, ActionTarget target, ItemBase item )
	{
		if(SZActionConditionHandler(player, target, item))
			return false;

		return super.ActionCondition(player, target, item);
	}
}

modded class ActionForceConsumeSingle
{
	bool TPAdditionalCondition()
	{
		return true;
	}

	bool SZActionConditionHandler(PlayerBase player, ActionTarget target, ItemBase item)
	{
		if (player.GetSafeZoneStatus() == SZ_IN_SAFEZONE && player.GetAdminStatus() != SZ_IS_ADMIN && TPAdditionalCondition())
				 return true;
		return false;
	}

	override bool ActionCondition( PlayerBase player, ActionTarget target, ItemBase item )
	{
		if(SZActionConditionHandler(player, target, item))
			return false;

		return super.ActionCondition(player, target, item);
	}
}

modded class ActionForceConsume
{
	bool TPAdditionalCondition()
	{
		return true;
	}

	bool SZActionConditionHandler(PlayerBase player, ActionTarget target, ItemBase item)
	{
		if (player.GetSafeZoneStatus() == SZ_IN_SAFEZONE && player.GetAdminStatus() != SZ_IS_ADMIN && TPAdditionalCondition())
				 return true;
		return false;
	}

	override bool ActionCondition( PlayerBase player, ActionTarget target, ItemBase item )
	{
		if(SZActionConditionHandler(player, target, item))
			return false;

		return super.ActionCondition(player, target, item);
	}
}

modded class ActionPackTent
{
	bool TPAdditionalCondition()
	{
		return true;
	}

	bool SZActionConditionHandler(PlayerBase player, ActionTarget target, ItemBase item)
	{
		if (player.GetSafeZoneStatus() == SZ_IN_SAFEZONE && player.GetAdminStatus() != SZ_IS_ADMIN && TPAdditionalCondition())
				 return true;
		return false;
	}

	override bool ActionCondition( PlayerBase player, ActionTarget target, ItemBase item )
	{
		if(SZActionConditionHandler(player, target, item))
			return false;

		return super.ActionCondition(player, target, item);
	}
};

modded class ActionGiveBloodTarget
{
	bool TPAdditionalCondition()
	{
		return true;
	}

	bool SZActionConditionHandler(PlayerBase player, ActionTarget target, ItemBase item)
	{
		if (player.GetSafeZoneStatus() == SZ_IN_SAFEZONE && player.GetAdminStatus() != SZ_IS_ADMIN && TPAdditionalCondition())
				 return true;
		return false;
	}

	override bool ActionCondition( PlayerBase player, ActionTarget target, ItemBase item )
	{
		if(SZActionConditionHandler(player, target, item))
			return false;

		return super.ActionCondition(player, target, item);
	}
}

modded class ActionCollectBloodTarget
{
	bool TPAdditionalCondition()
	{
		return true;
	}

	bool SZActionConditionHandler(PlayerBase player, ActionTarget target, ItemBase item)
	{
		if (player.GetSafeZoneStatus() == SZ_IN_SAFEZONE && player.GetAdminStatus() != SZ_IS_ADMIN && TPAdditionalCondition())
				 return true;
		return false;
	}

	override bool ActionCondition( PlayerBase player, ActionTarget target, ItemBase item )
	{
		if(SZActionConditionHandler(player, target, item))
			return false;

		return super.ActionCondition(player, target, item);
	}
}

modded class ActionBurnSewTarget
{
	bool TPAdditionalCondition()
	{
		return true;
	}

	bool SZActionConditionHandler(PlayerBase player, ActionTarget target, ItemBase item)
	{
		if (player.GetSafeZoneStatus() == SZ_IN_SAFEZONE && player.GetAdminStatus() != SZ_IS_ADMIN && TPAdditionalCondition())
				 return true;
		return false;
	}

	override bool ActionCondition( PlayerBase player, ActionTarget target, ItemBase item )
	{
		if(SZActionConditionHandler(player, target, item))
			return false;

		return super.ActionCondition(player, target, item);
	}
}

modded class ActionDeployObject
{
	bool TPAdditionalCondition()
	{
		return true;
	}

	bool SZActionConditionHandler(PlayerBase player, ActionTarget target, ItemBase item)
	{
		if (player.GetSafeZoneStatus() == SZ_IN_SAFEZONE && player.GetAdminStatus() != SZ_IS_ADMIN && TPAdditionalCondition())
				 return true;
		return false;
	}

	override bool ActionCondition( PlayerBase player, ActionTarget target, ItemBase item )
	{
		if(SZActionConditionHandler(player, target, item))
			return false;

		return super.ActionCondition(player, target, item);
	}
};

modded class ActionRestrainSelf
{
	bool TPAdditionalCondition()
	{
		return true;
	}

	bool SZActionConditionHandler(PlayerBase player, ActionTarget target, ItemBase item)
	{
		if (player.GetSafeZoneStatus() == SZ_IN_SAFEZONE && player.GetAdminStatus() != SZ_IS_ADMIN && TPAdditionalCondition())
				 return true;
		return false;
	}

	override bool ActionCondition( PlayerBase player, ActionTarget target, ItemBase item )
	{
		if(SZActionConditionHandler(player, target, item))
			return false;

		return super.ActionCondition(player, target, item);
	}
}

modded class ActionRestrainTarget
{
	bool TPAdditionalCondition()
	{
		return true;
	}

	bool SZActionConditionHandler(PlayerBase player, ActionTarget target, ItemBase item)
	{
		if (player.GetSafeZoneStatus() == SZ_IN_SAFEZONE && player.GetAdminStatus() != SZ_IS_ADMIN && TPAdditionalCondition())
				 return true;
		return false;
	}

	override bool ActionCondition( PlayerBase player, ActionTarget target, ItemBase item )
	{
		if(SZActionConditionHandler(player, target, item))
			return false;

		return super.ActionCondition(player, target, item);
	}
}

modded class ActionForceFeedCan
{
	override bool TPAdditionalCondition()
	{
		return true;
	}

	override bool SZActionConditionHandler(PlayerBase player, ActionTarget target, ItemBase item)
	{
		if (player.GetSafeZoneStatus() == SZ_IN_SAFEZONE && player.GetAdminStatus() != SZ_IS_ADMIN && TPAdditionalCondition())
				 return true;
		return false;
	}

	override bool ActionCondition( PlayerBase player, ActionTarget target, ItemBase item )
	{
		if(SZActionConditionHandler(player, target, item))
			return false;

		return super.ActionCondition(player, target, item);
	}
}

modded class ActionLockDoors
{
		bool TPAdditionalCondition()
		{
			return true;
		}

		bool SZActionConditionHandler(PlayerBase player, ActionTarget target, ItemBase item)
		{
			if (player.GetSafeZoneStatus() == SZ_IN_SAFEZONE && player.GetAdminStatus() != SZ_IS_ADMIN && TPAdditionalCondition())
					 return true;
			return false;
		}

		override bool ActionCondition( PlayerBase player, ActionTarget target, ItemBase item )
		{
			if(SZActionConditionHandler(player, target, item))
				return false;

			return super.ActionCondition(player, target, item);
		}
}

modded class ActionUnpin extends ActionSingleUseBase
{
	bool TPAdditionalCondition()
	{
		return true;
	}

	bool SZActionConditionHandler(PlayerBase player, ActionTarget target, ItemBase item)
	{
		if (player.GetSafeZoneStatus() == SZ_IN_SAFEZONE && player.GetAdminStatus() != SZ_IS_ADMIN && TPAdditionalCondition())
				 return true;
		return false;
	}

	override bool ActionCondition( PlayerBase player, ActionTarget target, ItemBase item )
	{
		if(SZActionConditionHandler(player, target, item))
			return false;

		return super.ActionCondition(player, target, item);
	}
}

modded class ActionDigInStash: ActionContinuousBase
{
	bool TPAdditionalCondition()
	{
		return true;
	}

	bool SZActionConditionHandler(PlayerBase player, ActionTarget target, ItemBase item)
	{
		if (player.GetSafeZoneStatus() == SZ_IN_SAFEZONE && player.GetAdminStatus() != SZ_IS_ADMIN && TPAdditionalCondition())
				 return true;
		return false;
	}

	override bool ActionCondition( PlayerBase player, ActionTarget target, ItemBase item )
	{
		if(SZActionConditionHandler(player, target, item))
			return false;

		return super.ActionCondition(player, target, item);
	}
}

modded class ActionDigOutStash: ActionContinuousBase
{
	bool TPAdditionalCondition()
	{
		return true;
	}

	bool SZActionConditionHandler(PlayerBase player, ActionTarget target, ItemBase item)
	{
		if (player.GetSafeZoneStatus() == SZ_IN_SAFEZONE && player.GetAdminStatus() != SZ_IS_ADMIN && TPAdditionalCondition())
				 return true;
		return false;
	}

	override bool ActionCondition( PlayerBase player, ActionTarget target, ItemBase item )
	{
		if(SZActionConditionHandler(player, target, item))
			return false;

		return super.ActionCondition(player, target, item);
	}
}

modded class ActionMineTree: ActionContinuousBase
{
	bool TPAdditionalCondition()
	{
		return true;
	}

	bool SZActionConditionHandler(PlayerBase player, ActionTarget target, ItemBase item)
	{
		if (player.GetSafeZoneStatus() == SZ_IN_SAFEZONE && player.GetAdminStatus() != SZ_IS_ADMIN && TPAdditionalCondition())
				 return true;
		return false;
	}

	override bool ActionCondition( PlayerBase player, ActionTarget target, ItemBase item )
	{
		if(SZActionConditionHandler(player, target, item))
			return false;

		return super.ActionCondition(player, target, item);
	}
}

#ifdef MuchCarKey
modded class ActionPickLockOnCar : ActionLockUnlockCar
{
	bool TPAdditionalCondition()
	{
		return true;
	}

	bool SZActionConditionHandler(PlayerBase player, ActionTarget target, ItemBase item)
	{
		if (player.GetSafeZoneStatus() == SZ_IN_SAFEZONE && player.GetAdminStatus() != SZ_IS_ADMIN && TPAdditionalCondition())
				 return true;
		return false;
	}

	override bool ActionCondition( PlayerBase player, ActionTarget target, ItemBase item )
	{
		if(SZActionConditionHandler(player, target, item))
			return false;

		return super.ActionCondition(player, target, item);
	}
};
#endif

modded class ActionTakeItem: ActionInteractBase
{
	override bool ActionCondition( PlayerBase player, ActionTarget target, ItemBase item )
	{
		if(!GetGame().IsServer())
		{
			HideOutObject stash = HideOutObject.Cast( target.GetObject() );
			int shortId = GetGame().GetUserManager().GetTitleInitiator().GetUid().Substring(8, 9).ToInt();
			if(stash && (player.GetSafeZoneStatus() == SZ_OUT_SAFEZONE || player.GetSafeZoneStatus() == SZ_IN_SAFEZONE && stash.GetOwner() != shortId))
				return false;
		}

		return super.ActionCondition(player,target,item);
	}
};
