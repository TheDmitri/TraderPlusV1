class CfgPatches
{
	class CarLock_Script
	{
		units[] = {"CarLockPick", "CarLock"};
		weapons[] = {};
		requiredVersion = 0.1;
		requiredAddons[] = {"DZ_Data", "SafeZone_Script"};
	};
};
class CfgMods
{
	class CarLock
	{
		dir = "CarLock";
		picture = "";
		action = "";
		hideName = 1;
		hidePicture = 1;
		name = "CarLock";
		credits = "TheDmitri";
		author = "TheDmitri";
		authorID = "0";
		version = "1.1";
		extra = 0;
		type = "mod";
		inputs = "CarLock/scripts/3_Game/inputs.xml";
		dependencies[] = {"Game", "World", "Mission"};
		class defs
		{
			class gameScriptModule
			{
				value = "";
				files[] = {"MuchCarKey/Scripts/Common", "MuchCarKeyDefines/scripts/Common", "CLDefine/scripts/Common", "TPDefine/scripts/Common", "TM/Trader/scripts/defines", "CarCover/Scripts/Common", "TPDefine/scripts/Common", "TraderPlus/scripts/Common", "CarLock/scripts/Common", "CarLock/scripts/3_Game"};
			};
			class worldScriptModule
			{
				value = "";
				files[] = {"MuchCarKey/Scripts/Common", "MuchCarKeyDefines/scripts/Common", "CLDefine/scripts/Common", "TPDefine/scripts/Common", "TM/Trader/scripts/defines", "CarCover/Scripts/Common", "TPDefine/scripts/Common", "TraderPlus/scripts/Common", "CarLock/scripts/Common", "CarLock/scripts/4_World"};
			};
			class missionScriptModule
			{
				value = "";
				files[] = {"MuchCarKey/Scripts/Common", "MuchCarKeyDefines/scripts/Common", "CLDefine/scripts/Common", "TPDefine/scripts/Common", "TM/Trader/scripts/defines", "CarCover/Scripts/Common", "TPDefine/scripts/Common", "TraderPlus/scripts/Common", "CarLock/scripts/Common", "CarLock/scripts/5_Mission"};
			};
		};
	};
};
class CfgVehicles
{
	class Inventory_Base;
	class CarLockPick : Inventory_Base
	{
		scope = 2;
		displayName = "CarLockPick";
		descriptionShort = "Use this tool to break the lock on someone's else car. Keep in mind that stealing is bad !";
		model = "\dz\gear\tools\Lockpick.p3d";
		animClass = "Knife";
		RestrainUnlockType = 0;
		rotationFlags = 17;
		isLockpick = 1;
		itemSize[] = {1, 2};
		weight = 190;
		fragility = 0.01;
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints = 50;
					healthLevels[] = {{1, {"DZ\gear\tools\data\Lockpick.rvmat"}}, {0.7, {"DZ\gear\tools\data\Lockpick.rvmat"}}, {0.5, {"DZ\gear\tools\data\Lockpick_damage.rvmat"}}, {0.3, {"DZ\gear\tools\data\Lockpick_damage.rvmat"}}, {0, {"DZ\gear\tools\data\Lockpick_destruct.rvmat"}}};
				};
			};
		};
		class MeleeModes
		{
			class Default
			{
				ammo = "MeleeLightBlunt";
				range = 1;
			};
			class Heavy
			{
				ammo = "MeleeLightBlunt_Heavy";
				range = 1;
			};
			class Sprint
			{
				ammo = "MeleeLightBlunt_Heavy";
				range = 2.8;
			};
		};
		class AnimEvents
		{
			class SoundWeapon
			{
				class lockpicker_move_in
				{
					soundSet = "lockpicker_move_in_SoundSet";
					id = 110202;
				};
				class lockpicker_move_out
				{
					soundSet = "lockpicker_move_out_SoundSet";
					id = 110203;
				};
				class lockpicker_loop
				{
					soundSet = "lockpicker_loop_SoundSet";
					id = 203;
				};
				class lockpicker_end
				{
					soundSet = "lockpicker_end_SoundSet";
					id = 110204;
				};
			};
		};
	};
	class CarLock : Inventory_Base
	{
		scope = 2;
		weight = 175;
		displayName = "CarLock";
		descriptionShort = "CarLock can be attached to a car in order to lock a car with an 8 digits password. Once set, password is saved. Can be used alone or with friends.";
		model = "CarLock\CarLock.p3d";
		rotationFlags = 17;
		itemSize[] = {2, 2};
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints = 100;
					healthLevels[] = {{1.01, {"DZ\gear\food\data\powdered_milk.rvmat"}}, {0.07, {"DZ\gear\food\data\powdered_milk.rvmat"}}, {0.5, {"DZ\gear\food\data\powdered_milk_damage.rvmat"}}, {0.3, {"DZ\gear\food\data\powdered_milk_damage.rvmat"}}, {0.01, {"DZ\gear\food\data\powdered_milk_destruct.rvmat"}}};
				};
			};
		};
	};
};
class CfgSoundShaders
{
	class cl_CarLock_CarAlarm_SoundShader
	{
		samples[] = {{"CarLock\Sounds\CarAlarm", 1}};
		radius = 500;
		range = 500;
		volume = 1;
		rangeCurve[] = {{0, 1}, {50, 1}, {500, 1}};
	};
	class cl_CarLock_CarLock_SoundShader
	{
		samples[] = {{"CarLock\Sounds\CarLock", 1}};
		range = 15;
		volume = 1.2;
	};
	class cl_CarLock_CarUnlock_SoundShader
	{
		samples[] = {{"CarLock\Sounds\CarUnlock", 1}};
		range = 15;
		volume = 1.2;
	};
};
class CfgSoundSets
{
	class cl_CarLock_CarAlarm_SoundSet
	{
		soundShaders[] = {"cl_CarLock_CarAlarm_SoundShader"};
	};
	class cl_CarLock_CarLock_SoundSet
	{
		soundShaders[] = {"cl_CarLock_CarLock_SoundShader"};
	};
	class cl_CarLock_CarUnlock_SoundSet
	{
		soundShaders[] = {"cl_CarLock_CarUnlock_SoundShader"};
	};
};
