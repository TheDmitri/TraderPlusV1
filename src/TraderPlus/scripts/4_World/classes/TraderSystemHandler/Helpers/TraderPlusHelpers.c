class TraderPlusHelper
{
  static bool IsOwnerOftheInsurance(PlayerBase player, int carUniqueId)
  {
    if(!GetBankAccount())return false;

    if(GetBankAccount().Insurances.Contains(carUniqueId))
      return true;

    return false;
  }

  //Based on item in hand and resulting array determine in UIHandle(), we check for an item if it's compatible
  static bool IsItemCompatible(string classname, TStringArray compatibleItemsFromItemInHand)
  {
    if(compatibleItemsFromItemInHand.Find(classname) != -1)
        return true;

    return false;
  }


  static bool HasLicences(int id, out string missingLicences)
  {
    bool   missingLicence = false;
    string licences = GetTraderPlusConfigClient().Licences.Get(id);
    TStringArray licencesArr = new TStringArray;
    licences.Split(",",licencesArr);
    missingLicences = "";
    foreach(string licence: licencesArr)
    {
      bool haslicence = false;
      if(!GetBankAccount() || !GetBankAccount().Licences)return true;
      if(GetBankAccount().Licences.Find(licence) == -1){
        missingLicence = true;
        missingLicences += "\n -"+licence;
      }
    }
    return !missingLicence;
  }

  static bool HasInsurance(string className)
  {
      if(GetTraderPlusConfigClient().InsuranceNames.Find(className) != -1)
        return true;

    return false;
  }

  static int GetInsurancePriceBasedOnClassname(string className, int price)
  {
    int pos = GetTraderPlusConfigClient().InsuranceNames.Find(className);

    if(pos == -1)return 0;

    return (price * GetTraderPlusConfigClient().InsurancePriceCoefficients[pos];
  }

  static int GetCollateralPriceBasedOnClassname(string className, int price)
  {
    int pos = GetTraderPlusConfigClient().InsuranceNames.Find(className);

    if(pos == -1)return 0;

    return price * GetTraderPlusConfigClient().CollateralMoneyCoefficients[pos];
  }

  static TStringArray GetCategoriesFromID(int id)
  {
    int count = GetTraderPlusIDConfig().IDs.Count();
    TStringArray categories = new TStringArray;
    for(int i = 0 ; i< count ; i++)
    {
      if(GetTraderPlusIDConfig().IDs[i].Id == id)
        categories = GetTraderPlusIDConfig().IDs[i].Categories;
    }

    return categories;
  }

  static TraderPlusCategory GetStockCategory(int id, string category, bool isLog = true)
  {
    string idPath = TRADERPLUS_DB_DIR_SERVER +"ID_"+id.ToString()+"\\";
		string categoryPath = idPath + category + ".json";
		TraderPlusCategory tpStockCategory = new TraderPlusCategory;
    if (!FileExist(idPath))
      MakeDirectory(idPath);

    if (FileExist(categoryPath)) {
			TraderPlusJsonLoader<TraderPlusCategory>.LoadFromFile(categoryPath, tpStockCategory, isLog);
		}
		else {
			tpStockCategory.CategoryName = category;
      if(category!="")
        tpStockCategory.Save(id);
		}

    return tpStockCategory;
  }

  static int GetTimeStamp()
  {
  	int year, month, day, hour, minute, second;
  	GetHourMinuteSecondUTC(hour, minute, second);
  	GetYearMonthDayUTC(year, month, day);
  	return JMDate.Timestamp(year,month,day,hour,minute,second);
  }

  static int GetExecutionTime(int timeOrigin)
  {
    return (GetTimeStamp()-timeOrigin);
  }

  //functions made by MaD to empty mag before selling them, thanks for your support !
  static void EmptyMag(PlayerBase player, Magazine mag)
    {
        #ifdef TRADERPLUSDEBUG
        GetTraderPlusLogger().LogInfo("EmptyMag function:");
        #endif
        float dmg;
        string ammoType;
        int quantity = mag.GetAmmoCount();

        // Gather ammo types, quantites and health
        TStringArray ammoKeys = new TStringArray;
        TStringArray ammoTypes = new TStringArray;
        TFloatArray damages = new TFloatArray;
        TIntArray quantites = new TIntArray;
        for (int i = 0; i < quantity; ++i)
        {
            if ( !mag.ServerAcquireCartridge(dmg,ammoType) )
                break;

            float roundDmg = Math.Round(dmg * 1000)/1000;
            string ammoKey = string.Format("%1_%2", ammoType, roundDmg);
            int bulletIndex = ammoKeys.Find(ammoKey);
            if (bulletIndex == -1)
            {
                ammoKeys.Insert(ammoKey);
                damages.Insert(roundDmg);
                quantites.Insert(1);
                ammoTypes.Insert(ammoType);
                continue;
            }
            quantites.Set(bulletIndex, quantites.Get(bulletIndex) + 1);
        }

        // Move to inventory, or ground if inventory full
        array< Magazine > piles;
        Magazine magazinePile;
        for (int j = 0; j < ammoKeys.Count(); ++j)
        {
            float bulletsDamage = damages.Get(j);
            int bulletsCount = quantites.Get(j);
            string bulletType = ammoTypes.Get(j);

            CreateMagazinePilesFromBullet(player, bulletType,bulletsCount,bulletsDamage);
        }
    }

    static array<Magazine> CreateMagazinePilesFromBullet(PlayerBase player,string bullet_type, float quantity, float bulletsDamage )
    {
      #ifdef TRADERPLUSDEBUG
      GetTraderPlusLogger().LogInfo("CreateMagazinePilesFromBullet:"+quantity);
      #endif
      array<Magazine> items = new array<Magazine>;
      string item_name;
      if( !g_Game.ConfigGetText("cfgAmmo " + bullet_type + " spawnPileType", item_name) )
          return items;

      float stack_size = g_Game.ConfigGetInt("cfgMagazines " + item_name + " count");

      if(stack_size > 0)
      {
          Magazine pile;
          int piles_count = Math.Floor(quantity/stack_size);
          int rest = quantity - (piles_count*stack_size);

          for ( int i = 0; i < piles_count; i++ )
          {
              pile = Magazine.Cast(CreateInInventory(player,item_name,stack_size, true));
              items.Insert(pile);
              SetHealthFromDamage(pile, bulletsDamage);
          }
          if ( rest > 0)
          {
              pile = Magazine.Cast(CreateInInventory(player,item_name, rest, true));
              items.Insert(pile);
              SetHealthFromDamage(pile, bulletsDamage);
          }
      }
      return items;
  }

  static void SetHealthFromDamage(Magazine pile, float bulletsDamage)
  {
      float bulletsHealth = pile.GetMaxHealth("", "") * (1.0 - bulletsDamage);
      pile.SetHealth( "", "", bulletsHealth );
  }

  static bool CanLoadMagazine(ItemBase target, ItemBase item)
	{
		Magazine ammo;
		Magazine mag;
		return ( Class.CastTo(ammo, target) && Class.CastTo(mag, item) && mag.GetAmmoCount() < mag.GetAmmoMax() && mag.IsCompatiableAmmo( ammo ) );
	}

  //end function from MaD


  static int GetTraderIDBasedOnCategory(string categoryname)
  {
    for(int i=0; i<GetTraderPlusIDConfig().IDs.Count();i++)
    {
      for(int j=0; j<GetTraderPlusIDConfig().IDs[i].Categories.Count();j++)
      {
        if(categoryname == GetTraderPlusIDConfig().IDs[i].Categories[j])
        {
          return i;
        }
      }
    }
    return -1;
  }

  static int CalculateStockBasedOnCE(string classname)
	{
		int stock = 100;
		EntityAI ent = EntityAI.Cast(GetGame().CreateObject(classname,Vector(0,0,0)));
		GetTraderPlusLogger().LogInfo("ent:"+classname+"created !");
		if(ent)
		{
			ItemBase item = ItemBase.Cast(ent);
			if(!item)return stock;
			GetTraderPlusLogger().LogInfo("ent:exist");
			if(item.GetEconomyProfile())
			{
				GetTraderPlusLogger().LogInfo("ent economy exist !");
				int nominal = item.GetEconomyProfile().GetNominal();
				int qtymax  = TraderPlusHelper.GetMagazineMaxCount(classname);
				int averageqty = qtymax/2;
				GetTraderPlusLogger().LogInfo("ent nominal:"+nominal.ToString()+" qtymax+"+qtymax.ToString()+ " averageqty:"+averageqty.ToString());
				stock = nominal;
				if(averageqty > 0)
				{
					stock = nominal*averageqty;
				}
				if(stock == 0)stock = 100;
			}
		}
		GetGame().ObjectDelete(ent);
		return stock;
	}

  //used to be in deadzone, was originally from Dr.Jones trader mod FileReadHelper
  static string TrimSpaces(string line)
	{
		line.Replace("	", "");
		bool HasSpaces = true;
		while(HasSpaces)
		{
			line = line.Trim();
			if (line.Length() > 0)
				HasSpaces = line[0] == " " || line[line.Length() - 1] == " ";
			else
				HasSpaces = false;
		}
		return line;
	}

  //https://stackoverflow.com/questions/6741883/c-sharp-convert-int-to-currency-string-with-decimal-places
  static string IntToCurrencyString(int number, string separator)
  {
    string moneyReversed = "";

    string strNumber = number.ToString();

    int processedCount = 0;

    for (int i = (strNumber.Length() - 1); i >= 0; i--)
    {
        moneyReversed += strNumber[i];

        processedCount += 1;

        if ((processedCount % 3) == 0 && processedCount < strNumber.Length())
        {
            moneyReversed += separator;
        }
    }

    string money = "";

    for (int j = (moneyReversed.Length() - 1); j >= 0; j--)
    {
        money += moneyReversed[j];
    }

    return money;
  }

  static bool Compare2String(string el1, string el2)
  {
    string lowel1 = el1;
    string lowel2 = el2;
    lowel1.ToLower();
    lowel2.ToLower();

    if(lowel1 == lowel2)
    return true;

    return false;
  }

  static string TrimUnt(string input)
  {
      input.Replace("$UNT$", "");
      return input;
  }

  static string GetDisplayName(string classname)
  {
      string displayName = "";

      if (GetGame().ConfigGetText(CFG_VEHICLESPATH + " " + classname + " displayName", displayName))
      {
          displayName = TrimUnt(displayName);
          return displayName;
      }

      if (GetGame().ConfigGetText(CFG_WEAPONSPATH + " " + classname + " displayName", displayName))
      {
          displayName = TrimUnt(displayName);
          return displayName;
      }

      if (GetGame().ConfigGetText(CFG_MAGAZINESPATH + " " + classname + " displayName", displayName))
      {
          displayName = TrimUnt(displayName);
          return displayName;
      }
      return classname;
  }

  static void RecreateAllAttachments(PlayerBase player, array<EntityAI> attachments)
  {
    #ifdef TRADERPLUSDEBUG
    GetTraderPlusLogger().LogInfo("RecreateAllAttachments => attachmentsC Count:"+attachments.Count().ToString());
    #endif
  	for(int i=0; i<attachments.Count();i++)
  	{
  		ItemBase temp = ItemBase.Cast(attachments.Get(i));
  		if(temp)
  		{
  			int stackQuantity = GetItemAmount(temp);
  			int health        = temp.GetHealthLevel();
  			string classname  = temp.GetType();
  			CreateInInventoryWithState(player, classname, health, stackQuantity);
  		}
  	}
  }

  static array<EntityAI> GetAttachments(EntityAI item)
  {
  			array<EntityAI> attachments = new array<EntityAI>;
  			Weapon_Base wpn = Weapon_Base.Cast(item);
  			if (wpn)
  			{
  				if (wpn.GetInventory().AttachmentCount() > 0 && !wpn.IsKindOf("Magnum_Base"))
  				{
  						int slotCountCorrect = wpn.GetSlotsCountCorrect();
  						for (int i = 0; i < slotCountCorrect; i++)
  						{
  								int slotId = wpn.GetInventory().GetAttachmentSlotId(i);
  								EntityAI attachment = EntityAI.Cast(wpn.GetInventory().FindAttachment(slotId));
  								if (attachment)
  								{
                      #ifdef TRADERPLUSDEBUG
                      GetTraderPlusLogger().LogInfo("GetAttachments => attachment added :"+attachment.GetType());
                      #endif
  										attachments.Insert(attachment);
  								}
  						}
  				}
  			}
  			return attachments;
  }

  static vector snapToGround(vector pos)
  {
      float pos_x = pos[0];
      float pos_z = pos[2];
      float pos_y = GetGame().SurfaceY( pos_x, pos_z );
      vector tmp_pos = Vector( pos_x, pos_y, pos_z );
      tmp_pos[1] = tmp_pos[1] + pos[1];

      return tmp_pos;
  }

  static int GetWeaponAttachmentInHand(PlayerBase player, out array<string>classname, out array<string>count, out array<string>quantity, out array<string>health, out array<bool>attch)
  {
    array<EntityAI> attachments = new array<EntityAI>;
    EntityAI ent = EntityAI.Cast(player.GetHumanInventory().GetEntityInHands());
    if(ent && ent.IsKindOf("Weapon_Base"))
    {
      attachments = GetAttachments(ent);
    }
    else
    {
      if(!ent || !ent.GetInventory() || !ent.GetInventory().GetCargo())return 0;
      int item_count = ent.GetInventory().GetCargo().GetItemCount();
      for(int j=0;j<item_count;j++)
      {
        attachments.Insert(ent.GetInventory().GetCargo().GetItem(j));
      }
    }
    if(attachments.Count() == 0)return 0;
    for(int i=0;i<attachments.Count();i++)
    {
      ItemBase temp = ItemBase.Cast(attachments[i]);
      if(!temp)continue;
      classname.Insert(temp.GetType());
      int amount = GetItemAmount(temp);
      quantity.Insert(amount.ToString());
      count.Insert("1");
      health.Insert(temp.GetHealthLevel().ToString());
      attch.Insert(true);
    }
    return attachments.Count();
  }

  static bool IsInventoryEmpty(ItemBase item)
  {
  	if(item.IsKindOf("Weapon_Base"))return true;
  	if (item.GetNumberOfItems() < 1 &&  item.GetInventory().AttachmentCount() < 1)
  	{
  		return true;
  	}
  	return false;
  }

  static bool CanAddProductToList(string filter, string classname)
	{
		if(filter == "")return true;
		string className = classname;
		string itemName = TraderPlusHelper.GetDisplayName(classname);
		itemName.ToLower();
		className.ToLower();
		if(className.Contains(filter) || itemName.Contains(filter))
		{
			return true;
		}

		return false;
	}

  static bool GetPlayersItems(PlayerBase player, out array<ref TraderPlusArticle> playerItems, string filter = "")
  {
    if (!player)
      return false;
  
    playerItems.Clear();
  
    array<EntityAI> itemsArray = new array<EntityAI>;
    player.GetInventory().EnumerateInventory(InventoryTraversalType.PREORDER, itemsArray);
  
    foreach(EntityAI entity : itemsArray)
    {
      ItemBase temp = ItemBase.Cast(entity);

      if(!temp)
        continue;
  
      if (ShouldSkipItem(temp))
      {
        Print(temp.GetType() + " should be skipped");
        continue;
      }
  
      TraderPlusArticle product = new TraderPlusArticle();
  
      if (temp.IsKindOf("NewReceiptBase"))
      {
        AddNewReceiptProduct(temp, playerItems, filter);
      }
      else if (temp.IsKindOf("Edible_Base"))
      {
        AddEdibleProduct(temp, playerItems, filter);
      }
      else
      {
        AddGenericProduct(temp, playerItems, filter);
      }
    }
  
    return playerItems.Count() > 0;
  }
  
  static bool ShouldSkipItem(ItemBase item)
  {
    if(item == null || item.IsLockedInSlot() || Weapon_Base.Cast(item.GetHierarchyParent()) || IsWearByPlayer(item))
      return true;

    return false;
  }
  
  static void AddNewReceiptProduct(ItemBase item, out array<ref TraderPlusArticle> playerItems, string filter)
  {
      NewReceiptBase carBox = NewReceiptBase.Cast(item);
      string type = carBox.CarClassName;
  
      if (!CanAddProductToList(filter, type))
          return;
  
      TraderPlusArticle product = new TraderPlusArticle();
      product.AddPlayerItems("", type, 1, 1, item.GetHealthLevel(), true);
      playerItems.Insert(product);
  }
  
  static void AddEdibleProduct(ItemBase item, out array<ref TraderPlusArticle> playerItems, string filter)
  {
      Edible_Base edible = Edible_Base.Cast(item);
  
      if (edible.HasFoodStage() && edible.GetFoodStageType() != FoodStageType.RAW)
          return;
  
      TraderPlusArticle product = new TraderPlusArticle();
      int quantity = GetItemAmount(item);
      if (quantity == 0) quantity = 1;
      if (!CanAddProductToList(filter, item.GetType()))
          return;
  
      product.AddPlayerItems("", item.GetType(), 1, quantity, item.GetHealthLevel(), IsInventoryEmpty(item));
      playerItems.Insert(product);
  }
  
  static void AddGenericProduct(ItemBase item, out array<ref TraderPlusArticle> playerItems, string filter)
  {
      TraderPlusArticle product = new TraderPlusArticle();
      int quantity = GetItemAmount(item);
      if (quantity == 0) quantity = 1;
      if (!CanAddProductToList(filter, item.GetType()))
          return;
  
      product.AddPlayerItems("", item.GetType(), 1, quantity, item.GetHealthLevel(), IsInventoryEmpty(item));
      playerItems.Insert(product);
  }

  static void GetPlayersAttachmentsWeight(PlayerBase player)
  {
    for(int i=0;i<playerAttachments.Count();i++)
    {
      EntityAI ent = player.FindAttachmentBySlotName(playerAttachments[i]);
      if(ent)
        float weight = ent.GetWeight();
    }
  }

  static ref array<int> m_AttachmentSlots = {
		InventorySlots.SHOULDER,
		InventorySlots.MELEE,
		InventorySlots.HEADGEAR,
		InventorySlots.MASK,
		InventorySlots.EYEWEAR,
		InventorySlots.GLOVES,
		InventorySlots.ARMBAND,
		InventorySlots.BODY,
		InventorySlots.VEST,
		InventorySlots.BACK,
		InventorySlots.HIPS,
		InventorySlots.LEGS,
		InventorySlots.FEET
	};

  static ref TStringArray playerAttachments = {
    "Head",
    "Shoulder",
    "Melee",
    "Headgear",
    "Mask",
    "Eyewear",
    "Hands",
    "LeftHand",
    "Gloves",
    "Armband",
    "Vest",
    "Body",
    "Back",
    "Hips",
    "Legs",
    "Feet",
    "Splint_Right"
  };

  static int GetSlotForPlayerPreview(string className)
  {
    TStringArray classnameArr = new TStringArray;
    string Vpath = CFG_VEHICLESPATH + " " + className + " inventorySlot";
    if ( GetGame().ConfigIsExisting( Vpath ) )
        g_Game.ConfigGetTextArray( Vpath , classnameArr);

    foreach(string classname: classnameArr)
    {
      foreach(int idx, string playerAtt:playerAttachments)
      {
        if(classname==playerAtt)
            return idx;
      }
    }
    return -1;
  }

static bool IsWearByPlayer(EntityAI entity)
{
    InventoryLocation il = new InventoryLocation;
    entity.GetInventory().GetCurrentInventoryLocation( il );
    if( !il.IsValid() )return true;
    // check the direct parent (clothing will be the parent if not on man attachment)
    if (il.GetParent() && !il.GetParent().IsInherited(Man))
        return false;

    // items in hands are not worn
    if (il.GetType() == InventoryLocationType.HANDS)
        return false;

    return true;
}

  static int GetItemAmount(ItemBase item)
  {
    int quantity = 1;

    if (item.HasQuantity())
    {
      Magazine mag;
      if (Class.CastTo(mag, item))
        quantity = mag.GetAmmoCount();
      else
        quantity = item.GetQuantity();
    }
    return quantity;
  }

  static int GetItemSpecificAmount(ItemBase item)
  {
    int quantity = 1;

    if (item.HasQuantity())
    {
      Magazine mag;
      if (Class.CastTo(mag, item))
      {
        if(mag.IsAmmoPile())
          quantity = mag.GetAmmoCount();
        else
          quantity = 1;
      }
      else
        quantity = item.GetQuantity();
    }
    return quantity;
  }

  static int GetQuantityOfSpecificItem(PlayerBase player, string classname, int healthLevel)
  {
    if(!player)
      return 0;

  	int quantity = 0;
  	array<EntityAI> itemsArray = new array<EntityAI>;
  	player.GetInventory().EnumerateInventory(InventoryTraversalType.PREORDER, itemsArray);
    foreach(EntityAI entity: itemsArray)
    {
      ItemBase item = ItemBase.Cast(entity);

      if (ShouldSkipItem(item) || !CF_String.EqualsIgnoreCase(item.GetType(),classname))
        continue;

      if(EqualHealth(item, healthLevel))
        quantity += GetItemSpecificAmount(item);
    }

    #ifdef TRADERPLUSDEBUG
    GetTraderPlusLogger().LogInfo("GetQuantityOfSpecificItem => quantity :"+quantity.ToString());
    #endif
  	return quantity;
  }

  static bool EqualHealth(ItemBase item, int healthLevel)
  {
    return item.GetHealthLevel() == healthLevel || healthLevel == -1;
  }

  //--------------------------Start Vehicle handler functions---------------------------//
  static int CheckItemType(string classname)
  {
    int returnValue = TraderPlusItemType.ITEM;
    EntityAI ent = EntityAI.Cast(GetGame().CreateObject(classname,Vector(0,0,0)));
    if(ent)
    {
      if(ent.IsInherited(CarScript))
        returnValue=TraderPlusItemType.VEHICLE;

      GetGame().ObjectDelete(ent);
    }

    return returnValue;
  }

  static TStringArray GetVehicleParts(string classname)
  {
    string lowClassName = classname;
    lowClassName.ToLower();

    TStringArray temp = new TStringArray;
    for(int i=0;i<GetTraderPlusVehiculeConfig().VehiclesParts.Count();i++)
    {
      string lowVehicleName = GetTraderPlusVehiculeConfig().VehiclesParts.Get(i).VehicleName;
      lowVehicleName.ToLower();
      if(lowVehicleName == lowClassName)
      {
        #ifdef TRADERPLUSDEBUG
        GetTraderPlusLogger().LogInfo("GetVehicleParts => found!");
        #endif
        temp = GetTraderPlusVehiculeConfig().VehiclesParts.Get(i).VehicleParts;
        return temp;
      }
    }
    return temp;
  }

  static TStringArray GetVehicleAttachmentsFromConfig(string classname)
  {
    string lowClassName = classname;
    lowClassName.ToLower();

    TStringArray attachments = new TStringArray;
    for(int i=0;i<GetTraderPlusVehiculeConfig().VehiclesParts.Count();i++)
    {
      string lowVehicleName = GetTraderPlusVehiculeConfig().VehiclesParts.Get(i).VehicleName;
      lowVehicleName.ToLower();
      if(lowVehicleName == lowClassName)
      {
        #ifdef TRADERPLUSDEBUG
        GetTraderPlusLogger().LogInfo("GetVehicleParts => found!");
        #endif
        attachments = GetTraderPlusVehiculeConfig().VehiclesParts.Get(i).VehicleParts;
        for(int j=0; j<attachments.Count(); j++)
        {
          attachments[j] = attachments[j] + ",-1";
        }
        return attachments;
      }
    }
    return attachments;
  }

  static bool AddVehicleToGarage(PlayerBase player, string classname, int collateralMoney)
  {
    CarScript vehicle = CarScript.Cast(GetGame().CreateObject(classname, vector.Zero, false, false, true));
    if (vehicle)
    {
      string VehicleName = classname;
      int    vehicleData = -1;
      int    cargoId = GetTimeStamp();
      float  VehicleHealth= vehicle.GetHealth();
      if(VehicleHealth == 0)VehicleHealth = -1;
      float  VehicleFuelAmount= vehicle.GetFluidCapacity(CarFluid.FUEL);

      #ifdef MuchCarKey
        vehicleData = 0;
      #endif

      array<ref ItemsCargo> itemsArr = new array<ref ItemsCargo>;
      TStringArray vehicleparts = new TStringArray;
      vehicleparts = GetVehicleParts(classname);
      for (int i = 0; i < vehicleparts.Count(); i++)
      {
          EntityAI attch = vehicle.GetInventory().CreateAttachment(vehicleparts[i]);
          //EntityAI attch = EntityAI.Cast(attchObj);
      }

      GarageItemData cargo = GarageHelpers.GetGarageItemData(vehicle);
      int playerId = GarageHelpers.GetLowSteamID(player.GetIdentity().GetPlainId());
      cargo.Save(playerId, cargoId);
      GetGame().ObjectDelete(vehicle);

      int timestamp = 0;
      if(GetGarageConfig().UseGarageOnlyToTrade)
        timestamp = GetTimeStamp();


      string id = playerId.ToString();
      GarageData garageData = GarageData.Load(id, player.GetIdentity().GetName());
      if(garageData)
      {
        if(GetGarageConfig().UseGarageOnlyToTrade){
          NotificationSystem.SendNotificationToPlayerIdentityExtended(player.GetIdentity(), 2, "Garage", string.Format(GetGarageConfig().TradeVehicleWarning, GetGarageConfig().SavedVehicleInGarageForTradeInHour), "Garage/image/CarLogo.paa");
        }

        garageData.AddVehicle(VehicleName, cargoId, collateralMoney, vehicleData, VehicleHealth, VehicleFuelAmount, itemsArr, timestamp);
        garageData.Save(id);

        TStringArray vehiclesName = new TStringArray;
        TIntArray vehiclesHealth = new TIntArray;
        foreach(VehiculeData car: garageData.Vehicles)
        {
          vehiclesName.Insert(car.VehicleName);
          vehiclesHealth.Insert(GarageHelpers.ConvertHealthToHealthLevel(car.VehicleName, car.EngineHealth));
        }
        GetRPCManager().SendRPC("Garage", "GarageResponse",  new Param4<TStringArray, TIntArray,vector, int>(vehiclesName, vehiclesHealth, vector.Zero, TraderPlusResponse.BUY_SUCCESS),true, player.GetIdentity());
        return true;
      }
    }
    return false;
  }

  static bool RemoveVehicleFromGarage(PlayerBase player, string classname, int position, int health)
  {
    vector ori;
    bool state = false;
    string id = GarageHelpers.GetLowSteamID(player.GetIdentity().GetPlainId()).ToString();
    GarageData garageData = GarageData.Load(id, player.GetIdentity().GetName());
    if(garageData)
    {
      if(garageData.Vehicles.Count() > position)
      {
        if(garageData.Vehicles[position].VehicleName == classname)
        {
            garageData.Vehicles.RemoveOrdered(position);
            garageData.Save(id);
            state  = true;
        }
      }

      if(!state)
      {
        for(int j=0;j<garageData.Vehicles.Count();j++)
        {
          if(garageData.Vehicles[j].VehicleName == classname)
          {
              garageData.Vehicles.RemoveOrdered(j);
              garageData.Save(id);
              state  = true;
          }
        }
      }

      if(!state) return false;

      TStringArray vehiclesName = new TStringArray;
      TIntArray vehiclesHealth = new TIntArray;
      foreach(VehiculeData vehicle: garageData.Vehicles)
      {
        vehiclesName.Insert(vehicle.VehicleName);
        vehiclesHealth.Insert(GarageHelpers.ConvertHealthToHealthLevel(vehicle.VehicleName, vehicle.EngineHealth));
      }

      GetRPCManager().SendRPC("Garage", "GarageResponse",  new Param4<TStringArray,TIntArray,vector, int>(vehiclesName, vehiclesHealth, vector.Zero, TraderPlusResponse.BUY_SUCCESS),true, player.GetIdentity());
      return true;
    }

    return false;
  }

  static int GetHeightIfClassNameInVehicleConfig(string classname)
  {
    string lowClassName = classname;
    lowClassName.ToLower();

    for(int i=0;i<GetTraderPlusVehiculeConfig().VehiclesParts.Count();i++)
    {
      string lowVehicleName = GetTraderPlusVehiculeConfig().VehiclesParts[i].VehicleName;
      lowVehicleName.ToLower();
      if(lowVehicleName==lowClassName)
        return GetTraderPlusVehiculeConfig().VehiclesParts[i].Height;
    }
    return 0;
  }

  static EntityAI TransferInventoryVehicle(EntityAI sourceItem, EntityAI receipt)
  {
		array<EntityAI> children = new array<EntityAI>;
		sourceItem.GetInventory().EnumerateInventory(InventoryTraversalType.LEVELORDER, children);
		foreach (EntityAI child: children)
		{
			if (child)
			{
				if (child.IsRuined())continue;

        #ifdef EXPANSIONMODVEHICLE
    		//Since expansion use 4 integers to store the lock and my mod only store 1, resetting the key is the best I can do guys.
    		ExpansionCarKey key;
    		if (Class.CastTo(key, child) && !key.IsMaster())
    			key.Unpair( true );
    		#endif

				InventoryLocation child_src = new InventoryLocation;
				child.GetInventory().GetCurrentInventoryLocation( child_src );

				InventoryLocation child_dst = new InventoryLocation;
				child_dst.Copy( child_src );
				child_dst.SetParent( receipt );

				if (GameInventory.LocationCanAddEntity(child_dst))
					receipt.GetInventory().TakeToDst(InventoryMode.SERVER, child_src, child_dst);
			}
		}

    return receipt;
  }

  static int GetCollateralPriceInsuranceData(string insuranceData)
  {
    int collateralMoney = 0;
    TStringArray insuranceDataArr= new TStringArray;
    insuranceData.Split(",", insuranceDataArr);
    if(insuranceDataArr[1].ToInt())
      collateralMoney = insuranceDataArr[1].ToInt();

    return collateralMoney;
  }

  static string GetVehicleTypeInsuranceData(string insuranceData)
  {
    string vehicleType = "";
    TStringArray insuranceDataArr = new TStringArray;
    insuranceData.Split(",", insuranceDataArr);
    if(insuranceDataArr[0])
      vehicleType = insuranceDataArr[0];

    return vehicleType;
  }

  static bool CreateVehicle(PlayerBase player, string classname, vector position, vector orientation, array<string> attachments, EntityAI receipt, int carUniqueId, int collateralMoney, int carPassword = 0)
	{
		EntityAI obj = EntityAI.Cast(GetGame().CreateObject(classname, position, false, false, true));
		if (obj)
		{
			obj.SetPosition(position);
			obj.SetOrientation(orientation);
      vector roll = obj.GetOrientation();
      roll[2] = roll[2] - 1;
      obj.SetOrientation(roll);
      roll[2] = roll[2] + 1;
      obj.SetOrientation(roll);

      foreach(string line: attachments)
      {
        #ifdef TRADERPLUSDEBUG
        GetTraderPlusLogger().LogInfo("entity line:"+line);
        #endif
        TStringArray strs = new TStringArray;
        line.Split(",",strs);
        EntityAI att = obj.GetInventory().CreateAttachment(strs[0]);
        if(!att)continue;
        if(strs[1].ToInt() != -1)
          att.SetHealth("", "", strs[1].ToInt());
        #ifdef TRADERPLUSDEBUG
        GetTraderPlusLogger().LogInfo("entity added:"+att.GetType() + " health:"+strs[1]);
        #endif
      }

      CarScript vehicle = CarScript.Cast(obj);
      if(vehicle)
      {
        #ifdef TRADERPLUSDEBUG
        GetTraderPlusLogger().LogInfo("Create vehicle => is vehicle");
        #endif
        TStringArray vehicleparts = new TStringArray;
        vehicleparts = GetVehicleParts(classname);

    		vehicle.SetPosition(vehicle.GetPosition() + Vector(0, 0.3, 0));

        #ifdef TRADERPLUSDEBUG
        GetTraderPlusLogger().LogInfo("GetVehicleParts => refuel!");
        #endif

    		float fuelReq = vehicle.GetFluidCapacity(CarFluid.FUEL);
    		float oilReq = vehicle.GetFluidCapacity(CarFluid.OIL);
    		float coolantReq = vehicle.GetFluidCapacity(CarFluid.COOLANT);
    		float brakeReq = vehicle.GetFluidCapacity(CarFluid.BRAKE);
    		vehicle.Fill(CarFluid.FUEL, fuelReq);
    		vehicle.Fill(CarFluid.OIL, oilReq);
    		vehicle.Fill(CarFluid.COOLANT, coolantReq);
    		vehicle.Fill(CarFluid.BRAKE, brakeReq);

        TraderPlusHelper.TransferInventoryVehicle(receipt, vehicle);

        //CarLock script
        #ifndef CARLOCKDISABLE
            vehicle.SetCarLockOwner(player.CLSteamlowID);
            vehicle.SetCarLock(true);
            if(carPassword!=0)
              vehicle.SetCarLockPassword(carPassword);
        #else
          #ifdef MuchCarKey
            if(carPassword!=0)
            {
              vehicle.m_CarKeyId = carPassword;
              vehicle.m_HasCKAssigned = true;
              vehicle.SetSynchDirty();
            }
          #endif
        #endif

        //setup insurance after being deployed
        if(collateralMoney != 0)
        {
          int insuranceId = carUniqueId;
          TraderPlusBankingData account = player.GetBankAccount();
          if(account)
          {
            account.Insurances.Set(insuranceId, vehicle.GetType() + "," + collateralMoney.ToString());
            vehicle.SetCarUniqueId(insuranceId);
            account.UpdateAccount(player);
          }
        }

        return true;
      }
    }
    return false;
	}
  //--------------------------End Vehicle handler functions---------------------------//

  static int GetMaxItemQuantityServer(string classname)
  {
    EntityAI temp_ent = EntityAI.Cast(GetGame().CreateObject(classname , "0 0 0"));
    if(temp_ent)
    {
      ItemBase temp_item = ItemBase.Cast(temp_ent);
      if(temp_item)
      {
        float currentquantity, minquantity, maxquantity;
        QuantityConversions.GetItemQuantity(temp_item, currentquantity, minquantity, maxquantity);
        if(maxquantity == 0)
            maxquantity = 1;
        GetGame().ObjectDelete(temp_item);
        return maxquantity;
      }
    }
    GetGame().ObjectDelete(temp_ent);
    return 0;
  }

  static int GetMaxItemQuantityClient(string classname)
  {
    string Vpath = CFG_VEHICLESPATH + " " + classname + " varQuantityMax";
    if ( GetGame().ConfigIsExisting( Vpath ) )
        return g_Game.ConfigGetInt( Vpath );

    string Wpath = CFG_WEAPONSPATH + " " + classname + " varQuantityMax";
    if (GetGame().ConfigIsExisting( Wpath ))
        return g_Game.ConfigGetInt( Wpath );

    string Mpath = CFG_MAGAZINESPATH + " " + classname + " varQuantityMax";
    if(classname.Contains("Ammo_") || classname.Contains("Mag_"))
      Mpath = CFG_MAGAZINESPATH + " " + classname + " count";

    if (GetGame().ConfigIsExisting( Mpath ))
        return g_Game.ConfigGetInt( Mpath );

    return 0;
  }

  static int GetItemSlotCount(string classname)
  {
    string Vpath = CFG_VEHICLESPATH + " " + classname + " itemsCargoSize";
    if ( GetGame().ConfigIsExisting( Vpath ) )
    {
      TIntArray Cargo = new TIntArray;
      g_Game.ConfigGetIntArray(Vpath, Cargo);
      return Cargo[0]*Cargo[1];
    }
    return 0;
  }

  static int GetItemSizeCount(string classname)
  {
    string Vpath = CFG_VEHICLESPATH + " " + classname + " itemSize";
    if ( GetGame().ConfigIsExisting( Vpath ) )
    {
      TIntArray Size = new TIntArray;
      g_Game.ConfigGetIntArray(Vpath, Size);
      return Size[0]*Size[1];
    }
    return 0;
  }

  static string GetItemInHand(PlayerBase player)
  {
    Weapon_Base wpn = Weapon_Base.Cast(player.GetHumanInventory().GetEntityInHands());
    if(wpn)
        return wpn.GetType();
    return "";
  }

  static ref TStringArray GetCompatibleAttachmentsArray(string classname)
  {
    TStringArray magazines = new TStringArray;
    string path = CFG_WEAPONSPATH + " " + classname + " magazines";
    if ( GetGame().ConfigIsExisting( path ) )
        g_Game.ConfigGetTextArray(path, magazines);

    TStringArray ammunitions = new TStringArray;
    path = CFG_WEAPONSPATH + " " + classname + " chamberableFrom";
    if ( GetGame().ConfigIsExisting( path ) )
        g_Game.ConfigGetTextArray(path, ammunitions);

    TStringArray optics = new TStringArray;
    path = CFG_WEAPONSPATH + " " + classname + " ironsightsExcludingOptics";
    if ( GetGame().ConfigIsExisting( path ) )
        g_Game.ConfigGetTextArray(path, optics);

    TStringArray sumArray = new TStringArray;
    sumArray.InsertAll(magazines);
    sumArray.InsertAll(ammunitions);
    sumArray.InsertAll(optics);
    return sumArray;
  }

  static int GetMagazineMaxCount(string classname)
  {
    string Mpath = CFG_MAGAZINESPATH + " " + classname + " varQuantityMax";
    if(classname.Contains("Ammo_"))
        Mpath = CFG_MAGAZINESPATH + " " + classname + " count";
    if (GetGame().ConfigIsExisting( Mpath ))
        return g_Game.ConfigGetInt( Mpath );
    return 1;
  }

  //-----------------------Start Create - Delete Handler functions-----------------------------//
  static bool CreateInInventoryWithState(PlayerBase player, string classname,int state = 0, int quantity=1, int collateralMoney = 0)
  {
    string oldClassname="";
    ItemBase item;
    int itemType = CheckItemType(classname);

    if( itemType == TraderPlusItemType.VEHICLE )
    {
      #ifdef TRADERPLUSDEBUG
      GetTraderPlusLogger().LogInfo("CreateInInventoryWithState => ProductIsVehicle!");
      #endif

      if(GetTraderPlusConfigServer().UseGarageToTradeCar)
        return AddVehicleToGarage(player, classname, collateralMoney);

      oldClassname = classname;
      classname="NewReceipt";
      quantity = 1;
      item = CreateInInventory(player,classname,quantity);
      if(item)
      {
        NewReceipt carBox = NewReceipt.Cast(item);
        carBox.SetHealth("","",GetDamageStateFromLevel(state)*item.GetMaxHealth());
        carBox.CarClassName = oldClassname;
        carBox.Attachments = GetVehicleAttachmentsFromConfig(oldClassname);
        if(collateralMoney != 0)
        {
          carBox.CarUniqueId = GetTimeStamp();
          carBox.CollateralMoney = collateralMoney;
        }
        carBox.GetInventory().LockInventory(HIDE_INV_FROM_SCRIPT);
				carBox.SetSynchDirty();
        return true;
      }
    }

    if(itemType == TraderPlusItemType.ITEM)
    {
      item = CreateInInventory(player,classname,quantity);
      if(item)
      {
        item.SetHealth("","",GetDamageStateFromLevel(state)*item.GetMaxHealth());
      	return true;
      }
    }

  	return false;
  }

  static ItemBase CheckForMagToFill(PlayerBase player, string classname, int quantity = 1)
  {
    #ifdef TRADERPLUSDEBUG
    GetTraderPlusLogger().LogInfo("CheckForMagToFill - quantity != 0");
    #endif
    if(!IsAmmunition(classname))return NULL;

    array<EntityAI> itemsArray = new array<EntityAI>;
    array<ItemBase> stackableItems = new array<ItemBase>;
    player.GetInventory().EnumerateInventory(InventoryTraversalType.PREORDER, itemsArray);
    ItemBase ammo = ItemBase.Cast(GetGame().CreateObject(classname, vector.Zero));
    for (int i = 0; i < itemsArray.Count(); ++i)
    {
        ItemBase temp = ItemBase.Cast(itemsArray[i]);
        if (CanLoadMagazine(ammo,temp) && quantity > 0)
        {
            Magazine tmag = Magazine.Cast(temp);
            int current_qty = tmag.GetAmmoCount();
            int can_give_qty = tmag.GetAmmoMax() - current_qty;
            if(can_give_qty > quantity)
            {
              tmag.ServerSetAmmoCount(current_qty + quantity);
              #ifdef TRADERPLUSDEBUG
              GetTraderPlusLogger().LogInfo("Added qty:"+quantity + " to mag with qty:"+current_qty);
              #endif
              return tmag;
            }
            else
            {
              tmag.ServerSetAmmoCount(current_qty + can_give_qty);
              #ifdef TRADERPLUSDEBUG
              GetTraderPlusLogger().LogInfo("Added qty:"+quantity + " to mag with qty:"+current_qty);
              #endif
              quantity -= can_give_qty;
              #ifdef TRADERPLUSDEBUG
              GetTraderPlusLogger().LogInfo("quantity remaining:"+quantity);
              #endif
            }
        }
    }

    GetGame().ObjectDelete(ammo);

    #ifdef TRADERPLUSDEBUG
    GetTraderPlusLogger().LogInfo("quantity remaining:"+quantity);
    #endif

    ItemBase sItem = CheckForStackableItem(player,classname,quantity);
    if(sItem) return sItem;

    ItemBase temp_item = ItemBase.Cast(player.GetInventory().CreateInInventory(classname));

      //if item wasn't created in inventory, we try again to spawn it in any cargo available
    if(!temp_item){
      #ifdef TRADERPLUSDEBUG
      GetTraderPlusLogger().LogInfo("Item wasn't able to be spawned in inventory at first attempt");
      #endif
      temp_item = CreateInCargos(player, classname, quantity);
    }

    if (!temp_item){
      #ifdef TRADERPLUSDEBUG
      GetTraderPlusLogger().LogInfo("item doesn't exist in inv => attempt creation in hand");
      #endif
      temp_item = ItemBase.Cast(player.GetHumanInventory().CreateInHands(classname));
    }

    if (!temp_item){
      #ifdef TRADERPLUSDEBUG
      GetTraderPlusLogger().LogInfo("item doesn't exist in inv => attempt creation in ground");
      #endif
      //temp_item = ItemBase.Cast(GetGame().CreateObjectEx(classname, player.GetPosition(), ECE_PLACE_ON_SURFACE));
      temp_item = ItemBase.Cast(player.SpawnEntityOnGroundPos(classname, player.GetPosition()));
    }

    if (temp_item){
      #ifdef TRADERPLUSDEBUG
      GetTraderPlusLogger().LogInfo("item exist in inventory or ground => ");
      #endif
      return SetQuantity(temp_item, quantity);
    }

    #ifdef TRADERPLUSDEBUG
    GetTraderPlusLogger().LogInfo("item doesn't exist at all...");
    #endif

    return NULL;
  }

  static bool IsAmmunition(string classname)
  {
    bool isAmmo = false;

    ItemBase item = ItemBase.Cast(GetGame().CreateObject(classname, vector.Zero));
    Magazine mag;
    Ammunition_Base ammo;

    if(CastTo(ammo, item))
      isAmmo = true;

    GetGame().ObjectDelete(item);

    return isAmmo;
  }

  static bool IsMagazine(string classname)
  {
    bool isMag = false;
    ItemBase item = ItemBase.Cast(GetGame().CreateObject(classname, vector.Zero));
    Magazine mag;
    Ammunition_Base ammo;

    if(CastTo(mag, item) && !CastTo(ammo, item))
      isMag = true;

    GetGame().ObjectDelete(item);

    return isMag;
  }

  static ItemBase CheckForStackableItem(PlayerBase player, string classname, int quantity = 1)
  {
    #ifdef TRADERPLUSDEBUG
    GetTraderPlusLogger().LogInfo("CheckForStackableItem - quantity != 0");
    #endif

    if(IsMagazine(classname))return NULL;

    array<EntityAI> itemsArray = new array<EntityAI>;
    array<ItemBase> stackableItems = new array<ItemBase>;
    player.GetInventory().EnumerateInventory(InventoryTraversalType.PREORDER, itemsArray);
    for (int i = 0; i < itemsArray.Count(); ++i)
    {
        ItemBase temp = ItemBase.Cast(itemsArray[i]);
        if (temp && temp.GetType() == classname)
        {
            #ifdef TRADERPLUSDEBUG
            GetTraderPlusLogger().LogInfo(temp.GetType() + "=" + classname);
            #endif
            float currentquantity, minquantity, maxquantity;
            QuantityConversions.GetItemQuantity(temp, currentquantity, minquantity, maxquantity);
            #ifdef TRADERPLUSDEBUG
            GetTraderPlusLogger().LogInfo(temp.GetType()+" currentquantity="+currentquantity.ToString() + " minquantity="+minquantity.ToString() + " maxquantity="+maxquantity.ToString());
            #endif
            if (maxquantity==0)
            {
              currentquantity = 1;
              maxquantity = 1;
            }
            if ((currentquantity + quantity) <= maxquantity)
                stackableItems.Insert(temp);
        }
    }

    if (stackableItems.Count() > 0)
    {
      ItemBase item = stackableItems[0];
      #ifdef TRADERPLUSDEBUG
      GetTraderPlusLogger().LogInfo("item is stackable => add quantity:"+quantity.ToString());
      #endif
      return AddQuantity(item, quantity);
    }

    return NULL;
  }

  static ItemBase CreateInCargos(PlayerBase player, string classname, int quantity = 1)
  {
    #ifdef TRADERPLUSDEBUG
    GetTraderPlusLogger().LogInfo("Create In Cargos");
    #endif
    ItemBase item;
    array<EntityAI> itemsArray = new array<EntityAI>;
    player.GetInventory().EnumerateInventory(InventoryTraversalType.PREORDER, itemsArray);
    for(int j = 0; j < itemsArray.Count(); j++)
    {
      Class.CastTo(item, itemsArray.Get(j));
      if(!item)continue;
      EntityAI newItem = EntityAI.Cast(item.GetInventory().CreateInInventory(classname)); //CreateEntityInCargo
      if (newItem)
      {
        #ifdef TRADERPLUSDEBUG
        GetTraderPlusLogger().LogInfo("Item succesfully spawned in inventory at second attempt");
        #endif
        return ItemBase.Cast(newItem);
      }
    }
    return NULL;
  }

  static ItemBase CreateInInventory(PlayerBase player, string classname, int quantity = 1, bool skipFillMag = false)
  {
  		if(!player)return NULL;

      int maxQuantity = GetMaxItemQuantityServer(classname);
      if(quantity < 0) quantity = maxQuantity;

      if(quantity > maxQuantity)
      {
        int remainingqty = quantity-maxQuantity;
        quantity=maxQuantity;
        CreateInInventory(player, classname, remainingqty, skipFillMag);
      }

      #ifdef TRADERPLUSDEBUG
      GetTraderPlusLogger().LogInfo("item :"+classname + "qty: "+quantity.ToString());
      #endif

      if(quantity != 0 && !skipFillMag)
      {
        ItemBase itemM = CheckForMagToFill(player, classname, quantity);
        if(itemM)
        {
          #ifdef TRADERPLUSDEBUG
          GetTraderPlusLogger().LogInfo("itemM return:");
          #endif
          return itemM;
        }
      }

      #ifdef TRADERPLUSDEBUG
      GetTraderPlusLogger().LogInfo("itemM dont exist:");
      #endif

      if(quantity != 0){
        ItemBase item = CheckForStackableItem(player, classname, quantity);
        if(item)return item;
      }

  		ItemBase temp_item = ItemBase.Cast(player.GetInventory().CreateInInventory(classname));

      //if item wasn't created in inventory, we try again to spawn it in any cargo available
      if(!temp_item){
        #ifdef TRADERPLUSDEBUG
        GetTraderPlusLogger().LogInfo("Item wasn't able to be spawned in inventory at first attempt");
        #endif
        temp_item = CreateInCargos(player, classname, quantity);
      }

  		if (!temp_item){
        #ifdef TRADERPLUSDEBUG
        GetTraderPlusLogger().LogInfo("item doesn't exist in inv => attempt creation in hand");
        #endif
  			temp_item = ItemBase.Cast(player.GetHumanInventory().CreateInHands(classname));
      }

  		if (!temp_item){
        #ifdef TRADERPLUSDEBUG
        GetTraderPlusLogger().LogInfo("item doesn't exist in inv => attempt creation in ground");
        #endif
        //temp_item = ItemBase.Cast(GetGame().CreateObjectEx(classname, player.GetPosition(), ECE_PLACE_ON_SURFACE));
        temp_item = ItemBase.Cast(player.SpawnEntityOnGroundPos(classname, player.GetPosition()));
      }

  		if (temp_item){
        #ifdef TRADERPLUSDEBUG
        GetTraderPlusLogger().LogInfo("item exist in inventory or ground => ");
        #endif
        return SetQuantity(temp_item, quantity);
      }

      #ifdef TRADERPLUSDEBUG
      GetTraderPlusLogger().LogInfo("item doesn't exist at all...");
      #endif

  		return NULL;
  }

  static bool RemoveOurProduct(PlayerBase player, string classname, int quantity, int health = -1, bool handcheck = false, int multiplier = 1)
  {
    string vehicleclassname = "";
    //we start to check if our product is a vehicle, if it is, we delete the receipt based on the car's id
    if(CheckItemType(classname) == TraderPlusItemType.VEHICLE)
    {
      vehicleclassname = classname;
      classname="NewReceipt";
      quantity=multiplier;
    }

    //if quantity is -1, we determine max quantity of the item
    if(quantity < 0)
        quantity = GetMaxItemQuantityServer(classname);

    int itemSpecificCount = GetQuantityOfSpecificItem(player,classname, health);

    //if we don't have enough quantity in inventory, we don't delete anything to avoid any issue
    if( itemSpecificCount < quantity || itemSpecificCount == 0)
    {
      #ifdef TRADERPLUSDEBUG
      GetTraderPlusLogger().LogInfo("item doesn't exist or quantity to low");
      #endif
      return false;
    }

    array<EntityAI> attachments   = new array<EntityAI>;
    array<EntityAI> itemsArray = new array<EntityAI>;
    player.GetInventory().EnumerateInventory(InventoryTraversalType.PREORDER, itemsArray);

    foreach(EntityAI entity: itemsArray)
    {
      ItemBase temp = ItemBase.Cast(entity);

      //if item is lock, continue
      if(temp && temp.IsLockedInSlot())continue;

      //check if entity is wear by player, if yes, continue
      if(IsWearByPlayer(entity))continue;

      if( !handcheck && Weapon_Base.Cast(entity.GetHierarchyParent()))continue;

      if (temp && !temp.IsInherited(SurvivorBase) && (temp.GetHealthLevel() == health || health == -1))
      {
          #ifdef TRADERPLUSDEBUG
          GetTraderPlusLogger().LogInfo(temp.GetType() +"=?"+classname);
          #endif
          if (temp.GetType() == classname)
          {
            #ifdef TRADERPLUSDEBUG
            GetTraderPlusLogger().LogInfo("temp.GetType() == classname "+temp.GetType() + " == " + classname + " v:"+vehicleclassname);
            #endif
            if(vehicleclassname!="")
            {
              NewReceipt receipt = NewReceipt.Cast(temp);
              if(receipt){
              #ifdef TRADERPLUSDEBUG
              GetTraderPlusLogger().LogInfo("receipt:"+receipt.CarClassName + " vs " + vehicleclassname);
              #endif
              }
              if(receipt && receipt.CarClassName == vehicleclassname)
              {
                #ifdef TRADERPLUSDEBUG
                GetTraderPlusLogger().LogInfo("receipt:"+receipt.CarClassName + " vs " + vehicleclassname);
                #endif
                GetGame().ObjectDelete(entity);
                quantity--;
                if(quantity!= 0)
                  continue;
                else
                  return true;
              }
              else
              {
                continue;
              }
            }
            if (temp.IsKindOf("Edible_Base"))
            {
              Edible_Base edible = Edible_Base.Cast(temp);
              if (edible.HasFoodStage() && edible.GetFoodStageType() != FoodStageType.RAW)
                  continue;
            }
            if ((QuantityConversions.HasItemQuantity(temp) == 0) || (temp.IsInherited(Magazine) && !temp.IsInherited(Ammunition_Base)))
            {
                Magazine mag = Magazine.Cast(temp);
                if(mag)EmptyMag(player,mag);
                attachments = GetAttachments(temp);
                quantity--;
                GetGame().ObjectDelete(entity);
                #ifdef TRADERPLUSDEBUG
                GetTraderPlusLogger().LogInfo("item without quantity => deleted");
                #endif
            }
            else
            {
              if (GetItemAmount(temp) > quantity && quantity!=0)
              {
                  temp = AddQuantity(temp,quantity*-1);
                  #ifdef TRADERPLUSDEBUG
                  GetTraderPlusLogger().LogInfo("item with quantity => quantity removed:"+quantity.ToString());
                  #endif
                  quantity=0;
              }
              else
              {
                  attachments = GetAttachments(temp);
                  quantity -= GetItemAmount(temp);
                  GetGame().ObjectDelete(entity);
                  #ifdef TRADERPLUSDEBUG
                  GetTraderPlusLogger().LogInfo("item with quantity => item & quantity removed:"+quantity.ToString());
                  #endif
              }
            }
            if(attachments && attachments.Count()>0){
              RecreateAllAttachments(player,attachments);
              #ifdef TRADERPLUSDEBUG
              GetTraderPlusLogger().LogInfo("recreate att");
              #endif
            }
            if(quantity <= 0)
            {
              #ifdef TRADERPLUSDEBUG
              GetTraderPlusLogger().LogInfo("quantity <= 0"+quantity.ToString());
              #endif
              return true;
            }
          }
      }
    }
    if(quantity == 0)
    {
      #ifdef TRADERPLUSDEBUG
      GetTraderPlusLogger().LogInfo("quantity == 0"+quantity.ToString());
      #endif
      return true;
    }
    if(!handcheck) return RemoveOurProduct(player, classname, quantity, health, true);
    return false;
  }
  //-----------------------End Create - Delete Handler functions-----------------------------//

  static ItemBase AddQuantity(ItemBase item, int quantity)
  {
    Magazine mag = Magazine.Cast(item);
    if (mag)
        mag.ServerSetAmmoCount(mag.GetAmmoCount() + quantity);
    else
      item.SetQuantity((QuantityConversions.GetItemQuantity(item) + quantity));
    return item;
  }

  static ItemBase SetQuantity(ItemBase item, int quantity)
  {
    Magazine mag = Magazine.Cast(item);
    if (mag)
        mag.ServerSetAmmoCount(quantity);
    else
      item.SetQuantity(quantity);
    return item;
  }

  //--------------------------Money handler functions---------------------------//
  static bool CheckifPlayerHasEnoughMoney(PlayerBase player, int price, int id, out int playerMoneyAmount)
  {
    playerMoneyAmount = GetPlayerMoney(player, id);
    if(playerMoneyAmount<price)
      return false;

    return true;
  }

  static bool AddMoneyToPlayer(PlayerBase player, int amount)
  {
    int AmountRemaining = amount;
    int AmountGiven = 0;
    int AmountToGive = 0;

    for (int i = 0; i < GetTraderPlusConfigServer().Currencies.Count(); i++)
  	{
      string currenciesName = GetTraderPlusConfigServer().Currencies.Get(i).ClassName;
      int    currenciesValue = GetTraderPlusConfigServer().Currencies.Get(i).Value;
      TStringArray strs = new TStringArray;
      currenciesName.Split(",", strs );

      #ifdef TRADERPLUSDEBUG
      GetTraderPlusLogger().LogInfo("AddMoneyToPlayer: no specific money require");
      #endif
      if( currenciesValue <= AmountRemaining)
      {
        AmountToGive = AmountRemaining/currenciesValue;
        if(AmountToGive > 0)
        {
          CreateInInventory(player, strs[0], AmountToGive);
          AmountGiven += AmountToGive*currenciesValue;
          AmountRemaining -= AmountToGive*currenciesValue;
          AmountToGive = 0;
        }
      }
    }
    return true;
  }

  static bool ModdedAddMoneyToPlayer(PlayerBase player, int amount)
  {
    int AmountRemaining = amount;
    int AmountGiven = 0;
    int AmountToGive = 0;

    for (int i = 0; i < GetTraderPlusConfigServer().Currencies.Count(); i++)
  	{
      string currenciesName = GetTraderPlusConfigServer().Currencies.Get(i).ClassName;
      int    currenciesValue = GetTraderPlusConfigServer().Currencies.Get(i).Value;
      TStringArray strs = new TStringArray;
      currenciesName.Split(",", strs );

      #ifdef TRADERPLUSDEBUG
      GetTraderPlusLogger().LogInfo("AddMoneyToPlayer: no specific money require");
      #endif
      if( currenciesValue <= AmountRemaining)
      {
        AmountToGive = AmountRemaining/currenciesValue;
        if(AmountToGive > 0)
        {
          CreateInInventory(player, strs[0], AmountToGive);
          AmountGiven += AmountToGive*currenciesValue;
          AmountRemaining -= AmountToGive*currenciesValue;
          AmountToGive = 0;
        }
      }
    }
    return true;
  }

  static bool ModdedRemoveMoneyFromPlayer(PlayerBase player, int moneyamount)
  {
  	int moneyamountTaken = 0;
    int currentCurrencyQuantity = 0;
    int take = 0;
    bool state = false;
  	int moneyamountRemaining = moneyamount;
    int count = -1 + GetTraderPlusConfigServer().Currencies.Count();
  	for (int i = count; i >= 0 ; i--)
  	{
        string currenciesName = GetTraderPlusConfigServer().Currencies.Get(i).ClassName;
        int    currenciesValue= GetTraderPlusConfigServer().Currencies.Get(i).Value;
        TStringArray strs = new TStringArray;
        currenciesName.Split(",", strs );
        for(int j=0;j<strs.Count();j++)
        {
          currentCurrencyQuantity = GetQuantityOfSpecificItem(player, strs[j], -1);
          take = 0;
          while ((moneyamountRemaining > 0) && (take < currentCurrencyQuantity))
          {
            take++;
            moneyamountRemaining -= currenciesValue;
            moneyamountTaken += currenciesValue;
          }

          if (take > 0)
          {
              state = RemoveOurProduct(player, strs[j], take);
              if (!state)
                return false;
          }
        }
  	}
  	if (moneyamountTaken > moneyamount)
  	{
  		int difference = moneyamountTaken - moneyamount;
      ModdedAddMoneyToPlayer(player, difference);
  	}
  	return true;
  }

  static int ModdedGetPlayerMoney(PlayerBase player)
  {
      int amount = 0;
      float quantity = 0.0;
      int value=0;
      array<EntityAI> itemsArray = new array<EntityAI>;
      player.GetInventory().EnumerateInventory(InventoryTraversalType.INORDER, itemsArray);
      for (int i = 0; i < itemsArray.Count(); i++)
      {
          ItemBase temp = ItemBase.Cast(itemsArray.Get(i));
          if (temp)
          {
              string classname = temp.GetType();
              for (int j = 0; j < GetTraderPlusConfigServer().Currencies.Count(); j++)
              {
                  string currenciesName = GetTraderPlusConfigServer().Currencies.Get(j).ClassName;
                  TStringArray strs = new TStringArray;
                  currenciesName.Split( ",", strs );
                  for(int k=0;k<strs.Count();k++)
                  {
                    if (strs[k]==classname)
                    {
                        quantity = TraderPlusHelper.GetItemAmount(temp);
                        value = GetTraderPlusConfigServer().Currencies.Get(j).Value;
                        amount += (value * quantity);
                    }
                  }
              }
          }
      }
      return amount;
  }

  //Get player's money based on the accepted currencies by the trader
  static int GetPlayerMoney(PlayerBase player, int id)
  {
      if(!player)return 0;

      int amount = 0;
      float qty = 0.0;
      int value = 0;

      array<EntityAI> itemsArray = new array<EntityAI>;
      player.GetInventory().EnumerateInventory(InventoryTraversalType.INORDER, itemsArray);
      for (int i = 0; i < itemsArray.Count(); i++)
      {
          if(IsWearByPlayer(itemsArray[i]))
            continue;
            
          ItemBase item = ItemBase.Cast(itemsArray.Get(i));
          if (item)
          {
              string className = item.GetType();
              for (int j = 0; j < GetTraderPlusConfigClient().MoneyName.Count(); j++)
              {
                TStringArray traderCurrencyArray = new TStringArray;
                GetTraderPlusConfigClient().MoneyName.Get(j).Split( ",", traderCurrencyArray );
                for(int k=0;k<traderCurrencyArray.Count();k++)
                {
                  if (traderCurrencyArray.Get(k) == className)
                  {
                    TStringArray acceptedCurrencyArray = new TStringArray;
                    string currency = GetTraderPlusConfigClient().CurrenciesAccepted.Get(id);
                    currency.Split( ",", acceptedCurrencyArray );
                    if(acceptedCurrencyArray.Count() == 0)
                    {
                      qty = TraderPlusHelper.GetItemAmount(item);
                      value = GetTraderPlusConfigClient().MoneyValue.Get(j);
                      amount += (value * qty);
                    }
                    else
                    {
                      for(int l=0;l<acceptedCurrencyArray.Count();l++)
                      {
                        if(className == acceptedCurrencyArray[l])
                        {
                          qty = TraderPlusHelper.GetItemAmount(item);
                          value = GetTraderPlusConfigClient().MoneyValue.Get(j);
                          amount += (value * qty);
                        }
                      }
                    }
                  }
                }
              }
          }
      }

      GetTraderPlusLogger().LogInfo("Player:"+player.GetIdentity().GetName() + " - currency amount:"+amount);
      return amount;
  }

  /*static int GetPlayerMoney(PlayerBase player, int id)
  {
      int amount = 0;
      float quantity = 0.0;
      int value=0;
      array<EntityAI> itemsArray = new array<EntityAI>;
      player.GetInventory().EnumerateInventory(InventoryTraversalType.INORDER, itemsArray);
      for (int i = 0; i < itemsArray.Count(); i++)
      {
          ItemBase temp = ItemBase.Cast(itemsArray.Get(i));
          if (temp)
          {
              string classname = temp.GetType();
              for (int j = 0; j < GetTraderPlusConfigServer().Currencies.Count(); j++)
              {
                  string currenciesName = GetTraderPlusConfigServer().Currencies.Get(j).ClassName;
                  TStringArray strs = new TStringArray;
                  currenciesName.Split( ",", strs );
                  for(int k=0;k<strs.Count();k++)
                  {
                    if (strs[k]==classname)
                    {
                      if(GetTraderPlusIDConfig().IDs[id].CurrenciesAccepted.Count() == 0)
                      {
                        quantity = GetItemAmount(temp);
                        value = GetTraderPlusConfigServer().Currencies.Get(j).Value;
                        amount += (value * quantity);
                        continue;
                      }

                      //Multi currency part
                      for(int l=0;l<GetTraderPlusIDConfig().IDs[id].CurrenciesAccepted.Count();l++)
                      {
                        if(GetTraderPlusIDConfig().IDs[id].CurrenciesAccepted[l]==classname)
                        {
                          quantity = GetItemAmount(temp);
                          value = GetTraderPlusConfigServer().Currencies.Get(j).Value;
                          amount += (value * quantity);
                        }
                      }
                    }
                  }
              }
          }
      }
      return amount;
  }*/

  static bool AddMoneyToPlayer(PlayerBase player, int amount, int id)
  {
    int AmountRemaining = amount;
    int AmountGiven = 0;
    int AmountToGive = 0;

    for (int i = 0; i < GetTraderPlusConfigServer().Currencies.Count(); i++)
  	{
      string currenciesName = GetTraderPlusConfigServer().Currencies.Get(i).ClassName;
      int    currenciesValue = GetTraderPlusConfigServer().Currencies.Get(i).Value;
      TStringArray strs = new TStringArray;
      currenciesName.Split(",", strs );

      if(GetTraderPlusIDConfig().IDs[id].CurrenciesAccepted.Count() == 0)
      {
        #ifdef TRADERPLUSDEBUG
        GetTraderPlusLogger().LogInfo("AddMoneyToPlayer: no specific money require");
        #endif
        if( currenciesValue <= AmountRemaining)
        {
          #ifdef TRADERPLUSDEBUG
          GetTraderPlusLogger().LogInfo("AddMoneyToPlayer: "+currenciesValue.ToString()+"<="+AmountRemaining.ToString());
          #endif
          AmountToGive = AmountRemaining/currenciesValue;
          if(AmountToGive > 0)
          {
            #ifdef TRADERPLUSDEBUG
            GetTraderPlusLogger().LogInfo("AddMoneyToPlayer: Amount to give"+AmountToGive.ToString());
            #endif
            CreateInInventory(player, strs[0], AmountToGive);
            AmountGiven += AmountToGive*currenciesValue;
            AmountRemaining -= AmountToGive*currenciesValue;
            AmountToGive = 0;
          }
        }
      }
      else
      {
        #ifdef TRADERPLUSDEBUG
        GetTraderPlusLogger().LogInfo("AddMoneyToPlayer: specific money required");
        #endif
        for(int j=0;j<GetTraderPlusIDConfig().IDs[id].CurrenciesAccepted.Count();j++)
        {
          int pos = strs.Find(GetTraderPlusIDConfig().IDs[id].CurrenciesAccepted[j]);
          if(pos == -1)continue;
          #ifdef TRADERPLUSDEBUG
          GetTraderPlusLogger().LogInfo("AddMoneyToPlayer: we found the money to give:"+strs[pos]);
          #endif
          AmountToGive = AmountRemaining/currenciesValue;
          if(AmountToGive > 0)
          {
            #ifdef TRADERPLUSDEBUG
            GetTraderPlusLogger().LogInfo("AddMoneyToPlayer: Amount to give"+AmountToGive.ToString());
            #endif
            CreateInInventory(player, strs[pos], AmountToGive);
            AmountGiven += AmountToGive*currenciesValue;
            AmountRemaining -= AmountToGive*currenciesValue;
            AmountToGive = 0;
          }
        }
      }
    }
    #ifdef TRADERPLUSDEBUG
    GetTraderPlusLogger().LogInfo("AddMoneyToPlayer: amountgiven:"+AmountGiven.ToString());
    #endif
    return true;
  }

  static bool RemoveMoneyFromPlayer(PlayerBase player, int moneyamount, int id)
  {
  	int moneyamountTaken = 0;
    int currentCurrencyQuantity = 0;
    int take = 0;
    bool state = false;
  	int moneyamountRemaining = moneyamount;
    int count = -1 + GetTraderPlusConfigServer().Currencies.Count();
  	for (int i = count; i >= 0 ; i--)
  	{
        string currenciesName = GetTraderPlusConfigServer().Currencies.Get(i).ClassName;
        int    currenciesValue= GetTraderPlusConfigServer().Currencies.Get(i).Value;
        TStringArray strs = new TStringArray;
        currenciesName.Split(",", strs );
        for(int j=0;j<strs.Count();j++)
        {
          if(GetTraderPlusIDConfig().IDs[id].CurrenciesAccepted.Count() == 0)
          {
            currentCurrencyQuantity = GetQuantityOfSpecificItem(player, strs[j], -1);
            #ifdef TRADERPLUSDEBUG
            GetTraderPlusLogger().LogInfo("currentCurrencyQuantity"+currentCurrencyQuantity.ToString());
            #endif
            take = 0;
            while ((moneyamountRemaining > 0) && (take < currentCurrencyQuantity))
            {
              take++;
              moneyamountRemaining -= currenciesValue;
              moneyamountTaken += currenciesValue;
            }

            if (take > 0)
            {
              #ifdef TRADERPLUSDEBUG
              GetTraderPlusLogger().LogInfo("[MONEY TAKEN]: "+take.ToString() + " TYPE:"+strs[j]);
              #endif
              state = RemoveOurProduct(player, strs[j], take);
              if (!state)
                return false;
            }
          }
          else
          {
            //Multi currency part
            for(int l=0;l<GetTraderPlusIDConfig().IDs[id].CurrenciesAccepted.Count();l++)
            {
              if(GetTraderPlusIDConfig().IDs[id].CurrenciesAccepted[l]==strs[j])
              {
                currentCurrencyQuantity = GetQuantityOfSpecificItem(player, strs[j], -1);
            		take = 0;
            		while ((moneyamountRemaining > 0) && (take < currentCurrencyQuantity))
            		{
            			take++;
            			moneyamountRemaining -= currenciesValue;
            			moneyamountTaken += currenciesValue;
            		}
            		if (take > 0)
            		{
                  #ifdef TRADERPLUSDEBUG
                  GetTraderPlusLogger().LogInfo("[MONEY TAKEN]-CA: "+take.ToString() + " TYPE:"+strs[j]);
                  #endif
            			state = RemoveOurProduct(player, strs[j], take);
            			if (!state)
            				return false;
            		}
              }
            }
          }
        }
  	}

    #ifdef TRADERPLUSDEBUG
    GetTraderPlusLogger().LogInfo("[MONEY TAKEN]: "+moneyamountTaken.ToString() + " DEFAULT:"+moneyamount.ToString());
    #endif
  	if (moneyamountTaken > moneyamount)
  	{
  		int difference = moneyamountTaken - moneyamount;
      #ifdef TRADERPLUSDEBUG
      GetTraderPlusLogger().LogInfo("[MONEY GIVEN BACK]: "+difference.ToString());
      #endif
      AddMoneyToPlayer(player, difference, id);
  	}
  	return true;
  }

  //-------------------------- End Money handler functions---------------------------//

  //--------------------------Health handler functions---------------------------//
  static float GetDamageStateFromLevel(int level)
  {
  	switch(level)
  	{
  		case GameConstants.STATE_PRISTINE:
  			return GameConstants.DAMAGE_PRISTINE_VALUE;
  			break;

  		case GameConstants.STATE_BADLY_DAMAGED:
  			return GameConstants.DAMAGE_BADLY_DAMAGED_VALUE;
  			break;

  		case GameConstants.STATE_DAMAGED:
  			return GameConstants.DAMAGE_DAMAGED_VALUE;
  			break;

  		case GameConstants.STATE_WORN:
  			return GameConstants.DAMAGE_WORN_VALUE;
  			break;

  		case GameConstants.STATE_RUINED:
  			return GameConstants.DAMAGE_RUINED_VALUE;
  			break;

  		default: return GameConstants.DAMAGE_PRISTINE_VALUE;
  			break;
  	}
	   return GameConstants.DAMAGE_PRISTINE_VALUE;
  }

  static string GetHealthFromLevel(int level, out int ca, out int cr, out int cg, out int cb)
  {
    switch(level)
  	{
  		case GameConstants.STATE_PRISTINE:
        {
          ca=255;
          cr=40;
          cg=159;
          cb=70;
          return "#tpm_pristine";
        }
  			break;

  		case GameConstants.STATE_BADLY_DAMAGED:
        {
          ca=255;
          cr=161;
          cg=87;
          cb=13;
          return "#tpm_badly_dmg";
        }
  			break;

  		case GameConstants.STATE_DAMAGED:
        {
          ca=255;
          cr=212;
          cg=149;
          cb=86;
          return "#tpm_damaged";
        }
  			break;

  		case GameConstants.STATE_WORN:
        {
            ca=255;
            cr=129;
            cg=207;
            cb=149;
            return "#tpm_worn";
        }
  			break;

  		case GameConstants.STATE_RUINED:
        {
          ca=255;
          cr=159;
          cg=40;
          cb=40;
          return "#tpm_ruined";
        }
  			break;

  		default:
        {
          ca=255;
          cr=255;
          cg=255;
          cb=255;
          return "default";
        }
  			break;
  	}

    ca=255;
    cr=255;
    cg=255;
    cb=255;
    return "default";
  }
  //--------------------------End Health handler functions---------------------------//
}
