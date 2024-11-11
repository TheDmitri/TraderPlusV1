/*To make things easier for me, I use some static class that can be run from anywhere in my code,
it's very efficient to get config data or access my main class that run my code.
However it can create conflict with other mods if someone use to same name, so if you create some static function, make sure
to make the name specific as much as you can. I usually add Name of my mod in front of my function*/

/*Class that deals with my core class, where config, client and server part of my code are created*/
static ref GarageCore m_GarageCore;
static GarageCore GetGarageCore() {
	if ( !m_GarageCore ) {
		m_GarageCore = new GarageCore;
	}
	return m_GarageCore;
}

/*clas that allows me to access the client class */
static ref GarageSettings GetGarageConfig()	{
	return GetGarageCore().m_GarageSettings;
}

/*Function that allows to get the player variable from its identity => used for RPC*/
static PlayerBase GMGetPlayerByIdentity(PlayerIdentity sender)
{
	int	low	 =	0;
	int	high =	0;
	GetGame().GetPlayerNetworkIDByIdentityID( sender.GetPlayerId(), low, high );
	return PlayerBase.Cast( GetGame().GetObjectByNetworkId(low, high ));
}

class GarageHelpers
{
	static int GetLowSteamID(string SteamID64)
	{
		string temp_ID="";
		for(int j = 8; j<17; j++)
		{
			temp_ID+=SteamID64.Get(j);
		}
		return temp_ID.ToInt();
	}

	static CarScript CheckForCarAtPos(vector position)
	{
		array<Object> objects = new array<Object>;
		array<CargoBase> proxyCargos = new array<CargoBase>;
		GetGame().GetObjectsAtPosition(position, 3, objects, proxyCargos);
		CarScript car;
		if (objects)
		{
			for (int i = 0; i < objects.Count(); i++ )
			{
				if (Class.CastTo( car, objects.Get(i) ) )
						return car;
			}
		}
		return NULL;
	}

	static array<EntityAI> GetAttachmentsFromEntityAI(EntityAI ent)
	{
		array<EntityAI> attachementInEntity = new array<EntityAI>;

		if(!ent.GetInventory())return attachementInEntity;

		GameInventory inv = ent.GetInventory();
		int attachmentCount = inv.AttachmentCount();
		for(int i = 0; i < attachmentCount; i++)
		{
			attachementInEntity.Insert(inv.GetAttachmentFromIndex(i));
		}
		return attachementInEntity;
	}

	static array<EntityAI> GetItemsInCargoFromEntityAI(EntityAI ent)
	{
		array<EntityAI> itemsInCargos = new array<EntityAI>;

		if(!ent.GetInventory() || !ent.GetInventory().GetCargo())return itemsInCargos;

		CargoBase cargo = ent.GetInventory().GetCargo();
		for ( int i = 0; i < cargo.GetItemCount(); i++ )
		{
			 itemsInCargos.Insert(cargo.GetItem( i ));
		}
		return itemsInCargos;
	}

	static array<ref ItemsCargo> ConvertEntitiesToItemsCargo(array<EntityAI>entities, int extradata)
	{
		array<ref ItemsCargo> itemsInCargo = new array<ref ItemsCargo>;
		foreach(EntityAI ent: entities)
		{
			#ifdef MuchCarKey
				MCK_CarKey_Base key;
				if(Class.CastTo(key, ent))extradata = key.GetMCKId();
			#else
				#ifdef TRADER
				VehicleKeyBase key;
				if(Class.CastTo(key, ent))extradata = key.GetHash();
				#endif
			#endif
			itemsInCargo.Insert(new ItemsCargo(ent.GetType(), TraderPlusHelper.GetItemAmount(ItemBase.Cast(ent)), ent.GetHealth(),extradata));
		}
		return itemsInCargo;
	}

