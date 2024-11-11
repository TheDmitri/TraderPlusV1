static ref SafeZoneLocations GetSZConfigClient()	{
	TraderPlusSafeZone module = TraderPlusSafeZone.Cast(CF_ModuleCoreManager.Get(TraderPlusSafeZone));
	return module.m_SZSafeZoneClient.m_SafeZoneLocations;
}

static bool GetSafeZoneStatut()	{
	TraderPlusSafeZone module = TraderPlusSafeZone.Cast(CF_ModuleCoreManager.Get(TraderPlusSafeZone));
  return module.m_SZSafeZoneClient.SZStatut;
}

static ref SafeAreaSettings GetSZConfig()	{
	TraderPlusSafeZone module = TraderPlusSafeZone.Cast(CF_ModuleCoreManager.Get(TraderPlusSafeZone));
  return module.m_ActiveSafeAreaSettings;
}

static ref SZSafeZoneClient GetSafeZoneClient(){
	TraderPlusSafeZone module = TraderPlusSafeZone.Cast(CF_ModuleCoreManager.Get(TraderPlusSafeZone));
  return module.m_SZSafeZoneClient;
}
