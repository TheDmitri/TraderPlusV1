#ifndef CARLOCKDISABLE
modded class MissionGameplay
{
    override void TryToLockVehicleNearby(PlayerBase player)
    {
        if (!player) return;
        array<Object> objects=new array<Object>;
        GetGame().GetObjectsAtPosition3D(player.GetPosition(),5,objects,NULL);
        MotorbikeScript selected;
        float nearest=4;
        foreach (Object object:objects)
        {
            // Preserve the original car shortcut in mixed parking areas.
            Transport other=Transport.Cast(object);
            if (other && !MotorbikeScript.Cast(other)) { super.TryToLockVehicleNearby(player); return; }
            MotorbikeScript bike=MotorbikeScript.Cast(object);
            if (!TPCM.Supports(bike) || !bike.m_TPCMInstalled || bike.m_TPCMFault) continue;
            float distance=vector.Distance(player.GetPosition(),bike.GetPosition());
            if (distance<nearest) { selected=bike; nearest=distance; }
        }
        if (!selected) { super.TryToLockVehicleNearby(player); return; }
        // The server decides ownership and the resulting lock state.
        if (GetGame().GetUIManager().GetMenu()) return;
        GetRPCManager().SendRPC("TPMotorbikeCarLock","TPCMHotkeyRequest",new Param1<MotorbikeScript>(selected),true,NULL);
    }
};
#endif
