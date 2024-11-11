class TraderPlusBankHelpers
{
	static void SendBankAccountInfo(PlayerBase player)
	{
		#ifdef TRADERPLUSDEBUG
		GetTraderPlusLogger().LogInfo("SendBankAccountInfo");
		#endif
		if(player.m_BankAccount)
			player.m_BankAccount.UpdateAccount(player);
	}

	static void SetAmountBankAccount(PlayerBase player, int amount)
	{
		#ifdef TRADERPLUSDEBUG
		GetTraderPlusLogger().LogInfo("SetAmountBankAccount");
		#endif
		TraderPlusBankingData account = player.GetBankAccount();
		if(account)
		{
			account.MoneyAmount = amount;
			account.UpdateAccount(player);
		}
	}

	static bool CheckifPlayerHasEnoughMoney(PlayerBase player, int amount)
  {
    int playerMoneyAmount = GetPlayerMoney(player);
    #ifdef TRADERPLUSDEBUG
    GetTraderPlusLogger().LogInfo("CheckifPlayerHasEnoughMoney: amount"+playerMoneyAmount.ToString());
    #endif
    if(playerMoneyAmount<amount)
    {
      return false;
    }
    return true;
  }

	static int GetPlayerMoney(PlayerBase player)
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
                      if(GetTraderPlusBankConfigServer().CurrenciesAccepted.Count() == 0)
                      {
                        quantity = TraderPlusHelper.GetItemAmount(temp);
                        value = GetTraderPlusConfigServer().Currencies.Get(j).Value;
                        amount += (value * quantity);
                        continue;
                      }

                      //Multi currency part
                      for(int l=0;l<GetTraderPlusBankConfigServer().CurrenciesAccepted.Count();l++)
                      {
                        if(GetTraderPlusBankConfigServer().CurrenciesAccepted[l]==classname)
                        {
                          quantity = TraderPlusHelper.GetItemAmount(temp);
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
  }

	/*static bool AddMoneyToPlayer(PlayerBase player, int moneyamount)
  {
  	int moneyamountGiven = 0;
  	int moneyamountRemaining = moneyamount;

  	for (int i = 0; i < GetTraderPlusConfigServer().Currencies.Count(); i++)
  	{
  			int give = 0;
        TraderPlusCurrency  traderCurrency = GetTraderPlusConfigServer().Currencies.Get(i);
  			while (moneyamountRemaining >= traderCurrency.Value)
  			{
  				give++;
  				moneyamountRemaining -= traderCurrency.Value;
  				moneyamountGiven += traderCurrency.Value;
  			}
  			if (give > 0)
  			{
          TStringArray strs = new TStringArray;
          traderCurrency.ClassName.Split( ",", strs );
          if(GetTraderPlusBankConfigServer().CurrenciesAccepted.Count() == 0)
          {
            TraderPlusHelper.CreateInInventory(player, strs[0], give);
            give = 0;
            break;
          }

          //Multi currency part
          for(int k=0;k<strs.Count();k++)
          {
              for(int j=0;j<GetTraderPlusBankConfigServer().CurrenciesAccepted.Count();j++)
              {
                if(GetTraderPlusBankConfigServer().CurrenciesAccepted[j]==strs[k])
                {
                   TraderPlusHelper.CreateInInventory(player, strs[k], give);
                   give = 0;
									 break;
                }
              }
          }
          if(give > 0)
          {
            moneyamountRemaining += give*traderCurrency.Value;
          }
  			}
  	}
  	return true;
  }*/

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

      if(GetTraderPlusBankConfigServer().CurrenciesAccepted.Count() == 0)
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
            TraderPlusHelper.CreateInInventory(player, strs[0], AmountToGive);
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
        for(int j=0;j<GetTraderPlusBankConfigServer().CurrenciesAccepted.Count();j++)
        {
          int pos = strs.Find(GetTraderPlusBankConfigServer().CurrenciesAccepted[j]);
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
            TraderPlusHelper.CreateInInventory(player, strs[pos], AmountToGive);
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

  static bool RemoveMoneyFromPlayer(PlayerBase player, int moneyamount)
  {
  	int moneyamountTaken = 0;
    int currentCurrencyQuantity = 0;
    int take = 0;
    bool state = false;
  	int moneyamountRemaining = moneyamount;
    int count = -1 + GetTraderPlusConfigServer().Currencies.Count();
  	for (int i = count; i >= 0 ; i--)
  	{
        string currenciesName = GetTraderPlusConfigServer().Currencies.Get(i).ClassName;;
        TStringArray strs = new TStringArray;
        currenciesName.Split(",", strs );
        for(int j=0;j<strs.Count();j++)
        {
          if(GetTraderPlusBankConfigServer().CurrenciesAccepted.Count() == 0)
          {
            currentCurrencyQuantity = TraderPlusHelper.GetQuantityOfSpecificItem(player, strs[j], -1);
            take = 0;
            while ((moneyamountRemaining > 0) && (take < currentCurrencyQuantity))
            {
              take++;
              moneyamountRemaining -= GetTraderPlusConfigServer().Currencies.Get(i).Value;
              moneyamountTaken += GetTraderPlusConfigServer().Currencies.Get(i).Value;
            }
            if (take > 0)
            {
              state = TraderPlusHelper.RemoveOurProduct(player, strs[j], take);
              if (!state)
                return false;
            }
          }
					else
					{
						//Multi currency atm
	          for(int k=0;k<GetTraderPlusBankConfigServer().CurrenciesAccepted.Count();k++)
	          {
	            if(GetTraderPlusBankConfigServer().CurrenciesAccepted[k]==strs[j])
	            {
	              currentCurrencyQuantity = TraderPlusHelper.GetQuantityOfSpecificItem(player, strs[j], -1);
	          		take = 0;
	          		while ((moneyamountRemaining > 0) && (take < currentCurrencyQuantity))
	          		{
	          			take++;
	          			moneyamountRemaining -= GetTraderPlusConfigServer().Currencies.Get(i).Value;
	          			moneyamountTaken += GetTraderPlusConfigServer().Currencies.Get(i).Value;
	          		}
	          		if (take > 0)
	          		{
	          			state = TraderPlusHelper.RemoveOurProduct(player, strs[j], take);
	          			if (!state)
	          				return false;
	          		}
	            }
						}
					}
        }
  	}
  	if (moneyamountTaken > moneyamount)
  	{
  		int difference = moneyamountTaken - moneyamount;
  		GiveTheDifferenceBackToPlayer(player,difference);
  	}
  	return true;
  }

	static void GiveTheDifferenceBackToPlayer(PlayerBase player, int moneyamount)
  {
    GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).Call(AddMoneyToPlayer,player, moneyamount);
  }
};
