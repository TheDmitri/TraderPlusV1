class CfgPatches
{
	class TPSupportMenu
	{
		requiredVersion = 0.1;
		requiredAddons[] = {};
		units[] = {};
		weapons[] = {};
	};
};
class CfgMods
{
	class TPSupportMenu
	{
		dir = "TPSupportMenu";
		name = "TP";
		type = "mod";
		dependencies[] = {"GameLib", "Game", "World", "Mission"};
		class defs
		{
			class gameLibScriptModule
			{
				value = "";
				files[] = {"TPSupportMenu/scripts/Common"};
			};
			class gameScriptModule
			{
				value = "";
				files[] = {"TPSupportMenu/scripts/Common"};
			};
			class worldScriptModule
			{
				value = "";
				files[] = {"TPSupportMenu/scripts/Common"};
			};
			class missionScriptModule
			{
				value = "";
				files[] = {"TPSupportMenu/scripts/Common"};
			};
		};
	};
};
