#ifndef CARLOCKDISABLE
modded class CarScript
{
    bool m_CarLock_IsLocked = false;
    int  m_CarLockOwner     = -1;
    int  m_CarLockPassword  = -1;

    int  m_SoundToPlay      = 0;

    protected EffectSound 		m_AlarmSound;
    protected EffectSound 		m_BipSound;

    void CarScript()
	   {
        RegisterNetSyncVariableInt("m_SoundToPlay");
		    RegisterNetSyncVariableBool("m_CarLock_IsLocked");
        RegisterNetSyncVariableInt("m_CarLockOwner");
        RegisterNetSyncVariableInt("m_CarLockPassword");
        if(m_AlarmSound)m_AlarmSound.SoundStop();
        if (GetGame().IsServer())
            GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(this.CarLockLogInit, 10, false);
	   }

     void CarLockLogInit()
     {
        if(m_CarLockOwner != -1 &&  m_CarLockPassword!= -1)
        {
          GetCarLockLogger().Log(GetDisplayName() + " (ownerId:" + m_CarLockOwner + " - pos " + GetPosition() +" - islocked:"+m_CarLock_IsLocked+" - password:"+m_CarLockPassword+ ") initialized");
        }
    }

     void CarLockLogClaimed()
     {
        GetCarLockLogger().Log(GetDisplayName() + " (ownerId:" + m_CarLockOwner + " - pos " + GetPosition() +" - islocked:"+m_CarLock_IsLocked+" - password:"+m_CarLockPassword+ ") new car claimed");
     }

    override void EEDelete(EntityAI parent)
	   {
        super.EEDelete(parent);
		    if(GetGame().IsServer())
        {
          if(m_CarLockOwner != -1 &&  m_CarLockPassword!= -1)
          {
            GetCarLockLogger().Log(GetDisplayName() + " (ownerId:" + m_CarLockOwner + " - pos " + GetPosition() +" - islocked:"+m_CarLock_IsLocked+" - password:"+m_CarLockPassword+ ") has been deleted");
          }
        }
    }


    void ~CarScript()
	   {
        if(m_AlarmSound)m_AlarmSound.Stop();
	   }

   override void OnVariablesSynchronized()
	   {
		     super.OnVariablesSynchronized();

		     switch (m_SoundToPlay)
         {
           case 0:return;
           break;

           case 1:
           {
             PlayCarLockSound();
             m_SoundToPlay = 0;
           }
           break;

           case 2:
           {
             PlayCarUnlockSound();
             m_SoundToPlay = 0;
           }
           break;

           case 3:
           {
             PlayCarLockAlarm();
             m_SoundToPlay = 0;
           }
           break;

           default:return;
           break;
         }
	   }

     void SetSoundToPlay(int id = 0)
     {
       m_SoundToPlay = id;
       SetSynchDirty();

       thread ResetSoundToPlay();
     }

     void ResetSoundToPlay()
     {
       Sleep(1000);
       m_SoundToPlay = 0;
       SetSynchDirty();
     }

     void SetCarLockPassword(int pw)
     {
       m_CarLockPassword = pw;
       CarLockLogClaimed();
       SetSynchDirty();
       //GetTraderPlusLogger().LogInfo("m_CarLockPassword:"+m_CarLockPassword.ToString());
     }

     void SetCarLockOwner(int low_uid)
     {
       m_CarLockOwner = low_uid;
       SetSynchDirty();
       //GetTraderPlusLogger().LogInfo("m_CarLockOwner:"+m_CarLockOwner.ToString());
     }

     void SetCarLock(bool rep)
     {
       m_CarLock_IsLocked = rep;
       SetSynchDirty();
       SynchronizeValues();
       //GetTraderPlusLogger().LogInfo("m_CarLock_IsLocked:"+m_CarLock_IsLocked.ToString());
     }

     void StopAlarm()
     {
       if(m_AlarmSound)m_AlarmSound.SoundStop();
     }

     void PlayCarLockAlarm()
     {
         if(m_AlarmSound && m_AlarmSound.IsSoundPlaying())m_AlarmSound.SoundStop();
         PlaySoundSet( m_AlarmSound,"cl_CarLock_CarAlarm_SoundSet" , 0, 0);
     }

     void PlayCarLockSound()
     {
       if(m_AlarmSound && m_AlarmSound.IsSoundPlaying())m_AlarmSound.SoundStop();
       PlaySoundSet( m_BipSound,"cl_CarLock_CarLock_SoundSet" , 0, 0);
     }

     void PlayCarUnlockSound()
     {
       if(m_AlarmSound && m_AlarmSound.IsSoundPlaying())m_AlarmSound.SoundStop();
       PlaySoundSet( m_BipSound,"cl_CarLock_CarUnlock_SoundSet" , 0, 0);
     }

     override bool CanDisplayAttachmentSlot( string slot_name )
     {
    		if(!super.CanDisplayAttachmentSlot(slot_name))
    			return false;

    		return !m_CarLock_IsLocked;
     }

     override bool CanDisplayAttachmentCategory( string category_name )
     {
    		if(!super.CanDisplayAttachmentCategory(category_name))
    			return false;

    		return !m_CarLock_IsLocked;
     }

     void SynchronizeValues()
  	  {
  		     if (GetGame().IsServer())
           {
              if(GetInventory())
              {
                   if (m_CarLock_IsLocked)
                       GetInventory().LockInventory(HIDE_INV_FROM_SCRIPT);
                   else
                       GetInventory().UnlockInventory(HIDE_INV_FROM_SCRIPT);
              }
  			      SetSynchDirty();
          }
       }

     override void OnStoreSave( ParamsWriteContext ctx )
      {
          super.OnStoreSave(ctx);
          Param3<bool, int, int> data = new Param3<bool, int, int>(m_CarLock_IsLocked, m_CarLockOwner, m_CarLockPassword);
          ctx.Write(data);
      }

    override bool OnStoreLoad( ParamsReadContext ctx, int version )
    {
      if ( !super.OnStoreLoad( ctx, version ) )
        return false;

      Param3<bool, int, int> data = new Param3<bool, int, int>(false, -1, -1);
      if (ctx.Read(data))
      {
        m_CarLock_IsLocked = data.param1;
        m_CarLockOwner     = data.param2;
        m_CarLockPassword  = data.param3;
      }
      SynchronizeValues();
      return true;
    }

    override bool IsInventoryVisible()
    {
      return !m_CarLock_IsLocked && (GetGame().GetPlayer() && (!GetGame().GetPlayer().GetCommand_Vehicle() || GetGame().GetPlayer().GetCommand_Vehicle().GetTransport() == this));
    }
};
#endif
