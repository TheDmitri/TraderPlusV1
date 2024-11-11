class TraderPlusBankingServer
{
    float BankingTick;
    ref BankingTransactionQueue TransactionsQueue;

    void TraderPlusBankingServer()
    {
      BankingTick = 0.0;
      TransactionsQueue = new BankingTransactionQueue;
      GetTraderPlusLogger().LogInfo("TraderPlusBankingServer - Started");
      //I usually make a small delay to make sure that my static config class is available
    }

  void  ~TraderPlusBankingServer(){}

  void OnUpdate(float timeslice)
  {
    BankingTick+=timeslice;
    if(BankingTick > TRADERPLUS_QUEUE_TRANSACTION_INTERVAL)
    {
        if(TransactionsQueue.HasNextQueue())
        {
          TransactionRequest(TransactionsQueue.Peek());
          TransactionsQueue.DeQueue();
        }
      BankingTick = 0;
    }
  }

  void TransactionRequest(TraderPlusBankingTransaction transaction)
  {
    int newAmount;
    TraderPlusBankingData account = transaction.Customer.GetBankAccount();
    switch(transaction.TransactionMode)
    {
      case TP_BANK_TRANSACTION_DEPOSIT:
      {
        if(account)
        {
          if(!TraderPlusBankHelpers.CheckifPlayerHasEnoughMoney(transaction.Customer, transaction.Amount))return;
          int amountMinusFees = transaction.Amount - transaction.Amount*GetTraderPlusBankConfigServer().TransactionFees;
          newAmount = account.MoneyAmount + amountMinusFees;
          account.MoneyAmount = newAmount;
          account.UpdateAccount(transaction.Customer);
          TraderPlusBankHelpers.RemoveMoneyFromPlayer(transaction.Customer,transaction.Amount);
        }
      }
      break;

      case TP_BANK_TRANSACTION_WITHDRAW:
      {
        if(account)
        {
          if(account.MoneyAmount < transaction.Amount)return;
          newAmount = account.MoneyAmount - transaction.Amount;
          account.MoneyAmount = newAmount;
          account.UpdateAccount(transaction.Customer);
          TraderPlusBankHelpers.AddMoneyToPlayer(transaction.Customer,transaction.Amount);
        }
      }
      break;

      case TP_BANK_TRANSACTION_TRANSFERT:
      {
        if(account)
        {
          if(!FileExist(string.Format(TPB_DB_FILENAME, transaction.DestinationIDAccount)))
          {
            NotificationSystem.SendNotificationToPlayerIdentityExtended(transaction.Customer.GetIdentity(), 2, "Bank Transfer", GetTraderPlusBankConfigServer().TheAmountErrorTransferAccount, "TraderPlusBanking/datasets/Bank.paa");
            return;
          }
          if(account.MoneyAmount < transaction.Amount)return;
          newAmount = account.MoneyAmount - transaction.Amount - transaction.Amount*GetTraderPlusBankConfigServer().TransactionFees;
          account.MoneyAmount = newAmount;
          account.UpdateAccount(transaction.Customer);
          TraderPlusBankingData playerAccount = TraderPlusBankingData.Load(transaction.DestinationIDAccount, GetTraderPlusBankConfigServer().DefaultMaxCurrency,GetTraderPlusBankConfigServer().DefaultStartCurrency);
          if(playerAccount)
          {
            playerAccount.Version= "-1";
            if(playerAccount.MoneyAmount + transaction.Amount > playerAccount.MaxAmount)
              playerAccount.MoneyAmount = playerAccount.MaxAmount;
            else
              playerAccount.MoneyAmount += transaction.Amount;

            playerAccount.Save(transaction.DestinationIDAccount);
            NotificationSystem.SendNotificationToPlayerIdentityExtended(transaction.Customer.GetIdentity(), 2, "Bank Transfer", GetTraderPlusBankConfigServer().TheAmountHasBeenTransferedToTheAccount, "TraderPlusBanking/datasets/Bank.paa");
          }
        }
      }
      break;
    }
  }

  //-------------------------------------------RPC PART--------------------------------------------------------//
  void TraderPlusBankingRequest(CallType type, ParamsReadContext ctx, PlayerIdentity sender, Object target)
  {
     if (!GetGame().IsServer())
     return;

     PlayerBase player = TraderPlusGetPlayerByIdentity(sender);
     TraderPlusBankingData account = player.GetBankAccount();
     if(account)
     {
       #ifdef TRADERPLUSDEBUG
       GetTraderPlusLogger().LogInfo("TraderPlusBankingRequest player.m_BankAccount:"+player.m_BankAccount.MoneyAmount+ " - " +player.m_BankAccount.MaxAmount + " - " + player.m_BankAccount.Name + " - " + player.m_BankAccount.Licences); //sender.GetName() = player.GetIdentity().GetName()
       #endif
       if(account.MoneyAmount > account.MaxAmount)
         account.MoneyAmount = account.MaxAmount;

       account.UpdateAccount(player);
     }

     #ifdef TRADERPLUSDEBUG
     GetTraderPlusLogger().LogInfo("TraderPlusBankingRequest by player:"+sender.GetName()); //sender.GetName() = player.GetIdentity().GetName()
     #endif
  }

  void TraderPlusBankingWithdraw(CallType type, ParamsReadContext ctx, PlayerIdentity sender, Object target)
  {
   if (!GetGame().IsServer())
     return;

     Param1<int> data;
     if (!ctx.Read(data))
       return;

     PlayerBase player = TraderPlusGetPlayerByIdentity(sender);

     TraderPlusBankingTransaction m_transaction = new TraderPlusBankingTransaction;
     m_transaction.Customer = player;
     m_transaction.Amount = data.param1;
     m_transaction.TransactionMode = TP_BANK_TRANSACTION_WITHDRAW;

     TransactionsQueue.EnQueue(m_transaction);

     if(GetTraderPlusBankConfigServer().BankingLogs)
      GetTraderPlusLogger().LogInfo("TraderPlusBankingWithdraw:"+sender.GetName() +" amount:" +data.param1.ToString()); //sender.GetName() = player.GetIdentity().GetName()
  }

  void TraderPlusBankingDeposit(CallType type, ParamsReadContext ctx, PlayerIdentity sender, Object target)
  {
   if (!GetGame().IsServer())
     return;

     Param1<int> data;
     if (!ctx.Read(data))
       return;

     PlayerBase player = TraderPlusGetPlayerByIdentity(sender);

     TraderPlusBankingTransaction m_transaction = new TraderPlusBankingTransaction;
     m_transaction.Customer = player;
     m_transaction.Amount = data.param1;
     m_transaction.TransactionMode = TP_BANK_TRANSACTION_DEPOSIT;

     TransactionsQueue.EnQueue(m_transaction);

     if(GetTraderPlusBankConfigServer().BankingLogs)
      GetTraderPlusLogger().LogInfo("TraderPlusBankingDeposit:"+sender.GetName() +" amount:" +data.param1.ToString()); //sender.GetName() = player.GetIdentity().GetName()
  }

  void TraderPlusTransfertRequest(CallType type, ParamsReadContext ctx, PlayerIdentity sender, Object target)
  {
   if (!GetGame().IsServer())
     return;

     Param2<string, int> data;
     if (!ctx.Read(data))
       return;

     PlayerBase player = TraderPlusGetPlayerByIdentity(sender);

     TraderPlusBankingTransaction m_transaction = new TraderPlusBankingTransaction;
     m_transaction.Customer = player;
     m_transaction.Amount = data.param2;
     m_transaction.DestinationIDAccount = data.param1;
     m_transaction.TransactionMode = TP_BANK_TRANSACTION_TRANSFERT;

     TransactionsQueue.EnQueue(m_transaction);
  }
  //---------------------------------------------------------------------------------------------------------//
};
