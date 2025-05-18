class CfgPatches
{
	class CLDefine
	{
		requiredVersion = 0.1;
		requiredAddons[] = {};
		units[] = {};
		weapons[] = {};
	};
};
class CfgMods
{
	class CLDefine
	{
		dir = "CLDefine";
		name = "CL";
		type = "mod";
		dependencies[] = {"GameLib", "Game", "World", "Mission"};
		class defs
		{
			class gameLibScriptModule
			{
				value = "";
				files[] = {"CLDefine/scripts/Common"};
			};
			class gameScriptModule
			{
				value = "";
				files[] = {"CLDefine/scripts/Common"};
			};
			class worldScriptModule
			{
				value = "";
				files[] = {"CLDefine/scripts/Common"};
			};
			class missionScriptModule
			{
				value = "";
				files[] = {"CLDefine/scripts/Common"};
			};
		};
	};
};
