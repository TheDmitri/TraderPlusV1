
class TraderPlusBankingMenu extends UIScriptedMenu
{
  //Player's panel widget
  private   Widget            m_PlayerAccountPanel;
  private   ButtonWidget      m_PlayerAccountBttn;
  private   ButtonWidget      m_PlayerWithdrawBttn;
  private   ButtonWidget      m_PlayerDepositBttn;
  private   RichTextWidget    m_PlayerDebugText;
  private   TextWidget        m_BankAmountText;
  private   TextWidget        m_PlayerFees;
  private   TextWidget        m_PlayerAmountText;
  private   TextWidget        m_PlayerAmountTextOutput;
  private   TextWidget        m_TransactionAmountText;
  private   TextWidget        m_BankAmountTextInput;
  private   TextWidget        m_MaxText;
  private   EditBoxWidget     m_PlayerAmountTextInput;
  private   ProgressBarWidget  m_MoneyProgressBar;

  private   int               m_MoneyAmountOnPlayer;
  private   int               m_MaxMoneyBank;
  private   int               m_MoneyBankAmount;

  private   string            m_AccountID;

  private   bool              m_canTradeRequest;

  private   float             m_UIUpdate;
  private   float             m_transactionTick;

  //Clan's panel widget

  //Player's transfer widgets
  private ButtonWidget       m_PtPTransfertBttn;
  private ButtonWidget       m_TransfertBttn;
  private Widget             m_TransfertPanel;

  private TextWidget        m_TransfertBankAmountText;
  private TextWidget        m_TransfertBankAmountTextOutput;
  private TextWidget        m_TransfertAmountText;
  private EditBoxWidget     m_TransfertAmountTextInput;
  private TextWidget        m_TransfertIDText;
  private EditBoxWidget     m_TransfertIDTextInput;

  //Player ID Account widgets
  private TextWidget        m_PlayerIDAccountText;
  private ButtonWidget      m_CopyIDBttn;


  void TraderPlusBankingMenu()
  {
    m_UIUpdate            = 0.0;
    m_transactionTick     = 0.0;
    m_MoneyAmountOnPlayer = 0;
    m_canTradeRequest     = true;
    m_MaxMoneyBank = 0;
    m_MoneyBankAmount = 0;
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
    UIHandle();
	}

	override void OnHide()
	{
		super.OnHide();
		PPEffects.SetBlurMenu(0);
		GetGame().GetInput().ResetGameFocus();
		Close();
	}

  override Widget Init()
    {
        layoutRoot = GetGame().GetWorkspace().CreateWidgets( GetTraderPlusBankLayoutPath() );

        //Player's panel widget
        m_PlayerAccountPanel = Widget.Cast( layoutRoot.FindAnyWidget( "PlayerAccountPanel" ) );
        m_PlayerAccountBttn  = ButtonWidget.Cast( layoutRoot.FindAnyWidget( "PlayerAccountBttn" ) );
        m_PlayerWithdrawBttn = ButtonWidget.Cast( layoutRoot.FindAnyWidget( "PlayerWithdrawBttn" ) );
        m_PlayerDepositBttn = ButtonWidget.Cast( layoutRoot.FindAnyWidget( "PlayerDepositBttn" ) );
        m_PlayerAmountTextInput = EditBoxWidget.Cast( layoutRoot.FindAnyWidget( "PlayerAmountTextInput" ) );
        m_PlayerDebugText = RichTextWidget.Cast(layoutRoot.FindAnyWidget( "PlayerDebugText" ));
        m_PlayerFees = TextWidget.Cast(layoutRoot.FindAnyWidget( "PlayerFees" ));
        m_PlayerAmountTextOutput = TextWidget.Cast(layoutRoot.FindAnyWidget( "PlayerAmountTextOutput" ));
        m_BankAmountTextInput = TextWidget.Cast(layoutRoot.FindAnyWidget( "BankAmountTextInput" ));
        m_MoneyProgressBar = ProgressBarWidget.Cast(layoutRoot.FindAnyWidget( "MoneyProgressBar" ));

        //Transfer's panel widget
        m_PtPTransfertBttn = ButtonWidget.Cast(layoutRoot.FindAnyWidget("PtPTransfertBttn"));
        m_TransfertBttn = ButtonWidget.Cast(layoutRoot.FindAnyWidget("TransfertBttn"));
        m_TransfertPanel = Widget.Cast( layoutRoot.FindAnyWidget("TransfertPanel"));
        m_TransfertBankAmountText = TextWidget.Cast(layoutRoot.FindAnyWidget("TransfertBankAmountText"));
        m_TransfertBankAmountTextOutput = TextWidget.Cast(layoutRoot.FindAnyWidget("TransfertBankAmountTextOutput"));
        m_TransfertAmountText = TextWidget.Cast(layoutRoot.FindAnyWidget("TransfertAmountText"));
        m_TransfertAmountTextInput = EditBoxWidget.Cast( layoutRoot.FindAnyWidget( "TransfertAmountTextInput"));
        m_TransfertIDText = TextWidget.Cast(layoutRoot.FindAnyWidget("TransfertIDText"));
        m_TransfertIDTextInput = EditBoxWidget.Cast( layoutRoot.FindAnyWidget( "TransfertIDTextInput"));

        m_PlayerIDAccountText = TextWidget.Cast(layoutRoot.FindAnyWidget("PlayerIDAccountText"));
        m_CopyIDBttn = ButtonWidget.Cast(layoutRoot.FindAnyWidget("CopyIDBttn"));

        layoutRoot.Show(false);
        return layoutRoot;
    }

