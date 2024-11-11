modded class DayZPlayerImplement
{
    override void EEKilled(Object killer)
    {
      super.EEKilled(killer);

      HideOutObject stash;
  		stash = HideOutObject.Cast(FindAttachmentBySlotName("Armband"));
  		if(stash)
  		{
  			ServerDropEntity(stash);
  			stash.SetPosition(vector.Zero);
  		}
    }

    // ------------------------------------------------------------
}
