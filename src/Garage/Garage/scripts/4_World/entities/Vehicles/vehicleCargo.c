class TraderPlusVehicleCargo extends ItemBase
{
  private static ref map<int, TraderPlusVehicleCargo> m_MapAllCargos = new map<int, TraderPlusVehicleCargo>;

  private int m_CargoId;

  void TraderPlusVehicleCargo()
  {
    m_CargoId = 0;
  }

  void ~TraderPlusVehicleCargo()
  {
    if(m_MapAllCargos)
    m_MapAllCargos.Remove(m_CargoId);
  }

  override bool IsInventoryVisible()
	{
		return false;
	}

  static map<int, TraderPlusVehicleCargo> GetMapAll()
  {
    return m_MapAllCargos;
  }

  int GetCargoId()
  {
    return m_CargoId;
  }

  void SetCargoId(int id)
  {
    m_CargoId = id;
    m_MapAllCargos.Set(id, this);
  }

  override void OnStoreSave( ParamsWriteContext ctx )
   {
      super.OnStoreSave(ctx);
      Param1<int> data = new Param1<int>(m_CargoId);
      ctx.Write(data);
   }

  override bool OnStoreLoad( ParamsReadContext ctx, int version )
    {
      if ( !super.OnStoreLoad( ctx, version ) )
        return false;

          Param1<int>data = new Param1<int>(0);
          if (ctx.Read(data))
          {
            m_CargoId = data.param1;
            SetCargoId(m_CargoId);
            RefreshLifeTimeCargo();
            this.SetPosition("0 0 0");
          }
          return true;
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
						item.SetLifetime(3888000);
			}
		}
}
