#ifndef CARLOCKDISABLE
modded class Pliers extends ItemBase
{
  override void SetActions()
	{
		super.SetActions();
		AddAction(ActionDetachCarLock);
	}
};

class CarLockPick extends ItemBase
{
  override void SetActions()
	{
		super.SetActions();
		AddAction(ActionDetachCarLockPick);
	}
};

class CarLock extends ItemBase
{
  override void SetActions()
	{
		super.SetActions();
		AddAction(ActionAttachCarLock);
	}
};
#endif
