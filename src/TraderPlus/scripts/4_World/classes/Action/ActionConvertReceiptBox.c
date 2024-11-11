class ActionConvertToNewReceiptCB : ActionContinuousBaseCB
{
	override void CreateActionComponent()
	{
		m_ActionData.m_ActionComponent = new CAContinuousTime(UATimeSpent.UNPACK);
	}
};

class ActionConvertToNewReceipt: ActionContinuousBase
{
	void ActionConvertToNewReceipt()
	{
		m_CallbackClass = ActionConvertToNewReceiptCB;
		m_CommandUID = DayZPlayerConstants.CMD_ACTIONMOD_OPENITEM;
		m_CommandUIDProne = DayZPlayerConstants.CMD_ACTIONFB_OPENITEM;
		m_SpecialtyWeight = UASoftSkillsWeight.PRECISE_LOW;
	}

	override void CreateConditionComponents()
	{
		m_ConditionItem = new CCINonRuined;
		m_ConditionTarget = new CCTNone;
	}

	override bool HasProneException()
	{
		return true;
	}

	override bool HasTarget()
	{
		return false;
	}

	override string GetText()
	{
		return "CONVERT TO NEW RECEIPT";
	}

	override void OnFinishProgressServer( ActionData action_data )
	{
		if ( action_data.m_MainItem && action_data.m_MainItem.GetHierarchyRootPlayer() == action_data.m_Player )
		{
			Receipt oldReceipt = Receipt.Cast(action_data.m_MainItem);
			NewReceipt receipt = NewReceipt.Cast(GetGame().CreateObject("NewReceipt", action_data.m_Player.GetPosition(), false, false, true));
			if(GetTraderPlusVehiculeConfig().VehiclesParts && GetTraderPlusVehiculeConfig().VehiclesParts.Get(oldReceipt.CarID))
				receipt.CarClassName = GetTraderPlusVehiculeConfig().VehiclesParts.Get(oldReceipt.CarID).VehicleName;
			receipt.Attachments = TraderPlusHelper.GetVehicleAttachmentsFromConfig(receipt.CarClassName);

			receipt.GetInventory().LockInventory(HIDE_INV_FROM_SCRIPT);
			receipt.SetSynchDirty();

			GetGame().ObjectDelete(action_data.m_MainItem);
		}
	}
};
