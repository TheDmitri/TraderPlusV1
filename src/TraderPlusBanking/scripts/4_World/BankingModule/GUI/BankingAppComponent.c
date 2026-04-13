class BankingAppComponent extends DZlrComponentBase
{
    private static ref ComponentAutoRegister s_Register = new ComponentAutoRegister("banking-app-component", BankingAppComponent);

    // Bound properties
    string feesText;
    string bankAmountDisplay;
    string playerAmountText;
    string debugText;
    string playerIdText;
    string transferBankAmountText;
    int progressValue;

    // Panels loaded via AutoLoadWidgets
    Widget PlayerAccountPanel;
    Widget TransfertPanel;

    // Form — auto-discovered by EditBoxBinding via m_FormGroup naming convention
    ref FormGroup m_FormGroup;

    // Internal state
    private int m_MoneyAmountOnPlayer;
    private int m_MaxMoneyBank;
    private int m_MoneyBankAmount;
    private string m_AccountID;
    private ref TransactionThrottle m_Throttle;

    override protected void RegisterMetadata()
    {
        m_Metadata = new ComponentMetadata();
        m_Metadata.selector = "banking-app-component";
        m_Metadata.templatePath = "TraderPlusBanking/datasets/layouts/BankingAppComponent.layout";
    }

    override void OnInit()
    {
        m_Throttle = new TransactionThrottle(TRADERPLUS_BANK_TRANSACTION_INTERVAL);
        m_MoneyAmountOnPlayer = 0;
        m_MaxMoneyBank = 0;
        m_MoneyBankAmount = 0;

        // FormGroup — EditBoxBinding auto-creates controls from layout Form_Validators
        m_FormGroup = new FormGroup("bankingForm");

        // Set initial display — UIHandle called by service once RPC data arrives
        m_AccountID = GetGame().GetUserManager().GetTitleInitiator().GetUid();
        playerIdText = m_AccountID;

        int fees = 100 * GetTraderPlusBankingConfigClient().TransactionFees;
        feesText = "#tpb_fees " + fees.ToString() + "%";

        debugText = "";
        bankAmountDisplay = "...";
        playerAmountText = "...";
        transferBankAmountText = "...";
        progressValue = 0;

        MarkForCheck();

        GetGame().GetCallQueue(CALL_CATEGORY_GUI).CallLater(PeriodicUpdate, 500, true);
    }

    override void OnDestroy()
    {
        GetGame().GetCallQueue(CALL_CATEGORY_GUI).Remove(PeriodicUpdate);
    }

    void PeriodicUpdate()
    {
        MoneyUpdate();
        m_Throttle.Update(0.5);
    }

    void UIHandle()
    {
        if (!GetBankAccount())
        {
            return;
        }

        m_AccountID = GetGame().GetUserManager().GetTitleInitiator().GetUid();

        int fees = 100 * GetTraderPlusBankingConfigClient().TransactionFees;
        feesText = "#tpb_fees " + fees.ToString() + "%";

        playerIdText = m_AccountID;

        m_MoneyAmountOnPlayer = 0;
        m_MaxMoneyBank = GetBankAccount().MaxAmount;
        m_MoneyBankAmount = GetBankAccount().MoneyAmount;

        string stringMoneyBank = TraderPlusHelper.IntToCurrencyString(m_MoneyBankAmount, ",");
        bankAmountDisplay = stringMoneyBank;
        transferBankAmountText = stringMoneyBank;

        MoneyUpdate();

        progressValue = 0;
        if (m_MaxMoneyBank != 0)
        {
            progressValue = (m_MoneyBankAmount * 100) / m_MaxMoneyBank;
        }

        MarkForCheck();
    }

    void MoneyUpdate()
    {
        m_MoneyAmountOnPlayer = TraderPlusMoneyHelper.GetPlayerMoneyFromInventory();
        string stringMoney = TraderPlusHelper.IntToCurrencyString(m_MoneyAmountOnPlayer, ",");
        playerAmountText = stringMoney;
        MarkForCheck();
    }

    // --- Tab switching ---

    void OnAccountTabClicked(ClickEventArgs args)
    {
        if (PlayerAccountPanel)
        {
            PlayerAccountPanel.Show(true);
        }
        if (TransfertPanel)
        {
            TransfertPanel.Show(false);
        }
        UIHandle();
    }

    void OnTransferTabClicked(ClickEventArgs args)
    {
        if (PlayerAccountPanel)
        {
            PlayerAccountPanel.Show(false);
        }
        if (TransfertPanel)
        {
            TransfertPanel.Show(true);
        }

        if (GetBankAccount())
        {
            m_MoneyBankAmount = GetBankAccount().MoneyAmount;
            string stringMoneyBank = TraderPlusHelper.IntToCurrencyString(m_MoneyBankAmount, ",");
            transferBankAmountText = stringMoneyBank;
        }
        MarkForCheck();
    }

    // --- Deposit ---

    void OnDepositClicked(ClickEventArgs args)
    {
        if (!m_Throttle.CanRequest())
        {
            debugText = "#tpm_not_too_fast";
            MarkForCheck();
            return;
        }

        FormControl depositControl = m_FormGroup.GetControl("depositAmount");
        if (!depositControl)
        {
            return;
        }

        depositControl.MarkAsTouched();
        depositControl.Validate();
        if (!depositControl.IsValid())
        {
            debugText = "#tpb_number_only";
            MarkForCheck();
            return;
        }

        int AmountAttempt = depositControl.GetValue().ToInt();

        if (AmountAttempt > m_MoneyAmountOnPlayer)
        {
            debugText = "#tpm_dont_enough_money";
            MarkForCheck();
            return;
        }

        if (AmountAttempt <= 0)
        {
            AmountAttempt = TraderPlusMoneyHelper.GetPlayerMoneyFromInventory();
        }

        if (AmountAttempt > m_MaxMoneyBank || (AmountAttempt + m_MoneyBankAmount > m_MaxMoneyBank))
        {
            debugText = "#tpb_over_max";
            MarkForCheck();
            return;
        }

        GetRPCManager().SendRPC("TraderPlusBanking", "TraderPlusBankingDeposit", new Param1<int>(AmountAttempt), true, NULL);
        PlayerBase player = PlayerBase.Cast(GetGame().GetPlayer());
        if (player)
        {
            player.PlayTradeSound(true);
        }
        m_Throttle.OnRequestSent();
    }

    // --- Withdraw ---

    void OnWithdrawClicked(ClickEventArgs args)
    {
        if (!m_Throttle.CanRequest())
        {
            debugText = "#tpm_not_too_fast";
            MarkForCheck();
            return;
        }

        FormControl depositControl = m_FormGroup.GetControl("depositAmount");
        if (!depositControl)
        {
            return;
        }

        depositControl.MarkAsTouched();
        depositControl.Validate();
        if (!depositControl.IsValid())
        {
            debugText = "#tpb_number_only";
            MarkForCheck();
            return;
        }

        int AmountAttempt = depositControl.GetValue().ToInt();

        if (AmountAttempt == 0)
        {
            return;
        }

        if (AmountAttempt > m_MoneyBankAmount)
        {
            debugText = "#tpm_dont_enough_money";
            MarkForCheck();
            return;
        }

        GetRPCManager().SendRPC("TraderPlusBanking", "TraderPlusBankingWithdraw", new Param1<int>(AmountAttempt), true, NULL);
        PlayerBase player = PlayerBase.Cast(GetGame().GetPlayer());
        if (player)
        {
            player.PlayTradeSound(false);
        }
        m_Throttle.OnRequestSent();
    }

    // --- Transfer ---

    void OnTransferClicked(ClickEventArgs args)
    {
        if (!m_Throttle.CanRequest())
        {
            debugText = "#tpm_not_too_fast";
            MarkForCheck();
            return;
        }

        FormControl amountControl = m_FormGroup.GetControl("transferAmount");
        FormControl idControl = m_FormGroup.GetControl("transferId");

        if (!amountControl || !idControl)
        {
            return;
        }

        m_FormGroup.MarkAllAsTouched();
        amountControl.Validate();
        idControl.Validate();

        if (!amountControl.IsValid())
        {
            debugText = "#tpm_only_number";
            MarkForCheck();
            return;
        }

        if (!idControl.IsValid())
        {
            debugText = "#tpm_destination_account_incorrect";
            MarkForCheck();
            return;
        }

        string destinationIDAccount = idControl.GetValue();
        int amountAttempt = amountControl.GetValue().ToInt();

        if (destinationIDAccount == m_AccountID)
        {
            debugText = "#tpm_destination_account_incorrect";
            MarkForCheck();
            return;
        }

        if (amountAttempt <= 0)
        {
            if (GetBankAccount())
            {
                amountAttempt = GetBankAccount().MoneyAmount;
            }
        }

        if (amountAttempt > m_MoneyBankAmount)
        {
            debugText = "#tpm_not_enough_money_in_bank";
            MarkForCheck();
            return;
        }

        GetRPCManager().SendRPC("TraderPlusBanking", "TraderPlusTransfertRequest", new Param2<string, int>(destinationIDAccount, amountAttempt), true, NULL);
        PlayerBase player = PlayerBase.Cast(GetGame().GetPlayer());
        if (player)
        {
            player.PlayTradeSound(false);
        }
        m_Throttle.OnRequestSent();
    }

    // --- Copy ID ---

    void OnCopyIdClicked(ClickEventArgs args)
    {
        GetGame().CopyToClipboard(m_AccountID);
    }

    // --- Input change events (called by EditBoxBinding OnChange) ---

    void OnDepositAmountChanged()
    {
        debugText = "";
        FormControl control = m_FormGroup.GetControl("depositAmount");
        if (!control)
        {
            return;
        }

        control.Validate();
        if (!control.IsValid())
        {
            debugText = "#tpb_number_only";
            MarkForCheck();
            return;
        }

        int AmountAttempt = control.GetValue().ToInt();
        int AmountMinusFees = AmountAttempt - AmountAttempt * GetTraderPlusBankingConfigClient().TransactionFees;
        bankAmountDisplay = m_MoneyBankAmount.ToString() + "(+" + AmountMinusFees.ToString() + "/-" + AmountAttempt.ToString() + ")";
        MarkForCheck();
    }

    void OnTransferAmountChanged()
    {
        debugText = "";
        FormControl control = m_FormGroup.GetControl("transferAmount");
        if (!control)
        {
            return;
        }

        control.Validate();
        if (!control.IsValid())
        {
            debugText = "#tpb_number_only";
            MarkForCheck();
            return;
        }

        int AmountAttempt = control.GetValue().ToInt();
        int AmountPlusFees = AmountAttempt + AmountAttempt * GetTraderPlusBankingConfigClient().TransactionFees;
        transferBankAmountText = m_MoneyBankAmount.ToString() + "(-" + AmountPlusFees.ToString() + ")";
        MarkForCheck();
    }
}
