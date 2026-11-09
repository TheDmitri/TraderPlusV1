const string TRADERPLUS_BEANS_URL = "https://dayzbeanslauncher.com/download";
const string TRADERPLUS_BEANS_DEFAULT_CODE = "MWQ8SCQ7";
const string TRADERPLUS_BEANS_INFO = "The small DayZ Beans Launcher badge in the trader menu opens the launcher download page. Put your own partner code in PartnerCode (get one on dayzbeanslauncher.com) and every player who installs through your badge earns credits for your server: events, promotion and more. Keep the default code to support the TraderPlus maintainer, or set HideBadge to 1 to remove the badge.";

class TraderPlusBeansLauncherSettings
{
	string Info;
	string PartnerCode;
	bool   HideBadge;

	// Letters, digits, '-' and '_' only, so the code cannot change the link.
	static string CleanCode(string code)
	{
		string allowed = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789-_";
		string clean = "";
		for (int i = 0; i < code.Length() && clean.Length() < 32; i++)
		{
			string c = code.Get(i);
			if (allowed.Contains(c))
				clean += c;
		}
		return clean;
	}

	static string Link(string code)
	{
		code = CleanCode(code);
		if (code == "")
			return TRADERPLUS_BEANS_URL;
		return TRADERPLUS_BEANS_URL + "?ref=" + code;
	}

	void Save()
	{
		TraderPlusJsonLoader<TraderPlusBeansLauncherSettings>.SaveToFile(TRADERPLUS_BEANS_CONFIG, this);
	}

	static ref TraderPlusBeansLauncherSettings Load()
	{
		TraderPlusBeansLauncherSettings settings = new TraderPlusBeansLauncherSettings();
		if (FileExist(TRADERPLUS_BEANS_CONFIG))
		{
			TraderPlusJsonLoader<TraderPlusBeansLauncherSettings>.LoadFromFile(TRADERPLUS_BEANS_CONFIG, settings);
			if (settings.Info != TRADERPLUS_BEANS_INFO)
			{
				settings.Info = TRADERPLUS_BEANS_INFO;
				settings.Save();
			}
		}
		else
		{
			settings.Info = TRADERPLUS_BEANS_INFO;
			settings.PartnerCode = TRADERPLUS_BEANS_DEFAULT_CODE;
			settings.HideBadge = false;
			settings.Save();
		}
		return settings;
	}
}
