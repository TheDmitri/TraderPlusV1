/*Server-side check of every trade sent by a client.
The client sends a whole TraderPlusProduct (price, quantity, multiplier...). Nothing in it can be trusted:
a modified client could buy for 0, sell for any price, use a negative multiplier or overflow the price.
Prepare() rebuilds price, quantity and stock mode from the server config and stock files, using the same
formula as TraderPlusClient.CalculatePriceForThatItem, and refuses the trade when it cannot be priced.*/
class TraderPlusTradeGuard
{
  static const int MAX_MULTIPLIER = 5000;
  static const float MAX_PRICE = 2000000000;

  static ref map<string, int> m_MaxQuantityCache;

  static bool Prepare(TraderPlusProduct product)
  {
    if(!product || !product.Customer || !product.Customer.GetIdentity())
      return false;

    if(product.TradMode == TRADEMODE_SELLINSURANCE)
      return PrepareInsuranceClaim(product);

    if(product.Multiplier < 1 || product.Multiplier > MAX_MULTIPLIER)
      return Refuse(product, "multiplier " + product.Multiplier.ToString());

    TStringArray fields = FindPriceLine(product.TraderID, product.Category, product.ClassName);
    if(!fields)
      return Refuse(product, "not sold by this trader");

    bool selling = product.TradMode == TRADEMODE_SELL || product.TradMode == TRADEMODE_SELLVEHICLE;
    float stateCoefficient = StateCoefficient(product.Health);
    if(stateCoefficient <= 0)
      return Refuse(product, "item condition not accepted");

    float coefficient = 1;
    if(fields[1].ToFloat() > 0)
      coefficient = fields[1].ToFloat();
    int maxStock = fields[2].ToInt();
    int buyPrice = fields[4].ToInt();
    float sellValue = fields[5].ToFloat();
    if(sellValue < 1.0 && sellValue != -1.0)
      sellValue = fields[4].ToFloat() * sellValue;
    int sellPrice = sellValue;

    int basePrice = buyPrice;
    if(selling)
      basePrice = sellPrice;
    if(basePrice == -1)
      return Refuse(product, "price -1 in config");

    // A garage vehicle is one vehicle: the client's multiplier would otherwise be added to the stock.
    if(product.TradMode == TRADEMODE_SELLVEHICLE)
      product.Multiplier = 1;
    int multiplier = product.Multiplier;

    float price = 0;
    if(maxStock == -1)
    {
      price = 1.0 * basePrice * multiplier * stateCoefficient;
    }
    else
    {
      int stock = StockCount(product, selling);
      for(int i = 0; i < multiplier; i++)
      {
        int n = stock;
        if(selling && n == 0)
          n = 1;
        price += Math.Pow(coefficient, n - 1) * basePrice * stateCoefficient;
        stock++;
      }
    }
    if(price > MAX_PRICE)
      return Refuse(product, "price overflow");
    int serverPrice = price;
    if(serverPrice == 0)
      serverPrice = 1;

    if(!selling && product.CollateralMoney != 0)
    {
      // Motorbikes have no insurance tracking: the collateral would be paid for nothing.
      if(GetGame().IsKindOf(product.ClassName, "MotorbikeScript"))
        return Refuse(product, "insurance is not available for motorbikes");
      TraderPlusVehicleInsurance insurance = FindInsurance(product.ClassName);
      if(!insurance)
        return Refuse(product, "no insurance for this vehicle");
      float collateral = serverPrice * insurance.CollateralMoneyCoefficient;
      float insurancePrice = serverPrice * insurance.InsurancePriceCoefficient;
      if(serverPrice + insurancePrice > MAX_PRICE)
        return Refuse(product, "price overflow");
      product.CollateralMoney = collateral;
      serverPrice += insurancePrice;
    }

    int tradeQty = TradeQuantity(product.ClassName, fields[3].ToFloat()) * product.Multiplier;
    if(product.TradMode == TRADEMODE_BUY)
    {
      product.Quantity = tradeQty;
    }
    else if(product.TradMode == TRADEMODE_SELL)
    {
      product.Quantity = tradeQty;
      if(product.Multiplier > 1 && tradeQty == 0)
        product.Quantity = product.Multiplier;
    }

    if(Math.AbsInt(serverPrice - product.Price) > 1)
      GetTraderPlusLogger().LogInfo("TradeGuard: price corrected for " + product.Customer.GetIdentity().GetName() + " " + product.ClassName + " client:" + product.Price.ToString() + " server:" + serverPrice.ToString());

    product.Price = serverPrice;
    product.MaxStock = maxStock;
    return true;
  }