	static bool CarHasMember(CarScript car)
	{
		bool IsEmpty = false;

		Transport transport = car;
		if (!transport)return false;

		int crewSize = transport.CrewSize();
		for (int j = 0; j < crewSize; j++)
		{
			if (transport.CrewMember(j))
				IsEmpty = true;
		}

		return IsEmpty;
	}

	static bool CanParkVehicle(CarScript car)
	{
		#ifndef CARLOCKDISABLE
			PlayerBase player = PlayerBase.Cast(GetGame().GetPlayer());
			if(player)
			{
				if(car.m_CarLockOwner == player.CLSteamlowID || player.GetAdminStatus() == SZ_IS_ADMIN || player.HasPassword(car.m_CarLockPassword,car.m_CarLockOwner))
					return true;
				else
					return false;
			}
		#endif

		return true;
	}

	static bool IsParkingAvailable(vector carpos, vector carori)
	{
		vector size = "2 2 2";
		array<Object> excluded_objects = new array<Object>;
		array<Object> nearby_objects = new array<Object>;

		GetGame().IsBoxColliding( carpos, carori, size, excluded_objects, nearby_objects);
		if (nearby_objects.Count() > 0)
		{
			#ifdef GMDEBUG
			GetGMLogger().LogInfo("IsParkingAvailable"+nearby_objects.Get(0).GetType());
			#endif
			for(int i = 0;i<nearby_objects.Count();i++)
			{
				string objectName = nearby_objects.Get(i).GetType();
				if(GetGarageConfig().WhitelistedObjects.Find(objectName) == -1)
					return false;
			}
		}
		return true;
	}

	static int ConvertHealthToHealthLevel(string classname, float currentHealth)
	{
		if(currentHealth == -1)
			return TraderPlusItemState.PRISTINE;

		float maxHealth = MiscGameplayFunctions.GetTypeMaxGlobalHealth(classname);
		if(maxHealth == 0)
			return TraderPlusItemState.PRISTINE;

		float healthCoeff = currentHealth/maxHealth;
		if(healthCoeff <= 1.0 && healthCoeff> 0.7) return TraderPlusItemState.PRISTINE;
		if(healthCoeff <= 0.7 && healthCoeff> 0.5) return TraderPlusItemState.WORN;
		if(healthCoeff <= 0.5 && healthCoeff> 0.3) return TraderPlusItemState.DAMAGED;
		if(healthCoeff <= 0.3 && healthCoeff> 0.0) return TraderPlusItemState.BADLY_DAMAGED;

		return TraderPlusItemState.RUINED;
	}

