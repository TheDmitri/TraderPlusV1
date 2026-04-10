class TraderPlusMoneyHelper
{
    static int GetPlayerMoneyFromInventory()
    {
        PlayerBase player = PlayerBase.Cast(GetGame().GetPlayer());
        if (!player)
        {
            return 0;
        }

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
                    GetTraderPlusConfigClient().MoneyName.Get(j).Split(",", traderCurrencyArray);
                    for (int k = 0; k < traderCurrencyArray.Count(); k++)
                    {
                        if (traderCurrencyArray.Get(k) == className)
                        {
                            if (GetTraderPlusBankingConfigClient().CurrenciesAccepted.Count() == 0)
                            {
                                qty = TraderPlusHelper.GetItemAmount(item);
                                value = GetTraderPlusConfigClient().MoneyValue.Get(j);
                                amount += (value * qty);
                                continue;
                            }

                            for (int l = 0; l < GetTraderPlusBankingConfigClient().CurrenciesAccepted.Count(); l++)
                            {
                                if (GetTraderPlusBankingConfigClient().CurrenciesAccepted[l] == traderCurrencyArray[k])
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
}
