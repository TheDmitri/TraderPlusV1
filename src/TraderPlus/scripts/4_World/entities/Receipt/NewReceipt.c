class NewReceiptBase extends ItemBase
{
	string CarClassName = "";
	int CarUniqueId;
	int CollateralMoney;
	int CarPassword;

	ref array<string> Attachments = new array<string>;

	PlayerBase LastOwner = NULL;

	bool hasFallen = false;
	// Aim point saved by ActionDeployVehicle before the hologram is torn down; not persisted.
	protected vector m_DeployPos;
	protected vector m_DeployOri;
	protected bool m_HasDeployTarget;

	void NewReceiptBase()
	{

	}

	override void OnInventoryEnter(Man player)
	{
		super.OnInventoryEnter(player);

		PlayerBase tplayer = PlayerBase.Cast(player);
		if(tplayer)
			GetRPCManager().SendRPC("TraderPlus", "GetCarNameReceiptResponse",  new Param2<NewReceipt, string>(NewReceipt.Cast(this), CarClassName), true, player.GetIdentity());
	}

	void SetAttachments( CarScript car)
	{
		#ifdef TRADERPLUSDEBUG
		GetTraderPlusLogger().LogInfo("SetAttachments");
		#endif
		for ( int i = 0; i < car.GetInventory().AttachmentCount(); i++ )
		{
			EntityAI attachment = car.GetInventory().GetAttachmentFromIndex( i );
			if (attachment)
			{
				string data = attachment.GetType() + "," + attachment.GetHealth();
				#ifdef TRADERPLUSDEBUG
				GetTraderPlusLogger().LogInfo("entity added:"+data);
				#endif
				Attachments.Insert(data);
			}
		}
	}

	override void SetActions()
	{
		super.SetActions();
		AddAction(ActionTogglePlaceObject);
		AddAction(ActionDeployVehicle);
	}

	override string GetDisplayName()
	{
		string displayName;
		g_Game.ObjectGetDisplayName(this, displayName);
		
		if(CarClassName == string.Empty) return displayName;

		displayName = g_Game.ConfigGetTextOut("CfgVehicles " + CarClassName + " displayName");

		if(displayName == string.Empty) return "UNKNOWN_CAR_CLASSNAME";
		return displayName;
	}	

	void SetDeployTarget(vector pos, vector ori)
	{
		m_DeployPos = pos;
		m_DeployOri = ori;
		m_HasDeployTarget = true;
	}

	override void OnPlacementComplete(Man player, vector position = "0 0 0", vector orientation = "0 0 0") 
	{
		super.OnPlacementComplete(player);
	
		if (!GetGame().IsServer()) return;
		if (IsSetForDeletion() || hasFallen) return;

		PlayerBase playerBase = PlayerBase.Cast(player);
		if (!playerBase) return;

		// Best source first: the saved aim point, then where PlaceEntity moved the receipt,
		// then the synced projection, and the player as a last resort (never map origin).
		vector spawnPosition = playerBase.GetLocalProjectionPosition();
		vector spawnOrientation = playerBase.GetLocalProjectionOrientation();
		if (m_HasDeployTarget && !IsMapOrigin(m_DeployPos))
		{
			spawnPosition = m_DeployPos;
			spawnOrientation = m_DeployOri;
		}
		else if (!IsMapOrigin(GetPosition()))
			spawnPosition = GetPosition();
		else if (IsMapOrigin(spawnPosition))
			spawnPosition = playerBase.GetPosition();
		spawnPosition[1] = spawnPosition[1] + TraderPlusHelper.GetHeightIfClassNameInVehicleConfig(CarClassName);

		if (SpawnVehicleFromReceipt(playerBase, spawnPosition, spawnOrientation))
			SetIsDeploySound(true);
	}

	protected bool IsMapOrigin(vector pos)
	{
		return pos[0] == 0 && pos[2] == 0;
	}

	// The receipt is deleted only once the vehicle exists; a failed spawn gives it back.
	protected bool SpawnVehicleFromReceipt(PlayerBase owner, vector position, vector orientation)
	{
		if (IsSetForDeletion() || hasFallen)
			return false;
		hasFallen = true;
		GetInventory().UnlockInventory(HIDE_INV_FROM_SCRIPT);
		SetSynchDirty();
		if (TraderPlusHelper.CreateVehicle(owner, CarClassName, position, orientation, Attachments, this, CarUniqueId, CollateralMoney, CarPassword))
		{
			Delete();
			return true;
		}
		hasFallen = false;
		GetInventory().LockInventory(HIDE_INV_FROM_SCRIPT);
		GetTraderPlusLogger().LogInfo("Receipt kept: vehicle creation failed for " + CarClassName + " at " + position.ToString());
		return false;
	}

	override void EOnContact(IEntity other, Contact extra)
  {
		// The placing animation drops the receipt; without IsBeingPlaced this contact spawned a second vehicle.
		if ( GetGame().IsServer() && !IsSetForDeletion() && !IsBeingPlaced() )
		 {
			 	if(!hasFallen && LastOwner && (!GetTraderPlusConfigServer().IsReceiptTraderOnly || IsTraderNearby(GetPosition())) && IsNoPlayerNearby(GetPosition()) && !IsVehicleToHigh(GetPosition(),GetPosition()))
				{
					 vector pposition = GetPosition();
					 if( g_Game.SurfaceIsSea( pposition[0], pposition[2]))
					 {
							 pposition[1] = g_Game.SurfaceGetSeaLevel();
							 SetPosition(pposition);
					 }else if (g_Game.SurfaceIsPond(pposition[0], pposition[2]))
					 {
							 float depth = g_Game.GetWaterDepth(pposition);
							 pposition[1] = pposition[1] + depth;
							 SetPosition( pposition );
					 }
					 else
					 {
						 pposition[1]=GetGame().SurfaceY( pposition[0], pposition[2] ) + TraderPlusHelper.GetHeightIfClassNameInVehicleConfig(CarClassName);
					 }
					vector oorientation = LastOwner.GetOrientation();
					 SpawnVehicleFromReceipt(LastOwner, pposition, oorientation);
				}
		 }
	}

	bool IsTraderNearby(vector playerpos)
	{
		for(int i=0;i<GetTraderPlusConfigServer().Traders.Count();i++)
		{
			float dist = vector.Distance(GetTraderPlusConfigServer().Traders[i].Position,playerpos);
			if(dist < GetTraderPlusConfigServer().GetReceiptThrowTraderRadius())
			{
				return true;
			}
		}

		return false;
	}

	bool IsVehicleToHigh(vector ppos, vector carpos)
	{
		if(GetTraderPlusConfigServer().DisableHeightFailSafeForReceiptDeployment)
		{
			Print("DisableHeightFailSafeForReceiptDeployment");
			return false;
		}
		float distance = Math.Sqrt(Math.Pow((carpos[1]-ppos[1]),2));
		if(distance > 0.5)
			return true;
		else
			return false;
	}


	bool IsNoPlayerNearby(vector pos)
	{
		array<Object> objs = new array<Object>;

		GetGame().GetObjectsAtPosition3D(pos, 2.5, objs, NULL);
		if (objs.Count() < 1)
		{
		return true;
		}

		for (int i = 0; i < objs.Count(); i++)
		{
		string tempObjId = objs.Get(i).ToString();
		tempObjId.ToLower();
		if(objs.Get(i).IsKindOf("SurvivorBase"))
			return false;
		}
		return true;
	}

	override void OnStoreSave( ParamsWriteContext ctx )
	 {
		super.OnStoreSave(ctx);
		Param5<int,int,int, string,array<string>> data = new Param5<int, int,int,string,array<string>>(CarPassword, CarUniqueId,CollateralMoney,CarClassName, Attachments);
		ctx.Write(data);
	 }

	override bool OnStoreLoad( ParamsReadContext ctx, int version )
	{
		if ( !super.OnStoreLoad( ctx, version ) )
			return false;

		Param5<int, int,int, string, array<string>>data = new Param5<int, int, int, string, array<string>>(0,0,0,"",new array<string>);
		if (ctx.Read(data))
		{
			CarPassword = data.param1;
			CarUniqueId = data.param2;
			CollateralMoney = data.param3;
			CarClassName = data.param4;
			Attachments = data.param5;
		}
		RefreshLifeTimeCargo();
		CheckIfDataStored();
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

	#ifdef UNIVERSALAPI
	override void OnUApiSave(UApiEntityStore data)
	{
        super.OnUApiSave(data);
		data.Write("CarPassword", CarPassword); // can be any primitive type or primitive array
		data.Write("CarUniqueId", CarUniqueId); // can be any primitive type or primitive array
		data.Write("CollateralMoney", CollateralMoney); // can be any primitive type or primitive array
		data.Write("CarClassName", CarClassName); // can be any primitive type or primitive array
		data.Write("Attachments", Attachments); // can be any primitive type or primitive array
    }

    override void OnUApiLoad(UApiEntityStore data)
	{
        super.OnUApiLoad(data);
		if (!data.Read("CarPassword", CarPassword ))
            CarPassword  = 0;

        if (!data.Read("CarUniqueId", CarUniqueId ))
            CarUniqueId  = 0;

		if (!data.Read("CollateralMoney", CollateralMoney ))
		    CollateralMoney  = 0;

		if (!data.Read("CarClassName", CarClassName ))
			CarClassName  = "";

		if (!data.Read("Attachments", Attachments ))
			Attachments = new array<string>;

		CheckIfDataStored();
	}
	#endif

	void CheckIfDataStored()
	{
		if(CarClassName!= "")
		{
			GetInventory().LockInventory(HIDE_INV_FROM_SCRIPT);
			SetSynchDirty();
		}
	}
};

class NewReceipt extends NewReceiptBase
{

}
