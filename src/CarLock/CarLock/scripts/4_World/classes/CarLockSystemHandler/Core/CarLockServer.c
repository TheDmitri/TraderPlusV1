#ifndef CARLOCKDISABLE
class CarLockServer
{
  void CarLockServer()
  {
      GetTraderPlusLogger().LogInfo("CarLockServer - Started");
  }

  //-------------------------------------------RPC PART--------------------------------------------------------//
  void CarLockRequest(CallType type, ParamsReadContext ctx, PlayerIdentity sender, Object target)
  {
   if (!GetGame().IsServer())
     return;

     Param2<CarScript, bool> data;
     if (!ctx.Read(data))
       return;

     PlayerBase player = CLGetPlayerByIdentity(sender);
     //GetTraderPlusLogger().LogInfo("CarLockRequest");

     CarScript car = data.param1;
     if(!car)
     {
       //GetTraderPlusLogger().LogInfo("CarLockRequest: no car");
       return;
     }

     car.SetCarLock(data.param2);
     if(data.param2)
     {
       car.SetSoundToPlay(1);
     }else car.SetSoundToPlay(2);

     //GetTraderPlusLogger().LogInfo("CarLockRequest by player:"+sender.GetName()); //sender.GetName() = player.GetIdentity().GetName()
  }

  void CarLockPasswordRequest(CallType type, ParamsReadContext ctx, PlayerIdentity sender, Object target)
  {
   if (!GetGame().IsServer())
     return;

     Param2<CarScript, int> data;
     if (!ctx.Read(data))
       return;

     PlayerBase player = CLGetPlayerByIdentity(sender);
     //GetTraderPlusLogger().LogInfo("CarLockOwnerPasswordRequest");

     CarScript car = data.param1;
     if(!car)
     {
       //GetTraderPlusLogger().LogInfo("CarLockOwnerPasswordRequest: no car");
       return;
     }

     if(data.param2 == -1)
     {
       car.SetCarLockOwner(player.CLSteamlowID);
       car.SetCarLock(true);
     }
     else
     {
       car.SetCarLockOwner(player.CLSteamlowID);
       car.SetCarLock(true);
       car.SetCarLockPassword(data.param2);
     }
     //GetTraderPlusLogger().LogInfo("CarLockOwnerPasswordRequest by player:"+sender.GetName()); //sender.GetName() = player.GetIdentity().GetName()
  }
  //---------------------------------------------------------------------------------------------------------//
};
#endif