  //Insurance money is paid only for a vehicle that no longer exists, and only the amount the server recorded.
  static bool PrepareInsuranceClaim(TraderPlusProduct product)
  {
    TraderPlusBankingData account = product.Customer.GetBankAccount();
    string insuranceData;
    if(!account || !account.Insurances || !account.Insurances.Find(product.CollateralMoney, insuranceData))
      return Refuse(product, "unknown insurance");

    if(CarScript.GetMapAll()[product.CollateralMoney])
      return Refuse(product, "insured vehicle still exists");

    if(GetTraderPlusConfigClient().AuthorizedIDInsurance.Find(product.TraderID) == -1)
      return Refuse(product, "trader does not pay insurance");

    product.Price = TraderPlusHelper.GetCollateralPriceInsuranceData(insuranceData);
    product.Multiplier = 1;
    product.Quantity = 0;
    return product.Price > 0;
  }

  static TStringArray FindPriceLine(int traderId, string category, string classname)
  {
    TStringArray traderCategories = TraderPlusHelper.GetCategoriesFromID(traderId);
    if(!traderCategories || traderCategories.Find(category) == -1)
      return null;

    string wanted = classname;
    wanted.ToLower();
    foreach(TraderPlusCategory priceCategory: GetTraderPlusPriceConfig().TraderCategories)
    {
      if(priceCategory.CategoryName != category)
        continue;
      foreach(string line: priceCategory.Products)
      {
        TStringArray fields = new TStringArray;
        line.Split(",", fields);
        if(fields.Count() < 6)
          continue;
        string name = fields[0];
        name.TrimInPlace();
        name.ToLower();
        if(name == wanted)
          return fields;
      }
    }
    return null;
  }

  //Same stock the client priced with: the trader's count of that item at that condition.
  static int StockCount(TraderPlusProduct product, bool selling)
  {
    int health = product.Health;
    if(selling && GetTraderPlusConfigServer().StoreOnlyToPristineState)
      health = 0;

    string wanted = product.ClassName;
    wanted.ToLower();
    TraderPlusCategory stockCategory = TraderPlusHelper.GetStockCategory(product.TraderID, product.Category, false);
    if(!stockCategory)
      return 0;
    foreach(string line: stockCategory.Products)
    {
      TStringArray tokens = new TStringArray;
      line.Split(" ", tokens);
      if(tokens.Count() < 3)
        continue;
      string name = tokens[0];
      name.ToLower();
      if(name == wanted && tokens[2].ToInt() == health)
        return tokens[1].ToInt();
    }
    return 0;
  }

  static float StateCoefficient(int state)
  {
    TraderPlusStates states = GetTraderPlusConfigServer().AcceptedStates;
    if(state == TraderPlusItemState.PRISTINE)
      return 1.0;
    if(state == TraderPlusItemState.WORN && states.AcceptWorn)
      return states.CoefficientWorn;
    if(state == TraderPlusItemState.DAMAGED && states.AcceptDamaged)
      return states.CoefficientDamaged;
    if(state == TraderPlusItemState.BADLY_DAMAGED && states.AcceptBadlyDamaged)
      return states.CoefficientBadlyDamaged;
    return 0;
  }

  //Mirrors TraderPlusClient.CalculateTradeQty. The max quantity spawns a temporary entity, so it is cached.
  static int TradeQuantity(string classname, float tradeqty)
  {
    if(!m_MaxQuantityCache)
      m_MaxQuantityCache = new map<string, int>;
    int maxqty;
    if(!m_MaxQuantityCache.Find(classname, maxqty))
    {
      maxqty = TraderPlusHelper.GetMaxItemQuantityServer(classname);
      m_MaxQuantityCache.Set(classname, maxqty);
    }

    if(tradeqty > maxqty)
      return maxqty;
    if(classname.Contains("Mag_") && tradeqty != 0.0)
      return 0;
    if(tradeqty < 1.0 && tradeqty != 0.0 && tradeqty != -1.0)
      return tradeqty * (1.0 * maxqty);
    if(tradeqty == -1.0)
      return maxqty;
    return tradeqty;
  }

  static TraderPlusVehicleInsurance FindInsurance(string classname)
  {
    TraderPlusInsuranceSettings settings = GetTraderPlusCore().m_TraderPlusInsuranceSettings;
    if(!settings || !settings.Insurances)
      return null;
    foreach(TraderPlusVehicleInsurance insurance: settings.Insurances)
    {
      if(insurance.VehicleName == classname)
        return insurance;
    }
    return null;
  }

  static bool Refuse(TraderPlusProduct product, string reason)
  {
    string who = "?";
    if(product && product.Customer && product.Customer.GetIdentity())
      who = product.Customer.GetIdentity().GetName() + " (" + product.Customer.GetIdentity().GetPlainId() + ")";
    string what = "";
    if(product)
      what = product.ClassName + " mode:" + product.TradMode.ToString() + " x" + product.Multiplier.ToString() + " price:" + product.Price.ToString();
    GetTraderPlusLogger().LogInfo("TradeGuard: refused " + who + " " + what + " - " + reason);
    return false;
  }
};
