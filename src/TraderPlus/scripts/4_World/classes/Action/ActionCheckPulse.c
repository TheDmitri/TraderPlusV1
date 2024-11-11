modded class ActionCheckPulse: ActionInteractBase
{
  //Disable check pulse action on trader NPC
	override bool ActionCondition( PlayerBase player, ActionTarget target, ItemBase item )
	{
		PlayerBase ntarget = PlayerBase.Cast(  target.GetObject() );
		if( ntarget && ntarget.IsAlive() && ntarget.TraderID != -1)
		{
			return false;
		}
		return super.ActionCondition(player, target, item);
	}
}