	static bool CreateVehicle(PlayerBase player, string classname, int cargoId, int collateralMoney, int id, float health, float fuel, vector position, vector orientation, array<ref ItemsCargo> items)
	{
		#ifdef GMDEBUG
		GetGMLogger().LogInfo("CreateVehicle");
		#endif
		Object obj = GetGame().CreateObject(classname, vector.Zero, false, false, true);

		obj.SetPosition(vector.Zero);
		obj.SetOrientation(orientation);
		vector roll = obj.GetOrientation();
		roll[2] = roll[2] - 1;
		obj.SetOrientation(roll);
		roll[2] = roll[2] + 1;
		obj.SetOrientation(roll);

		EntityAI ent = EntityAI.Cast(obj);
		if (ent)
		{
			//compatibility SIB Helis
			if(ent.IsKindOf("HeliTest_SIB"))
			{
				for(int indexH = 0; indexH < 4; indexH++)
				{
					EntityAI fakeWheel = ent.GetInventory().CreateAttachment("HatchbackWheel");
				}
			}
			for (int i = 0; i < items.Count(); i++)
			{
					if (items[i].ExtraData == GARAGEVEHICLEATTACHMENT)
					{
						#ifdef GMDEBUG
						GetGMLogger().LogInfo("Create attach:"+items[i].ItemName);
						#endif
						EntityAI att = ent.GetInventory().CreateAttachment(items[i].ItemName);
						if(!att)att = EntityAI.Cast( GetGame().CreateObjectEx(items[i].ItemName, vector.Zero, ECE_PLACE_ON_SURFACE) );
						if(att)
						{
							ItemBase item = ItemBase.Cast(att);
							if(item)
							{
								item = TraderPlusHelper.SetQuantity(item, items[i].Amount);
								item.SetHealth(items[i].Health);
							}
						}
					}
			}

			if(cargoId != 0)
			{
				TraderPlusVehicleCargo vehicleCargo;
				if(GetGarageConfig().SaveVehicleCargo && TraderPlusVehicleCargo.GetMapAll().Find(cargoId, vehicleCargo))
				{
					ent = TraderPlusHelper.TransferInventoryVehicle(vehicleCargo, ent);
					GetGame().ObjectDelete(vehicleCargo);
				}
				else
				{
					int playerId = GetLowSteamID(player.GetIdentity().GetPlainId());
					GarageItemData garageItemData = GarageItemData.Load(playerId,cargoId);
					if(garageItemData)
					{
						RestoreCargo(ent, garageItemData.childrens);
						garageItemData.DeleteCargo(playerId, cargoId);
					}
				}
			}

			CarScript vehicle = CarScript.Cast(ent);
			if(vehicle)
			{
				vehicle.DifferOrientation(orientation);
				if(collateralMoney != 0)
				{
					int insuranceId = TraderPlusHelper.GetTimeStamp();
					TraderPlusBankingData account = player.GetBankAccount();
					if(account)
					{
						account.Insurances.Set(insuranceId, vehicle.GetType() + "," + collateralMoney.ToString());
						vehicle.SetCarUniqueId(insuranceId);
						account.UpdateAccount(player);
					}
				}

				if(GetGarageConfig().SaveVehicleHealth){
					#ifdef GMDEBUG
					GetGMLogger().LogInfo("Set Health:"+health);
					#endif
					vehicle.SetHealth(health);
				}

				float fuelReq = vehicle.GetFluidCapacity(CarFluid.FUEL);
				float oilReq = vehicle.GetFluidCapacity(CarFluid.OIL);
				float coolantReq = vehicle.GetFluidCapacity(CarFluid.COOLANT);
				float brakeReq = vehicle.GetFluidCapacity(CarFluid.BRAKE);

				if(GetGarageConfig().SaveVehicleFuel){
					#ifdef GMDEBUG
					GetGMLogger().LogInfo("Set Fuel:"+fuel);
					#endif
					fuelReq = fuel;
				}
				vehicle.Fill(CarFluid.FUEL, fuelReq);
				vehicle.Fill(CarFluid.OIL, oilReq);
				vehicle.Fill(CarFluid.COOLANT, coolantReq);
				vehicle.Fill(CarFluid.BRAKE, brakeReq);

				//LOCK SCRIPT
				if(GetGarageConfig().VehicleMustHaveLock || GetGarageConfig().SaveVehicleLock)
				{
					#ifdef GMDEBUG
					GetGMLogger().LogInfo("Set Lock:"+id.ToString());
					#endif
					#ifndef CARLOCKDISABLE
						if(id != -1 && id != 0){
							vehicle.SetCarLockOwner(player.CLSteamlowID);
							vehicle.SetCarLockPassword(id);
							vehicle.SetCarLock(true);
						}
					#else
						#ifdef MuchCarKey
							if(id != 0)
							{
								vehicle.m_HasCKAssigned = true;
								vehicle.m_CarKeyId = id;
								vehicle.SynchronizeValues();
							}
						#else
						 #ifdef TRADER
							 if(id != 0){
								 vehicle.m_Trader_HasKey = true;
								 vehicle.m_Trader_VehicleKeyHash = id;
								 vehicle.SynchronizeValues();
							 }
						 #endif
						#endif
					#endif
				}

				obj.SetPosition(position + Vector(0, 0.3, 0));
				obj.SetOrientation(orientation);
				GetGMLogger().LogInfo(string.Format("vehicle orientation: %1", roll));
				roll = obj.GetOrientation();
				roll[2] = roll[2] - 1;
				obj.SetOrientation(roll);
				roll[2] = roll[2] + 1;
				obj.SetOrientation(roll);
				obj.Update();

				GetGMLogger().LogInfo(string.Format("vehicle orientation: %1", roll));

				return true;
	    }
		}

		GetGame().ObjectDelete(obj);
		#ifdef GMDEBUG
		GetGMLogger().LogInfo("return false");
		#endif
    return false;
	}

