modded class Hologram
{
	override string ProjectionBasedOnParent()
	{
		ItemBase item_in_hands = ItemBase.Cast( m_Player.GetHumanInventory().GetEntityInHands() );

		 if ( item_in_hands.IsInherited( NewReceipt ))
		{
			NewReceipt carBox = NewReceipt.Cast(item_in_hands);
			if(carBox.CarClassName == ""){
				GetRPCManager().SendRPC("TraderPlus", "GetCarNameReceiptRequest",  new Param1<NewReceipt>(carBox), true, NULL);
				return carBox.GetType();
			}
			//return TraderPlusHelper.GetVehicleClassNameBasedOnID(carBox.CarID);
			return carBox.CarClassName;
		}

		return super.ProjectionBasedOnParent();
	}

	override EntityAI PlaceEntity( EntityAI entity_for_placing )
	{
		ItemBase item_in_hands = ItemBase.Cast( m_Player.GetHumanInventory().GetEntityInHands() );

		if ( entity_for_placing.IsKindOf("NewReceiptBase") )
		{
			/*Object obj = GetGame().CreateObject(m_Projection.GetType(), m_Projection.GetPosition());
			obj.SetOrientation(m_Projection.GetOrientation());
			Class.CastTo(entity_for_placing, obj);*/
			return entity_for_placing;
		}
		else return super.PlaceEntity(entity_for_placing);
	}

	override void EvaluateCollision(ItemBase action_item = null)
	{
		ItemBase item_in_hands = ItemBase.Cast( m_Player.GetHumanInventory().GetEntityInHands() );

		if(item_in_hands)
		{
			 if ( item_in_hands.IsInherited( NewReceiptBase ))
			{
				SetIsColliding(false);
				return;
			}
		}
		super.EvaluateCollision();
	}
};