  string GetTraderPlusBankLayoutPath()
  {
    return "TraderPlusBanking/gui/BankUI.layout";
  }

  void UIHandle()
  {
    #ifdef TRADERPLUSDEBUG
    GetTraderPlusLogger().LogInfo("BankingBankMenu-UIHandle ");
    #endif

    m_AccountID = GetGame().GetUserManager().GetTitleInitiator().GetUid();

    int fees = 100*GetTraderPlusBankingConfigClient().TransactionFees;
    m_PlayerFees.SetText("#tpb_fees"+" "+fees.ToString()+"%");

    m_PlayerIDAccountText.SetText(m_AccountID);

    m_MoneyAmountOnPlayer = 0;
    m_MaxMoneyBank = GetBankAccount().MaxAmount;
    m_MoneyBankAmount = GetBankAccount().MoneyAmount;

    string stringMoneyBank = TraderPlusHelper.IntToCurrencyString(m_MoneyBankAmount, ",");
    m_BankAmountTextInput.SetText(stringMoneyBank);
    m_TransfertBankAmountTextOutput.SetText(stringMoneyBank);
    m_PlayerAmountTextInput.SetText("");
    MoneyUpdate();

    #ifdef TRADERPLUSDEBUG
    GetTraderPlusLogger().LogInfo("BankingBankMenu-UIHandle MaxMoney"+m_MaxMoneyBank.ToString());
    GetTraderPlusLogger().LogInfo("BankingBankMenu-UIHandle MoneyBankAmount"+m_MoneyBankAmount.ToString());
    #endif
    int progressValue = 0;
    if(m_MaxMoneyBank!=0)
      progressValue = (m_MoneyBankAmount * 100)/m_MaxMoneyBank;

    #ifdef TRADERPLUSDEBUG
    GetTraderPlusLogger().LogInfo("BankingBankMenu-UIHandle progressvalue"+progressValue.ToString());
    #endif

    m_MoneyProgressBar.SetCurrent(progressValue);
    m_UIUpdate = 1.0;
  }

  void MoneyUpdate()
  {
    m_MoneyAmountOnPlayer = GetPlayerMoney();
    string stringMoney = TraderPlusHelper.IntToCurrencyString(m_MoneyAmountOnPlayer, ",");
    m_PlayerAmountTextOutput.SetText(stringMoney);
  }

