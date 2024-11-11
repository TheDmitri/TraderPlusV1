
static ref TraderPlusLoggerModule GetTraderPlusLogger()
{
    TraderPlusLoggerModule module = TraderPlusLoggerModule.Cast(CF_ModuleCoreManager.Get(TraderPlusLoggerModule));
    
    return module;
}

[CF_RegisterModule(TraderPlusLoggerModule)]
class TraderPlusLoggerModule: CF_ModuleGame
{
    FileHandle fileHandle;
    int networkSync_LogLevel;
    ref TraderPlusLoggingSettings settings;

    float dtime = 0;

    override void OnInit()
	{
		super.OnInit();
		RegisterNetSyncVariable("networkSync_LogLevel");
        EnableUpdate();
        EnableMissionStart();
	}

    override void OnMissionStart(Class sender, CF_EventArgs args)
	{
		super.OnMissionStart(sender, args);
		
        fileHandle = CreateNewLogFile();
        
        if(GetGame().IsServer())
        {
            settings = TraderPlusLoggingSettings.Load();

            networkSync_LogLevel = settings.logLevel;
            SetSynchDirty();
        }
	}

    override void OnUpdate(Class sender, CF_EventArgs args)
	{
		auto update = CF_EventUpdateArgs.Cast(args);

		dtime += update.DeltaTime;

		if (GetGame().IsServer() && settings && dtime > settings.refreshRateInSeconds)
		{
            dtime = 0;
            settings = TraderPlusLoggingSettings.Load();

            networkSync_LogLevel = settings.logLevel;
            SetSynchDirty();
		}
	}

    void MakeDirectoryIfNotExists()
    {
        //we check if config folders exist, if not we create them
		if ( !FileExist( TRADERPLUS_CONFIG_ROOT_SERVER ) )
			MakeDirectory( TRADERPLUS_CONFIG_ROOT_SERVER );

		if ( !FileExist( TRADERPLUS_LOGGER_DIR_SERVER ) )
			MakeDirectory( TRADERPLUS_LOGGER_DIR_SERVER );
    }

    FileHandle CreateNewLogFile()	
	{
        MakeDirectoryIfNotExists();

		string filePath = string.Format(TRADERPLUS_LOGGER_FILENAME, GenerateFullTimeStamp());
		//Check the file does not already exist ... fuck knows how it would
        Print(filePath);
        fileHandle = OpenFile(filePath, FileMode.WRITE);

		//File created
		if (fileHandle != 0)
		{
			FPrintln(fileHandle, "Creation Time: " + GenerateFullTimeStamp());
			return fileHandle;
		}
		return NULL;
	}

	void Log(string content, TraderPlusLogLevel logLevel) 
	{
        if(logLevel >= networkSync_LogLevel)
        {
            string timeStamp = GenerateShortTimeString();
            FPrintln(fileHandle, timeStamp + " | " + GetLogLevelString(logLevel) + " | " + content);
        }
	}

	void LogError(string content) 
	{
		thread Log(content, TraderPlusLogLevel.Error);
	}

	void LogInfo(string content) 
	{
		thread Log(content, TraderPlusLogLevel.Info);
	}
	
	void LogDebug(string content) 
	{
		thread Log(content, TraderPlusLogLevel.Debug);
	}

	void LogWarning(string content) 
	{
		thread Log(content, TraderPlusLogLevel.Warning);
	}

	string GenerateShortDateString() 
	{
		int year, month, day;
		GetYearMonthDay(year, month, day);
		string ret = "" + year + "_" + month + "_" + day;
		return ret;
	}

	string GenerateShortTimeString() 
	{
		int hour, minute, second;
		GetHourMinuteSecond(hour, minute, second);
		string ret = "" + hour + "-" + minute + "-" + second;
		return ret;
	}

	string GenerateFullTimeStamp() 
	{
		string dateStr = GenerateShortDateString();
		string timeStr = GenerateShortTimeString();
		string ret = "" + dateStr + "-" + timeStr;
		return ret;
	}

	string GetLogLevelString(TraderPlusLogLevel logLevel)
	{
		switch (logLevel)
		{
			case TraderPlusLogLevel.Info:
				return "[INFO]";
			case TraderPlusLogLevel.Error:
				return "[ERROR]";
			case TraderPlusLogLevel.Debug:
				return "[DEBUG]";
            case TraderPlusLogLevel.Warning:
				return "[WARNING]";
			default:
				return "";
		}

        return "";
	}
}