	static bool CheckifPlayerHasEnoughMoney(PlayerBase player, int amount)
	{
		int playerMoneyAmount = GetPlayerMoney(player);
		if(playerMoneyAmount >= amount)
			return true;

		return false;
	}

	static bool RemoveMoneyFromPlayer(PlayerBase player, int amount)
	{
		if(GetGarageConfig().PayWithBankAccount)
    {
			TraderPlusBankingData account = player.GetBankAccount();
			if(account)
			{
				account.MoneyAmount = account.MoneyAmount - amount;
				account.UpdateAccount(player);
				return true;
			}
		}
		else
		{
			return TraderPlusBankHelpers.RemoveMoneyFromPlayer(player, amount);
		}

		return false;
	}

	static int GetPlayerMoney(PlayerBase player)
	{
		if(GetGarageConfig().PayWithBankAccount)
    {
			TraderPlusBankingData account = player.GetBankAccount();
			return account.MoneyAmount;
    }
    else
    {
			return TraderPlusBankHelpers.GetPlayerMoney(player);
    }
	}

	static ref GarageItemData GetGarageItemData(EntityAI item)
	{
		GarageItemData gItemD = new GarageItemData;
		gItemD.item = item.GetType();
		ItemBase itemBs;
		if (Class.CastTo(itemBs, item))
		{
			if(itemBs.IsLiquidContainer())
				gItemD.liquidType = itemBs.GetLiquidType();
			else
				gItemD.liquidType = 0;

			gItemD.quantity = itemBs.GetQuantity();
			gItemD.health = itemBs.GetHealth();

			#ifdef MuchCarKey
				MCK_CarKey_Base key;
				if(Class.CastTo(key, itemBs))gItemD.extraData = key.GetMCKId();
			#else
				#ifdef TRADER
				VehicleKeyBase key;
				if(Class.CastTo(key, itemBs))gItemD.extraData = key.GetHash();
				#endif
			#endif

			if(itemBs.IsFood()){
				Edible_Base itemFood = Edible_Base.Cast(itemBs);
				if(itemFood)
					gItemD.foodstage = itemFood.GetFoodStageType();
			}

			gItemD.temp = itemBs.GetTemperature();
			gItemD.wetness = itemBs.GetWet();

			ComponentEnergyManager em = itemBs.GetCompEM();
			if (em)
				gItemD.energy = em.GetEnergy();
		}

		Magazine itemMag;
		if (Class.CastTo(itemMag, item)) {
			gItemD.magAmmoCount = itemMag.GetAmmoCount();
		}

		GameInventory inv = item.GetInventory();

		InventoryLocation item_src = new InventoryLocation;
		inv.GetCurrentInventoryLocation(item_src);
		if (item_src) {
			gItemD.slot = item_src.GetSlot();
			gItemD.locType = item_src.GetType();
			gItemD.locIdx = item_src.GetIdx();
			gItemD.locRow = item_src.GetRow();
			gItemD.locCol = item_src.GetCol();
			gItemD.locFlip = item_src.GetFlip();
		}

		gItemD.childrens = new array < ref GarageItemData >;
		GarageItemData gItemDCargo;
		int i;

		CargoBase cargo = inv.GetCargo();
		if (cargo && GetGarageConfig().SaveVehicleCargo)
		{
			for (i = 0; i < cargo.GetItemCount(); i++)
			{
				gItemDCargo = GetGarageItemData(cargo.GetItem(i));
				gItemD.childrens.Insert(gItemDCargo);
			}
		}

		for (i = 0; i < inv.AttachmentCount(); i++)
		{
			gItemDCargo = GetGarageItemData(inv.GetAttachmentFromIndex(i));
			gItemD.childrens.Insert(gItemDCargo);
		}
		return gItemD;
	}

