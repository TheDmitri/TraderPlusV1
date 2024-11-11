class TraderPlus_Wallet extends Inventory_Base
{
	ref TStringArray TraderPlusMoneys = {
		"TraderPlusMoney_Base",
		"TraderPlusCoin_Base",
	};

	//Premade function so you can add your bills classname to the array
	void AddCompatibilityMoneyToWallet()
	{
		//Define your bills in this array
		TStringArray yourMoneyClasses = {
			"CLASSNAME_1",
			"CLASSNAME_2",
			"CLASSNAME_3"
		};

		//they will be added then in the main array => TraderPlusMoneys
		for(int i = 0; i<yourMoneyClasses.Count();i++)
		{
			TraderPlusMoneys.Insert(yourMoneyClasses[i]);
		}
	}

	override bool CanReceiveItemIntoCargo (EntityAI item)
    {
			//AddCompatibilityMoneyToWallet();  uncomment if you want to add new elements to the array for making your bills compatible
      for(int i=0;i<TraderPlusMoneys.Count();i++)
      {
        if(item.IsKindOf(TraderPlusMoneys[i])) return true;
      }
      return false;
    }
}
