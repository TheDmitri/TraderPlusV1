class TraderPlusItem
{
  string ClassName;
  float  Coeff;
  int    Quantity;
  int    MaxStock;
  int    Health;
  int    BuyPrice;
  int    SellPrice;

  void TraderPlusItem(string name,float coeff, int maxstock,int qty, int sellprice,int buyprice, int state=0)
  {
    ClassName = name;
    Coeff     = coeff;
    MaxStock  = maxstock;
    BuyPrice  = buyprice;
    SellPrice = sellprice;
    Quantity  = qty;
    Health    = state;
  }
};

class MapTraderPlusItems
{
  string DummyString="DONT_ASK_WHY_IM_HERE";
  ref map<string, ref array<ref TraderPlusItem>>traderPlusItems;

  void MapTraderPlusItems()
  {
    traderPlusItems = new map<string, ref array<ref TraderPlusItem>>;
  }
}
