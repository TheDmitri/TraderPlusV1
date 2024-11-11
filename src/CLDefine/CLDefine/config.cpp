////////////////////////////////////////////////////////////////////
//DeRap: config.bin
//Produced from mikero's Dos Tools Dll version 9.45
//https://mikero.bytex.digital/Downloads
//'now' is Mon Nov 11 10:51:45 2024 : 'file' last modified on Fri Jan 13 17:00:46 2023
////////////////////////////////////////////////////////////////////

#define _ARMA_

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
		dependencies[] = {"GameLib","Game","World","Mission"};
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