	static void RestoreCargo(EntityAI entity, array<ref GarageItemData> childrens)
	{
		GameInventory inv = entity.GetInventory();
		if (!inv)
			return;

		GarageItemData c;
		#ifdef GMDEBUG
		GetGMLogger().LogInfo("childrens.Count():"+childrens.Count());
		#endif
		for (int i = 0; i < childrens.Count(); i++)
		{
			#ifdef GMDEBUG
			GetGMLogger().LogInfo("childrens[i]:"+childrens[i].item + "i:"+i);
			#endif
			c = childrens.Get(i);
			if (!c)
				continue;

			EntityAI item = CreateItem(inv, entity, c);

			if (item)
			{
				#ifdef GMDEBUG
				GetGMLogger().LogInfo("ent exist");
				#endif
				ItemBase itemBs;
				if (Class.CastTo(itemBs, item))
				{
					#ifdef GMDEBUG
					GetGMLogger().LogInfo("item exist");
					#endif
					itemBs.SetQuantity(c.quantity);
					itemBs.SetHealth(c.health);
					itemBs.SetTemperature(c.temp);
					itemBs.SetWet(c.wetness);

					if(itemBs.IsLiquidContainer())
						itemBs.SetLiquidType(c.liquidType);

					if(c.liquidType >= 1 && c.liquidType <= 128) {
							BloodContainerBase bloodBag = BloodContainerBase.Cast(itemBs);
							bloodBag.SetBloodTypeVisible(true);
					}

					if(itemBs.IsFood()){
						Edible_Base itemFood = Edible_Base.Cast(itemBs);
						if(itemFood)
							itemFood.ChangeFoodStage(c.foodstage);
					}

					ComponentEnergyManager em = itemBs.GetCompEM();
					if (em)
						em.SetEnergy(c.energy);
				}

				Magazine itemMag;
				if (Class.CastTo(itemMag, item))
					itemMag.ServerSetAmmoCount(c.magAmmoCount);

				if(c.childrens.Count() != 0)
					RestoreCargoBis(item, c.childrens);
			}
		}
	}

	static void RestoreCargoBis(EntityAI entity, array<ref GarageItemData> childrens)
	{
		RestoreCargo(entity, childrens);
	}

	static EntityAI CreateItem(GameInventory inv, EntityAI entity, GarageItemData c)
	{
		EntityAI item;
		switch (c.locType)
		{
			case InventoryLocationType.ATTACHMENT:
			{
				#ifdef GMDEBUG
				GetGMLogger().LogInfo("create att");
				#endif
				item = inv.CreateAttachmentEx(c.item, c.slot);
				break;
			}
			case InventoryLocationType.CARGO:
			{
				#ifdef GMDEBUG
				GetGMLogger().LogInfo("create item in cargo");
				#endif
				item = inv.CreateEntityInCargoEx(c.item, c.locIdx, c.locRow, c.locCol, c.locFlip);
				break;
			}
			case InventoryLocationType.HANDS:
			{
				HumanInventory hInv;
				if (Class.CastTo(hInv, inv))
				{
					#ifdef GMDEBUG
					GetGMLogger().LogInfo("create item in hand");
					#endif
					item = hInv.CreateInHands(c.item);
					if (!item) {
						break;
					}
				}
				break;
			}
		}
		return item;
	}
}