  override void Update(float timeslice)
  {
      if(m_UIUpdate > 0.0)
      {
        MoneyUpdate();
        m_UIUpdate -= timeslice;
      }

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

  static int GetPlayerMoney()
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

  void  PtPTransfertHandler()
  {
    m_PlayerAccountPanel.Show(false);
    m_TransfertPanel.Show(true);

    m_MoneyBankAmount = GetBankAccount().MoneyAmount;
    string stringMoneyBank = TraderPlusHelper.IntToCurrencyString(m_MoneyBankAmount, ",");
    m_TransfertBankAmountTextOutput.SetText(stringMoneyBank);
  }

  void TransfertHandler()
  {
    if(!m_canTradeRequest)
    {
      m_PlayerDebugText.SetText("#tpm_not_too_fast");
      return;
    }

    string  amount = m_TransfertAmountTextInput.GetText();
    int amountAttempt = amount.ToInt();

    string destinationIDAccount = m_TransfertIDTextInput.GetText();

    if(destinationIDAccount == "" || destinationIDAccount.Length() < 16 || destinationIDAccount == m_AccountID)
    {
      m_PlayerDebugText.SetText("#tpm_destination_account_incorrect");
      return;
    }

    if(HasCharacterInAmount(amount) || HasCharacterInAmount(destinationIDAccount))
    {
      m_PlayerDebugText.SetText("#tpm_only_number");
      return;
    }

    if(amountAttempt <= 0)
    {
      amountAttempt = GetBankAccount().MoneyAmount;
    }

    if(amountAttempt > m_MoneyBankAmount)
    {
      m_PlayerDebugText.SetText("#tpm_not_enough_money_in_bank");
      return;
    }

    GetRPCManager().SendRPC("TraderPlusBanking", "TraderPlusTransfertRequest",  new Param2<string,int>(destinationIDAccount,amountAttempt), true, NULL);
    PlayerBase player = PlayerBase.Cast(GetGame().GetPlayer());
    player.PlayTradeSound(false);
    m_canTradeRequest = false;
  }

  void PlayerAccountHandler()
  {
    m_PlayerAccountPanel.Show(true);
    m_TransfertPanel.Show(false);
    UIHandle();
  }

  override bool OnClick (Widget w, int x, int y, int button)
  {
    switch(w)
    {
      case m_PlayerAccountBttn: PlayerAccountHandler();
      return true;
      break;

      case m_PtPTransfertBttn: PtPTransfertHandler();
      return true;
      break;

      case m_PlayerWithdrawBttn: WithdrawHandler();
      return true;
      break;

      case m_PlayerDepositBttn: DepositHandler();
      return true;
      break;

      case m_TransfertBttn: TransfertHandler();
      return true;
      break;

      case m_CopyIDBttn: CopyIDHandler();
      return true;
      break;
    }
    return false;
  }

  void CopyIDHandler()
  {
    GetGame().CopyToClipboard(m_AccountID);
  }

  bool OnChange(Widget w, int x, int y, bool finished)
	{
		super.OnChange(w,x,y,finished);

    string  amount;
    int AmountAttempt;
    int AmountMinusFees;

		if (w == m_PlayerAmountTextInput)
		{
      amount = m_PlayerAmountTextInput.GetText();
      AmountAttempt = amount.ToInt();
      m_PlayerDebugText.SetText("");
      if(HasCharacterInAmount(amount))
      {
        m_PlayerDebugText.SetText("#tpb_number_only");
        return false;
      }
      AmountMinusFees = AmountAttempt - AmountAttempt*GetTraderPlusBankingConfigClient().TransactionFees;
      m_BankAmountTextInput.SetText(m_MoneyBankAmount.ToString()+"(+"+AmountMinusFees.ToString()+"/-"+AmountAttempt.ToString()+")");
      return true;
    }

    if (w == m_TransfertAmountTextInput)
		{
      amount = m_TransfertAmountTextInput.GetText();
      AmountAttempt = amount.ToInt();
      m_PlayerDebugText.SetText("");
      if(HasCharacterInAmount(amount))
      {
        m_PlayerDebugText.SetText("#tpb_number_only");
        return false;
      }
      int AmountPlusFees = AmountAttempt + AmountAttempt*GetTraderPlusBankingConfigClient().TransactionFees;
      m_TransfertBankAmountTextOutput.SetText(m_MoneyBankAmount.ToString()+"(-"+AmountPlusFees.ToString()+")");
      return true;
    }

    return false;
	}

  void WithdrawHandler()
  {
    if(!m_canTradeRequest)
    {
      m_PlayerDebugText.SetText("#tpm_not_too_fast");
      return;
    }

    string  amount = m_PlayerAmountTextInput.GetText();
    int AmountAttempt = amount.ToInt();

    if(HasCharacterInAmount(amount))
    {
      m_PlayerDebugText.SetText("#tpb_number_only");
      return;
    }

    if(AmountAttempt == 0)
      return;

    if(AmountAttempt > m_MoneyBankAmount)
    {
      m_PlayerDebugText.SetText("#tpm_dont_enough_money");
      return;
    }

    GetRPCManager().SendRPC("TraderPlusBanking", "TraderPlusBankingWithdraw",  new Param1<int>(AmountAttempt), true, NULL);
    PlayerBase player = PlayerBase.Cast(GetGame().GetPlayer());
    player.PlayTradeSound(false);
    m_canTradeRequest = false;
  }

  void DepositHandler()
  {
    if(!m_canTradeRequest)
    {
      m_PlayerDebugText.SetText("#tpm_not_too_fast");
      return;
    }

    string  amount = m_PlayerAmountTextInput.GetText();
    int AmountAttempt = amount.ToInt();

    if(HasCharacterInAmount(amount))
    {
      m_PlayerDebugText.SetText("#tpb_number_only");
      return;
    }

    if(AmountAttempt > m_MoneyAmountOnPlayer)
    {
      m_PlayerDebugText.SetText("#tpm_dont_enough_money");
      return;
    }

    if(AmountAttempt <= 0)
    {
      AmountAttempt = GetPlayerMoney();
    }

    if(AmountAttempt > m_MaxMoneyBank || (AmountAttempt + m_MoneyBankAmount > m_MaxMoneyBank))
    {
      m_PlayerDebugText.SetText("#tpb_over_max");
      return;
    }

    GetRPCManager().SendRPC("TraderPlusBanking", "TraderPlusBankingDeposit",  new Param1<int>(AmountAttempt), true, NULL);
    PlayerBase player = PlayerBase.Cast(GetGame().GetPlayer());
    player.PlayTradeSound(true);
    m_canTradeRequest = false;
  }

  bool HasCharacterInAmount(string pwd)
  {
    for(int i=0;i<pwd.Length();i++)
    {
      switch(pwd[i])
      {
        case "0":continue;
        break;

        case "1":continue;
        break;

        case "2":continue;
        break;

        case "3":continue;
        break;

        case "4":continue;
        break;

        case "5":continue;
        break;

        case "6":continue;
        break;

        case "7":continue;
        break;

        case "8":continue;
        break;

        case "9":continue;
        break;

        default:return true;
        break;
      }
    }
    return false;
  }
};
