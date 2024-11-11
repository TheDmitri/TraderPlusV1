const string GM_CONFIG_ROOT_SERVER = "$profile:TraderPlus\\";
const string GM_LOGGER_DIR_SERVER = GM_CONFIG_ROOT_SERVER + "TraderPlusLogs\\";
const string GM_LOGGER_PREFIX_SERVER = "Garage_";
const string GM_CONFIG_DIR_SERVER = GM_CONFIG_ROOT_SERVER + "TraderPlusConfig\\";
const string GM_DB_DIR_SERVER = GM_CONFIG_ROOT_SERVER + "TraderPlusGarageDatabase\\";
const string GM_DB_DIR_PLAYER = GM_DB_DIR_SERVER + "%1\\";
const string GM_CONFIG = GM_CONFIG_DIR_SERVER + "TraderPlusGarageConfig.json";
const string GM_DATABASE = GM_DB_DIR_SERVER + "Garage_";

const int    GARAGEVEHICLEATTACHMENT = -1;
const int    GARAGEVEHICLECARGO      = -2;

const int    TP_ID_GARAGE = -3;

/*Class that deals with the log system*/
static ref GMLogger g_GMLogger;
static GMLogger GetGMLogger(){
	if (!g_GMLogger) {
		g_GMLogger = new GMLogger();
	}
	return g_GMLogger;
}
