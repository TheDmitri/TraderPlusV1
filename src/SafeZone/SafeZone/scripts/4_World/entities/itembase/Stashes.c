class HideOutObject extends Armband_Yellow
{
	private int m_PlayerID = 0;
	private static ref map<int, HideOutObject > m_MapAllHideOut = new map<int, HideOutObject >;

	void HideOutObject()
	{
		RegisterNetSyncVariableInt("m_PlayerID");
	}

	void ~HideOutObject()
	{
		if(m_MapAllHideOut)
			m_MapAllHideOut.Remove(m_PlayerID);
	}

	override bool CanSwapEntities(EntityAI otherItem, InventoryLocation otherDestination, InventoryLocation destination)
	{
		return false;
	}

	override bool CanPutIntoHands( EntityAI parent )
	{
		return false;
	}

	override bool CanPutInCargo( EntityAI item )
	{
		return false;
	}

	override bool CanReceiveItemIntoCargo(EntityAI item)
	{
		if(GetGame().IsServer() && GetSZConfig() && GetSZConfig().BlackListedItemInStash.Find(item.GetType()) != -1)
			return false;

		if(GetGame().IsClient() && GetSZConfigClient() && GetSZConfigClient().BlackListedItemInStash.Find(item.GetType()) != -1)
			return false;

		return super.CanReceiveItemIntoCargo(item);
	}

	static map<int, HideOutObject > GetMapAll()
	{
		#ifdef SZDEBUG
		GetTraderPlusLogger().LogInfo("Map All HideOut:"+m_MapAllHideOut.Count());
		#endif
		return m_MapAllHideOut;
	}

	void SetOwner(int id)
	{
		m_PlayerID = id;
		m_MapAllHideOut.Set(m_PlayerID, this);
		#ifdef SZDEBUG
		GetTraderPlusLogger().LogInfo("Map All HideOut:"+m_MapAllHideOut.Count());
		#endif
		SetSynchDirty();
	}

	void RefreshLifeTimeCargo()
	{
		#ifdef SZDEBUG
		GetTraderPlusLogger().LogInfo("RefreshLifeTimeCargo");
		#endif
		array<EntityAI> itemsArray = new array<EntityAI>;
    array<ItemBase> stackableItems = new array<ItemBase>;
    GetInventory().EnumerateInventory(InventoryTraversalType.PREORDER, itemsArray);
    for (int i = 0; i < itemsArray.Count(); ++i)
    {
        ItemBase item = ItemBase.Cast(itemsArray[i]);
        if (item)
				{
					#ifdef SZDEBUG
					GetTraderPlusLogger().LogInfo(string.Format("item_%1",item.GetType()));
					#endif
					item.SetLifetime(3888000);
				}

		}
		#ifdef SZDEBUG
		GetTraderPlusLogger().LogInfo("End RefreshLifeTimeCargo");
		#endif
	}

	int GetOwner()
	{
		return m_PlayerID;
	}

	override void OnStoreSave( ParamsWriteContext ctx )
	{
		super.OnStoreSave( ctx );
		ctx.Write( m_PlayerID );
	}

	override bool OnStoreLoad( ParamsReadContext ctx, int version )
	{
		if ( !super.OnStoreLoad( ctx, version ) )
			return false;

		if ( !ctx.Read( m_PlayerID ) )
		{
			this.Delete();
			return false;
		}
		else
			m_MapAllHideOut.Set(m_PlayerID, this);

		SetPosition(vector.Zero);
		RefreshLifeTimeCargo();
		return true;
	}

	void Hide()
	{
		SetPosition(vector.Zero);
		GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(CheckStashPos, 500, false);
	}

	void CheckStashPos()
	{
		float dist = vector.Distance(GetPosition(), vector.Zero);
		if(dist > 1)
			GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(Hide, 500, false);
	}

	void InitStarterKit()
	{
	};
}
