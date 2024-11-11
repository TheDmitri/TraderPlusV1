//for retrocompatibility do not use for modding use instead GetSafeZoneName(), SetSafeZoneName()
//for retrocompatibility do not use for modding use instead GetSafeZoneStatus() == SZ_IN_SAFEZONE or GetSafeZoneName() == SZ_OUT_SAFEZONE
class InsideSZ{

	string SZName;
	bool   SZStatut;

	void InsideSZ(){
		SZName="";
		SZStatut=false;
	}
}

modded class PlayerBase extends ManBase
{
	private int m_SafeZoneStatus;
	private string m_SafeZoneName;

	bool m_IsStashDisable = false;

	ref InsideSZ IsInsideSZ;

	//for retrocompatibility do not use for modding use instead GetSafeZoneStatus() == SZ_IN_SAFEZONE or GetSafeZoneName() == SZ_OUT_SAFEZONE
	bool IsSZAdmin = false;

	int m_SZ_InfluenzaEnteredSafeZone;

	override void Init()
	{
		m_SafeZoneStatus = 0;
		m_SafeZoneName = "";
		super.Init();
		IsInsideSZ=new InsideSZ;
		RegisterNetSyncVariableInt("m_SafeZoneStatus");
	}

	bool IsTraderPlusAdmin()
	{
		if(GetGame().IsServer())
			return GetSZConfig().SZSteamUIDs.Find(GetIdentity().GetPlainId()) != -1;

		return false;
	}

	void SetAdminStatus()
	{
		m_SafeZoneStatus = m_SafeZoneStatus | 0x02;
		SetSynchDirty();
	}

	void SetSafeZoneStatus(bool state)
	{
		if(state)
			m_SafeZoneStatus = m_SafeZoneStatus | 0x01;
		else
			m_SafeZoneStatus = m_SafeZoneStatus & 0xFE;

		if(GetSZConfig().IsHideOutActive)
			HideOutObjectHandle();

		SetSynchDirty();
	}

	void SetSafeZoneName(string name)
	{
		m_SafeZoneName = name;
	}

	int GetSafeZoneStatus()
	{
		return m_SafeZoneStatus & 0x01;
	}

	int GetAdminStatus()
	{
		return m_SafeZoneStatus & 0x02;
	}

	string GetSafeZoneName()
	{
		return m_SafeZoneName;
	}

	override void SetSuicide(bool state)
	{
		super.SetSuicide(state);
		SZSetSuicideHandler(state);
	}

	//for modding ability
	void SZSetSuicideHandler(bool state)
	{
		if (state && GetSafeZoneStatus() == SZ_IN_SAFEZONE && GetGame().IsServer())
		{
			SetAllowDamage(true);
			SetSafeZoneStatus(false);
			if(GetSZConfig().IsHideOutActive)
				DetachHideOutObjectFromPlayer();
		}
	}

	override void EEHitBy(TotalDamageResult damageResult, int damageType, EntityAI source, int component, string dmgZone, string ammo, vector modelPos, float speedCoef)
	{
		super.EEHitBy(damageResult,damageType,source,component,dmgZone,ammo,modelPos,speedCoef);
		SZEEHitByHandler(damageResult,damageType,source,component,dmgZone,ammo,modelPos,speedCoef);
	}

	//for modding ability
	void SZEEHitByHandler(TotalDamageResult damageResult, int damageType, EntityAI source, int component, string dmgZone, string ammo, vector modelPos, float speedCoef)
	{
		if(GetSafeZoneStatus() == SZ_IN_SAFEZONE)
		{
				if(source.IsDayZCreature() || source.IsAnimal());
						source.Delete();
				#ifdef SZDEBUG
				GetTraderPlusLogger().LogInfo(this.GetIdentity().GetName()+"the source of damage has been deleted");
				#endif
				return;
		}
	}

	override void EEItemDetached(EntityAI item, string slot_name)
	{
		super.EEItemDetached(item, slot_name);
		SZEEItemDetachedHandler(item, slot_name);
	}

	//for modding ability
	void SZEEItemDetachedHandler(EntityAI item, string slot_name)
	{
		if(slot_name == "Armband" && GetSafeZoneStatus() == SZ_IN_SAFEZONE && GetGame().IsServer() && GetSZConfig().IsHideOutActive && !m_IsStashDisable)
			HideOutObjectHandle();
	}

	override bool CanDropEntity(notnull EntityAI item)
	{
		if(SZCanDropEntityHandler(item))
			return false;

		return super.CanDropEntity(item);
	}

	bool SZCanDropEntityHandler(notnull EntityAI item)
	{
		if(item.IsInherited(HideOutObject))
			return true;

		return false;
	}

	void HideOutObjectHandle()
	{
		#ifdef SZDEBUG
		GetTraderPlusLogger().LogInfo("HideOutObjectHandle");
		#endif
		if(GetSafeZoneStatus() == SZ_IN_SAFEZONE)
		{
			#ifdef SZDEBUG
			GetTraderPlusLogger().LogInfo("m_SafeZoneStatus == SZ_IN_SAFEZONE");
			#endif
			AttachHideOutObjectToPlayer();
		}

		if(GetSafeZoneStatus() == SZ_OUT_SAFEZONE)
		{
			#ifdef SZDEBUG
			GetTraderPlusLogger().LogInfo("m_SafeZoneStatus == SZ_OUT_SAFEZONE");
			#endif
			DetachHideOutObjectFromPlayer();
		}

	}

	void AttachHideOutObjectToPlayer()
	{
		if(!GetIdentity())return;

		int playerId = GetIdentity().GetPlainId().Substring(8, 9).ToInt();

		//Check if the player wear an armband, ask him to remove it if so
		EntityAI tarmband;
		tarmband = FindAttachmentBySlotName("Armband");
		if(tarmband && !tarmband.IsInherited(HideOutObject))
		{
			NotificationSystem.SendNotificationToPlayerIdentityExtended(GetIdentity(), 2, "STASH WARNING", GetSZConfig().MustRemoveArmband, "SafeZone/images/SafeZone.paa");
			return;
		}

		HideOutObject stash;
		if(HideOutObject.GetMapAll().Find(playerId, stash))
		{
			LocalTakeEntityToTargetAttachmentEx(this, stash, InventorySlots.ARMBAND);
			GetGame().RemoteObjectTreeCreate(stash);
		}
		else
		{
			HideOutObject newHideOut = HideOutObject.Cast(GetInventory().CreateAttachmentEx("HideOutObject",InventorySlots.ARMBAND));
			newHideOut.SetOwner(playerId);
			newHideOut.InitStarterKit();
		}
	}

	void DetachHideOutObjectFromPlayer()
	{
		if(!this)return;

		#ifdef SZDEBUG
		GetTraderPlusLogger().LogInfo("DetachHideOutObjectFromPlayer");
		#endif

		EntityAI stash;
		stash = HideOutObject.Cast(FindAttachmentBySlotName("Armband"));
		if(stash)
		{
			ServerDropEntity(stash);
			GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(HideStash, 500, false, stash);
		}
	}

	void HideStash(HideOutObject stash)
	{
		if(stash)
			stash.Hide();
	}
}
