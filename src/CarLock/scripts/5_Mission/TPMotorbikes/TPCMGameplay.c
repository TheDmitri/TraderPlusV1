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
        float nearestOther=5;
        foreach (Object object:objects)
        {
            Transport vehicle=Transport.Cast(object);
            if (!vehicle) continue;
            float distance=vector.Distance(player.GetPosition(),vehicle.GetPosition());
            MotorbikeScript bike=MotorbikeScript.Cast(vehicle);
            if (!TPCM.Supports(bike) || !bike.m_TPCMInstalled || bike.m_TPCMFault)
            {
                if (distance<nearestOther) nearestOther=distance;
                continue;
            }
            if (distance<nearest) { selected=bike; nearest=distance; }
        }
        // The closest vehicle wins; a nearer car keeps the original car shortcut.
        if (!selected || nearestOther<nearest) { super.TryToLockVehicleNearby(player); return; }
        // The server decides ownership and the resulting lock state.
        if (GetGame().GetUIManager().GetMenu()) return;
        GetRPCManager().SendRPC("TPMotorbikeCarLock","TPCMHotkeyRequest",new Param1<MotorbikeScript>(selected),true,NULL);
    }
};
#endif
