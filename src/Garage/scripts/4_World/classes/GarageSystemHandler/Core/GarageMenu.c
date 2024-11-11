class GarageMenu extends UIScriptedMenu
{
  private   ButtonWidget      m_BttnParkIn;
  private   ButtonWidget      m_BttnParkOut;
  private   TextListboxWidget m_GarageList;
  private   TextWidget        m_TextCostParkIn;
  private   TextWidget        m_TextCostParkOut;
  private   TextWidget        m_TextMoney;

  private   ImageWidget       m_ParkingStatut;

  private   ItemPreviewWidget m_MainItemPreview;

            bool              m_IsFlag;
            vector            m_Position;
            vector            m_ParkingPos;
            int               m_LowUID;
            bool              m_PayWithBankAccount;

  private   float             m_transactionTick;
  private   bool              m_canTradeRequest;


  private   ref TStringArray  m_VehiclesName;
  private   ref TStringArray  m_VehiclesListName;

  private   int               m_MoneyAmountOnPlayer;
  private   int               m_ParkInFee;
  private   int               m_ParkOutFee;

  private   CarScript         m_CarInPark;

  private   int               m_SelectedVehicle;

  private   EntityAI          m_previewItem;

  void GarageMenu()
  {
    m_MoneyAmountOnPlayer = 0;
    m_CarInPark = NULL;
    m_VehiclesName = new TStringArray;
    m_VehiclesListName = new TStringArray;
  }

  void Show(bool rep)
  {
    if(rep)
    {
      GetGame().GetInput().ChangeGameFocus(1);
      GetGame().GetUIManager().ShowUICursor(true);
      GetGame().GetMission().GetHud().Show(false);
      OnShow();
    }
    else
    {
      GetGame().GetInput().ResetGameFocus();
      GetGame().GetUIManager().ShowUICursor(false);
      GetGame().GetMission().GetHud().Show(true);
      OnHide();
    }
    layoutRoot.Show(rep);
  }

  override void OnShow()
	{
		super.OnShow();
		PPEffects.SetBlurMenu(0.5);
		GetGame().GetInput().ChangeGameFocus(1);
		SetFocus( layoutRoot );
	}

	override void OnHide()
	{
		super.OnHide();
		PPEffects.SetBlurMenu(0);
		GetGame().GetInput().ResetGameFocus();
    if(m_previewItem)
      GetGame().ObjectDelete(m_previewItem);
		Close();
	}

  override Widget Init()
  {
		    layoutRoot = GetGame().GetWorkspace().CreateWidgets( "Garage/gui/GarageUI.layout" );
        m_BttnParkIn = ButtonWidget.Cast( layoutRoot.FindAnyWidget( "BttnParkIn" ) );
        m_BttnParkOut = ButtonWidget.Cast( layoutRoot.FindAnyWidget( "BttnParkOut" ) );
        m_GarageList = TextListboxWidget.Cast( layoutRoot.FindAnyWidget( "GarageList" ) );
        m_TextCostParkIn  = TextWidget.Cast(layoutRoot.FindAnyWidget( "TextCostParkIn" ));
        m_TextCostParkOut  = TextWidget.Cast(layoutRoot.FindAnyWidget( "TextCostParkOut" ));
        m_TextMoney  = TextWidget.Cast(layoutRoot.FindAnyWidget( "TextMoney" ));
        m_ParkingStatut  = ImageWidget.Cast(layoutRoot.FindAnyWidget( "ParkingStatut" ));
        layoutRoot.Show(false);
        return layoutRoot;
    }

  void UIHandle()
  {
    m_SelectedVehicle = -1;
    m_GarageList.ClearItems();
    m_VehiclesListName.Clear();

    m_CarInPark = GarageHelpers.CheckForCarAtPos(m_ParkingPos);
    if(m_CarInPark)
    {
      #ifdef GMDEBUG
      GetGMLogger().LogInfo("Garage UI m_CarInPark");
      #endif
      m_GarageList.AddItem(m_CarInPark.GetType(), NULL, 0);
      m_GarageList.SetItemColor(0, 0, ARGB(255, 191, 48, 48) );
      m_VehiclesListName.Insert(m_CarInPark.GetType());
    }

    for(int i=0;i<m_VehiclesName.Count();i++)
    {
      #ifdef GMDEBUG
      GetGMLogger().LogInfo(m_VehiclesName[i]);
      #endif
      int pos = i;
      if(m_CarInPark)pos++;
      m_GarageList.AddItem(m_VehiclesName[i], NULL, 0);
      m_GarageList.SetItemColor(pos, 0, ARGB(255, 255, 255, 255) );
      m_VehiclesListName.Insert(m_VehiclesName[i]);
    }

    MoneyUpdate();

    m_ParkInFee = GetGarageConfig().ParkInCost;
    m_ParkOutFee= GetGarageConfig().ParkOutCost;
    m_TextCostParkIn.SetText("#gm_cost_park_in" + " " + m_ParkInFee.ToString());
    m_TextCostParkOut.SetText("#gm_cost_park_out" + " " +m_ParkOutFee.ToString());

    if(m_ParkingPos == Vector(-1,-1,-1) || m_ParkingPos == Vector(0,0,0) || m_CarInPark)
    {
      m_ParkingStatut.SetColor(ARGB(255,191,48,48));
    }else m_ParkingStatut.SetColor(ARGB(255,0,255,0));

    if(m_VehiclesListName.Count() > 0)
    {
      m_SelectedVehicle = 0;
      UpdateVehiclePreview(m_VehiclesListName[m_SelectedVehicle]);
    }
  }

  void MoneyUpdate()
  {
    if(m_PayWithBankAccount){
      if(GetBankAccount())
      m_MoneyAmountOnPlayer = GetBankAccount().MoneyAmount;
    }else m_MoneyAmountOnPlayer = GetPlayerMoney();
    string stringMoney = TraderPlusHelper.IntToCurrencyString(m_MoneyAmountOnPlayer, ",");
    m_TextMoney.SetText(stringMoney);
  }

  int GetPlayerMoney()
  {
      PlayerBase player = PlayerBase.Cast(GetGame().GetPlayer());
      if(!player)return 0;

      int amount = 0;
      float qty;
      int value;
      array<EntityAI> itemsArray = new array<EntityAI>;
      player.GetInventory().EnumerateInventory(InventoryTraversalType.INORDER, itemsArray);
      for (int i = 0; i < itemsArray.Count(); i++)
      {
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
                    if(GetTraderPlusBankingConfigClient().CurrenciesAccepted.Count() == 0)
                    {
                      qty = TraderPlusHelper.GetItemAmount(item);
                      value = GetTraderPlusConfigClient().MoneyValue.Get(j);
                      amount += (value * qty);
                      continue;
                    }

                    for(int l=0;l<GetTraderPlusBankingConfigClient().CurrenciesAccepted.Count();l++)
                    {
                      if(GetTraderPlusBankingConfigClient().CurrenciesAccepted[l]==traderCurrencyArray[k])
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
      return amount;
  }

  void SetResponseData(TStringArray vehicles, vector parkpos)
  {
    m_VehiclesName.Clear();
    m_VehiclesName = vehicles;
    m_ParkingPos = parkpos;
    m_PayWithBankAccount = GetGarageConfig().PayWithBankAccount;
    if(m_PayWithBankAccount)
    GetRPCManager().SendRPC("TraderPlusBanking", "TraderPlusBankingRequest",  NULL, true, NULL);
  }

  override void Update(float timeslice)
  {
      if(!m_canTradeRequest)
      {
        m_transactionTick -= timeslice;
        if (m_transactionTick <= 0)
        {
              m_transactionTick = TRADERPLUS_BANK_TRANSACTION_INTERVAL;
              m_canTradeRequest = true;
        }
      }
  }

  override bool OnClick (Widget w, int x, int y, int button)
  {
    switch(w)
    {
      case m_BttnParkIn: ParkInHandler();
      return true;
      break;

      case m_BttnParkOut: ParkOutHandler();
      return true;
      break;

      case m_GarageList: VehicleListHandler();
      return true;
      break;
    }
    return false;
  }

  void VehicleListHandler()
  {
    m_SelectedVehicle = m_GarageList.GetSelectedRow();
    if(m_SelectedVehicle == -1)return;
    UpdateVehiclePreview(m_VehiclesListName[m_SelectedVehicle]);
  }

  void UpdateVehiclePreview(string itemType)
	{
    string tempstr = itemType;
    tempstr.ToLower();
		if ( !m_MainItemPreview )
			{
				Widget preview_frame = layoutRoot.FindAnyWidget("VehiclePreview");

				if ( preview_frame )
				{
					float width;
					float height;
					preview_frame.GetSize(width, height);

					m_MainItemPreview = ItemPreviewWidget.Cast( GetGame().GetWorkspace().CreateWidget(ItemPreviewWidgetTypeID, 0, 0, 1, 1, WidgetFlags.VISIBLE, ARGB(255, 255, 255, 255), 10, preview_frame) );
				}
			}

			if ( m_previewItem )
				GetGame().ObjectDelete( m_previewItem );

			m_previewItem = EntityAI.Cast(GetGame().CreateObject( itemType, "0 0 0", true, false, true ));
      if(!m_previewItem)return;

      m_MainItemPreview.SetItem(m_previewItem);
      m_MainItemPreview.SetModelPosition( Vector( 0, 0, 0.5 ) );
      m_MainItemPreview.SetModelOrientation( vector.Zero );
      m_MainItemPreview.SetView( m_previewItem.GetViewIndex() );
	}

  void ParkInHandler()
  {
      if(!m_CarInPark)return;
      if(GetGarageConfig().VehicleMustHaveLock)
      {
        if(!GarageHelpers.CanParkVehicle(m_CarInPark))return;
      }
      if(m_VehiclesName.Count() + 1 > GetGarageConfig().MaxVehicleStored)
      {
        NotificationSystem.AddNotificationExtended( 2, "Garage", GetGarageConfig().MaxVehicleStoredReached, "Garage/image/CarLogo.paa" );
        return;
      }
      if(m_MoneyAmountOnPlayer<m_ParkInFee)
      {
        NotificationSystem.AddNotificationExtended( 2, "Garage", GetGarageConfig().NotEnoughMoney, "Garage/image/CarLogo.paa" );
        //Not enough Money
        return;
      }
      if(!m_canTradeRequest)return;
      GetRPCManager().SendRPC("Garage", "ParkInRequest",  new Param3<int, CarScript, vector>(m_LowUID, m_CarInPark,m_ParkingPos), true, NULL);
      m_canTradeRequest = false;
  }

  void ParkOutHandler()
  {
    if(m_CarInPark)
    {
      NotificationSystem.AddNotificationExtended( 2, "Garage", GetGarageConfig().ParkingNotAvailable, "Garage/image/CarLogo.paa" );
      //vehicle is blocking garage
      return;
    }
    if(m_MoneyAmountOnPlayer<m_ParkInFee)
    {
      NotificationSystem.AddNotificationExtended( 2, "Garage", GetGarageConfig().NotEnoughMoney, "Garage/image/CarLogo.paa" );
      //Not enough Money
      return;
    }
    if(!m_canTradeRequest)return;
    GetRPCManager().SendRPC("Garage", "ParkOutRequest",  new Param4<int, int, string, vector>(m_LowUID,m_SelectedVehicle,m_VehiclesName[m_SelectedVehicle],m_ParkingPos), true, NULL);
    m_canTradeRequest = false;
  }
};
