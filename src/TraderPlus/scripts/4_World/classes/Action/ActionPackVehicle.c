
class ActionPackVehicleCB : ActionContinuousBaseCB
{
	override void CreateActionComponent()
	{
		#ifndef CARLOCKDISABLE
		m_ActionData.m_ActionComponent = new CAContinuousTime(15);
		#else
		m_ActionData.m_ActionComponent = new CAContinuousTime(30);
		#endif
	}
};

class ActionPackVehicle: ActionContinuousBase
{
	void ActionPackVehicle()
	{
		m_CallbackClass = ActionPackVehicleCB;
		m_CommandUID = DayZPlayerConstants.CMD_ACTIONFB_CRAFTING;
		m_FullBody = true;
		m_StanceMask = DayZPlayerConstants.STANCEMASK_CROUCH;
		m_SpecialtyWeight = UASoftSkillsWeight.PRECISE_LOW;
	}

	override void CreateConditionComponents()
	{
    m_ConditionTarget = new CCTNonRuined(10);
		m_ConditionItem = new CCINonRuined;
	}

	override string GetText()
	{
		return "#tpm_pack_vehicle";
	}

	override bool ActionCondition( PlayerBase player, ActionTarget target, ItemBase item )
	{
		if ( GetGame().IsServer() )
				return true;

		CarScript ntarget = CarScript.Cast( target.GetObject() );
		#ifndef CARLOCKDISABLE
			if (ConditionAddon(player, target, item) || ntarget && (GetTraderPlusConfigClient().IsReceiptSaveCargo || !GetTraderPlusConfigClient().IsReceiptSaveCargo && ntarget.GetInventory().GetCargo().GetItemCount() == 0) &&(TraderPlusHelper.IsOwnerOftheInsurance(player, ntarget.GetCarUniqueId()) || ntarget.GetCarUniqueId() == 0) && (player.GetSafeZoneStatus() == SZ_IN_SAFEZONE || !GetTraderPlusConfigClient().IsReceiptTraderOnly) && (ntarget.m_CarLockOwner == player.CLSteamlowID || (player.HasPassword(ntarget.m_CarLockPassword,ntarget.m_CarLockOwner) && ntarget.GetCarUniqueId() == 0)))
			{
				bool IsEmpty = true;
				Transport transport = Transport.Cast(target.GetObject());
				if (!transport)return false;

				int crewSize = transport.CrewSize();
				for (int j = 0; j < crewSize; j++)
				{
					if (transport.CrewMember(j))
						IsEmpty = false;
				}

				if (IsEmpty)return true;
			}
		#else
			#ifdef MuchCarKey
				if (ConditionAddon(player, target, item) || ntarget && (GetTraderPlusConfigClient().IsReceiptSaveCargo || !GetTraderPlusConfigClient().IsReceiptSaveCargo && ntarget.GetInventory().GetCargo().GetItemCount() == 0) && (TraderPlusHelper.IsOwnerOftheInsurance(player, ntarget.GetCarUniqueId()) || ntarget.GetCarUniqueId() == 0) && (player.GetSafeZoneStatus() == SZ_IN_SAFEZONE || !GetTraderPlusConfigClient().IsReceiptTraderOnly) && !ntarget.m_IsCKLocked)
				{
					bool IsEmpty = true;
					Transport transport = Transport.Cast(target.GetObject());
					if (!transport)return false;

					int crewSize = transport.CrewSize();
					for (int j = 0; j < crewSize; j++)
					{
						if (transport.CrewMember(j))
							IsEmpty = false;
					}

					if (IsEmpty)return true;
				}
			#else
				#ifdef TRADER
					if (ConditionAddon(player, target, item) || ntarget && (GetTraderPlusConfigClient().IsReceiptSaveCargo || !GetTraderPlusConfigClient().IsReceiptSaveCargo && ntarget.GetInventory().GetCargo().GetItemCount() == 0) && (TraderPlusHelper.IsOwnerOftheInsurance(player, ntarget.GetCarUniqueId()) || ntarget.GetCarUniqueId() == 0) && (player.GetSafeZoneStatus() == SZ_IN_SAFEZONE || !GetTraderPlusConfigClient().IsReceiptTraderOnly) && !ntarget.m_Trader_Locked)
					{
						bool IsEmpty = true;
						Transport transport = Transport.Cast(target.GetObject());
						if (!transport)return false;

						int crewSize = transport.CrewSize();
						for (int j = 0; j < crewSize; j++)
						{
							if (transport.CrewMember(j))
								IsEmpty = false;
						}

						if (IsEmpty)return true;
					}
					#else
					 #ifdef EXPANSIONMODVEHICLE
						 if (ConditionAddon(player, target, item) || ntarget && (GetTraderPlusConfigClient().IsReceiptSaveCargo || !GetTraderPlusConfigClient().IsReceiptSaveCargo && ntarget.GetInventory().GetCargo().GetItemCount() == 0) && (TraderPlusHelper.IsOwnerOftheInsurance(player, ntarget.GetCarUniqueId()) || ntarget.GetCarUniqueId() == 0) && ntarget.GetLockedState() == ExpansionVehicleLockState.READY_TO_LOCK && (player.GetSafeZoneStatus() == SZ_IN_SAFEZONE || !GetTraderPlusConfigClient().IsReceiptTraderOnly))
						 {
							 bool IsEmpty = true;
							 Transport transport = Transport.Cast(target.GetObject());
							 if (!transport)return false;

							 int crewSize = transport.CrewSize();
							 for (int j = 0; j < crewSize; j++)
							 {
								 if (transport.CrewMember(j))
									 IsEmpty = false;
							 }

							 if (IsEmpty)return true;
						 }
					 #else
						if (ConditionAddon(player, target, item) || ntarget && (GetTraderPlusConfigClient().IsReceiptSaveCargo || !GetTraderPlusConfigClient().IsReceiptSaveCargo && ntarget.GetInventory().GetCargo().GetItemCount() == 0) && (TraderPlusHelper.IsOwnerOftheInsurance(player, ntarget.GetCarUniqueId()) || ntarget.GetCarUniqueId() == 0) && (player.GetSafeZoneStatus() == SZ_IN_SAFEZONE || !GetTraderPlusConfigClient().IsReceiptTraderOnly))
						{
							bool IsEmpty = true;
							Transport transport = Transport.Cast(target.GetObject());
							if (!transport)return false;

							int crewSize = transport.CrewSize();
							for (int j = 0; j < crewSize; j++)
							{
								if (transport.CrewMember(j))
									IsEmpty = false;
							}

							if (IsEmpty)return true;
						}
					#endif
				#endif
			#endif
		#endif
		return false;
	}

