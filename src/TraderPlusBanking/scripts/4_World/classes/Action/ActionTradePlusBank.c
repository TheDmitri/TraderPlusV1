
class ActionTraderPlusBankingMenu: ActionInteractBase
{
	private int TraderID;
	private vector TraderPos;

	void ActionTraderPlusBankingMenu()
	{
		TraderID = -1;
		TraderPos = Vector(0,0,0);
		m_CommandUID = DayZPlayerConstants.CMD_ACTIONMOD_INTERACTONCE;
		m_StanceMask = DayZPlayerConstants.STANCEMASK_ERECT | DayZPlayerConstants.STANCEMASK_CROUCH;
		m_HUDCursorIcon = CursorIcons.CloseHood;
	}

    override void CreateConditionComponents()
		{
			m_ConditionTarget = new CCTObject(10);//CCTMan(10);
			m_ConditionItem = new CCINone;
		}

	override string GetText()
	{
		return "#tpm_bank_menu";
	}

	override bool ActionCondition( PlayerBase player, ActionTarget target, ItemBase item )
    {
        if ( GetGame().IsServer() )
            return true;

				ItemBase item_in_hands = ItemBase.Cast( player.GetHumanInventory().GetEntityInHands() );

        PlayerBase ntarget = PlayerBase.Cast( target.GetObject() );
				if(ntarget && ntarget.TraderID ==-2)
				{
					if((GetTraderPlusBankingConfigClient().IsCreditCarNeededForTransaction && !item_in_hands) || (GetTraderPlusBankingConfigClient().IsCreditCarNeededForTransaction && item_in_hands && !item_in_hands.IsKindOf("TraderPlusCreditCard")))
						return false;

					return true;
				}

				BuildingBase nObject = BuildingBase.Cast( target.GetObject() );
				if (nObject && nObject.TraderID ==-2)
				{
					if((GetTraderPlusBankingConfigClient().IsCreditCarNeededForTransaction && !item_in_hands) || (GetTraderPlusBankingConfigClient().IsCreditCarNeededForTransaction && item_in_hands && !item_in_hands.IsKindOf("TraderPlusCreditCard")))
						return false;
					
					return true;
				}
				return false;
    }

		bool AdditionalCondition(PlayerBase player, ActionTarget target, ItemBase item)
		{
			return true;
		}

    override void OnStartClient(ActionData action_data)
    {
        OpenTraderBankMenu();
    }

	void InitTraderBankMenu(PlayerBase player)
	{
		player.m_TraderPlusBankingMenu = new TraderPlusBankingMenu;
		player.m_TraderPlusBankingMenu.Init();
	}

	void OpenTraderBankMenu()
	{
      PlayerBase player = PlayerBase.Cast(GetGame().GetPlayer());
			if ( g_Game.GetUIManager().GetMenu() == NULL )
			{
				GetRPCManager().SendRPC("TraderPlusBanking", "TraderPlusBankingRequest",  NULL, true, NULL);
				InitTraderBankMenu(player);
				GetGame().GetUIManager().ShowScriptedMenu( player.m_TraderPlusBankingMenu, NULL );
			}
	}
}
