#ifndef CARLOCKDISABLE
modded class  ActionDetachCarLockPick: ActionContinuousBase
{
	override bool ActionCondition( PlayerBase player, ActionTarget target, ItemBase item )
	{
		if ( GetGame().IsServer() )
				return true;

		CarScript ntarget = CarScript.Cast( target.GetObject() );

		if (ntarget && ntarget.m_CarLockOwner != -1)
		{
			bool IsEmpty = true;
			Transport transport = Transport.Cast(target.GetObject());
			if (!transport)return false;

			int crewSize = transport.CrewSize();
			for (int j = 0; j < crewSize; j++)
			{
				if (transport.CrewMember(j))
					IsEmpty = false;
			}

			if (IsEmpty)return true;
		}
		return false;
	}
};
#endif
