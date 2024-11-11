modded class WoodenCrate: DeployableContainer_Base
{
  override bool CanReceiveItemIntoCargo(EntityAI item)
  {
    if(GetHierarchyParent() && GetHierarchyParent().IsKindOf("HideOutObject"))
    {
      if(GetGame().IsServer() && GetSZConfig() && GetSZConfig().BlackListedItemInStash.Find(item.GetType()) != -1)
        return false;

      if(GetGame().IsClient() && GetSZConfigClient() && GetSZConfigClient().BlackListedItemInStash.Find(item.GetType()) != -1)
        return false;
    }

    return super.CanReceiveItemIntoCargo(item);
  }

  override bool CanPutAsAttachment( EntityAI parent )
  {
    if(parent.IsKindOf("HideOutObject") && GetInventory().GetCargo().GetItemCount() > 0)
      return false;

    return super.CanPutAsAttachment(parent);
  }

	override void SetActions()
	{
		AddAction(ActionAttachOnSelection);
		super.SetActions();
	}
};
