class TraderPlusArticle
{
  int        TraderID;
  string     Category;
  string     ClassName;
  int        Count;
  int        Quantity;
  int        MaxStock;
  int        Health;
  bool       HasAttachments;

  void AddPlayerItems(string categoryname, string classname, int count, int quantity, int health, bool hasattach)
  {
    Category = categoryname;
    ClassName = classname;
    Count = count;
    Quantity = quantity;
    Health = health;
    HasAttachments = hasattach;
  }

  void AddStockItems(string categoryname, string classname, int quantity, int health, int maxstock)
  {
    Category = categoryname;
    ClassName = classname;
    Quantity = quantity;
    Health = health;
    MaxStock = maxstock;
  }
}
