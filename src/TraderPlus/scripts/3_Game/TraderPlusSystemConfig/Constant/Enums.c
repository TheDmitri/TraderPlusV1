enum TraderPlusResponse
{
  SELL_SUCCESS=1,
  SELL_FAILED,
  BUY_SUCCESS,
  BUY_FAILED,
  NO_TRADE,
  CLOSE_REQUEST
};

enum TraderPlusItemType
{
  ITEM=0,
  VEHICLE,
};

enum TraderPlusInventoryCategory
{
  SELLABLE=0,
  ALL=1,
  LICENCES=2,
  VEHICLES=3,
  INSURANCES=4
};

enum TraderPlusItemState
{
  PRISTINE = 0,
  WORN,
  DAMAGED,
  BADLY_DAMAGED,
  RUINED
}

enum TraderPlusSound
{
  QUICKEVENT=1,
  SELL,
  BUY
};

enum TraderPlusLogLevel
{
    Debug,
    Info,
    Warning,
    Error
}