	//you can override that function for modding purpose -> function is client only !
	bool ConditionAddon(PlayerBase player, ActionTarget target, ItemBase item)
	{
		return false;
	}

	override void OnFinishProgressServer( ActionData action_data )
	{
    if ( action_data.m_MainItem && action_data.m_MainItem.GetHierarchyRootPlayer() == action_data.m_Player )
		{
	      CarScript vehicle = CarScript.Cast(action_data.m_Target.GetObject());
				string type = vehicle.GetType();
				NewReceipt receipt = NewReceipt.Cast(GetGame().CreateObject("NewReceipt", action_data.m_Player.GetPosition()));
				receipt.CarClassName = type;

				int collateralMoney = 0;
				string insuranceData = "";
				TraderPlusBankingData account = action_data.m_Player.GetBankAccount();
				if(vehicle.GetCarUniqueId()!=0 && account && account.Insurances.Find(vehicle.GetCarUniqueId(),insuranceData))
				{
					collateralMoney = TraderPlusHelper.GetCollateralPriceInsuranceData(insuranceData);
					receipt.CarUniqueId = vehicle.GetCarUniqueId();
					receipt.CollateralMoney = collateralMoney;
					account.Insurances.Remove(receipt.CarUniqueId);
					account.UpdateAccount(action_data.m_Player);
				}

				if(GetTraderPlusConfigServer().IsReceiptSaveLock)
				{
					#ifndef CARLOCKDISABLE
						receipt.CarPassword = vehicle.m_CarLockPassword;
					#else
						#ifdef MuchCarKey
							receipt.CarPassword = vehicle.m_CarKeyId;
						#endif
					#endif
				}

				if(GetTraderPlusConfigServer().IsReceiptSaveAttachment)
					receipt.SetAttachments(vehicle);
				else
					receipt.Attachments = TraderPlusHelper.GetVehicleAttachmentsFromConfig(receipt.CarClassName);

				if(GetTraderPlusConfigServer().IsReceiptSaveCargo)
					receipt = NewReceipt.Cast(TraderPlusHelper.TransferInventoryVehicle(vehicle, receipt));

				receipt.GetInventory().LockInventory(HIDE_INV_FROM_SCRIPT);
				receipt.SetSynchDirty();

	      //bool state = TraderPlusHelper.CreateInInventoryWithState(action_data.m_Player, type,action_data.m_Target.GetObject().GetHealthLevel(),1);
				//if(!state)return;
				GetGame().ObjectDelete(vehicle);
		}
  }
};
