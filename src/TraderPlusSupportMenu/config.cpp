////////////////////////////////////////////////////////////////////
//DeRap: config.bin
//Produced from mikero's Dos Tools Dll version 9.45
//https://mikero.bytex.digital/Downloads
//'now' is Mon Nov 11 10:54:41 2024 : 'file' last modified on Fri Mar 25 18:18:47 2022
////////////////////////////////////////////////////////////////////

#define _ARMA_

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
		dependencies[] = {"GameLib","Game","World","Mission"};
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
