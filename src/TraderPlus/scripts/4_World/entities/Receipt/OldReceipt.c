class ReceiptBase extends ItemBase
{
	int CarID=0;
	int CarAttachments=0;

	void ReceiptBase()
	{
		RegisterNetSyncVariableInt("CarID");
		RegisterNetSyncVariableInt("CarAttachments");
	}

	void SetCarID(int id)
	{
		CarID = id;
		SetSynchDirty();
	}

	override void SetActions()
	{
		super.SetActions();
		AddAction(ActionConvertToNewReceipt);
	}

	override void OnStoreSave( ParamsWriteContext ctx )
	 {
			super.OnStoreSave(ctx);
			Param2<int,int> data = new Param2<int,int>(CarID,CarAttachments);
			ctx.Write(data);
	 }

	override bool OnStoreLoad( ParamsReadContext ctx, int version )
		{
			if ( !super.OnStoreLoad( ctx, version ) )
				return false;

					Param2<int,int> data = new Param2<int,int>(-1,-1);
					if (ctx.Read(data))
					{
							CarID = data.param1;
							CarAttachments = data.param2;
					}
					LockReceiptInventory();
					return true;
		}

		void LockReceiptInventory()
		{
			GetInventory().LockInventory(HIDE_INV_FROM_SCRIPT);
			SetSynchDirty();
		}
};

class Receipt extends ReceiptBase
{

}
