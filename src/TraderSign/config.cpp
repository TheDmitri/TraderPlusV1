class CfgPatches
{
	class Trader_Plus_Trader_Signs
	{
		units[] = {"Trade_Sign_Weapons", "Trade_Sign_Vehicles", "Trade_Sign_Food", "Trade_Sign_Clothing", "Trade_Sign_Equipment"};
		weapons[] = {};
		requiredVersion = 0.1;
		requiredAddons[] = {"DZ_Data"};
	};
};
class CfgMods
{
	class Trader_Plus_Trader_Signs
	{
		picture = "";
		action = "";
		hideName = 1;
		hidePicture = 1;
		name = "Trader_Plus_Trader_Signs";
		credits = "Dmitri";
		author = "Dave";
		authorID = "0";
		version = "1.0";
		extra = 0;
		type = "mod";
	};
};
class CfgVehicles
{
	class HouseNoDestruct;
	class Trade_Sign_Base : HouseNoDestruct
	{
		scope = 0;
		displayName = "Trader Sign";
		model = "TraderSign\TraderSign.p3d";
		StorageCategoty = 10;
		weight = 1000000;
		useEntityHierarchy = "true";
		slopeTolerance = 0.3;
		yawPitchRollLimit[] = {10, 10, 10};
		hiddenSelections[] = {"sign"};
	};
	class Trade_Sign_Weapons : Trade_Sign_Base
	{
		scope = 2;
		hiddenSelectionsTextures[] = {"TraderSign\Data\Signs\Weapons_Trader_Sign.paa"};
	};
	class Trade_Sign_Vehicles : Trade_Sign_Base
	{
		scope = 2;
		hiddenSelectionsTextures[] = {"TraderSign\Data\Signs\Vehicle_Trader_Sign.paa"};
	};
	class Trade_Sign_Food : Trade_Sign_Base
	{
		scope = 2;
		hiddenSelectionsTextures[] = {"TraderSign\Data\Signs\Food_Trader_Sign.paa"};
	};
	class Trade_Sign_Clothing : Trade_Sign_Base
	{
		scope = 2;
		hiddenSelectionsTextures[] = {"TraderSign\Data\Signs\Clothing_Trader_Sign.paa"};
	};
	class Trade_Sign_Equipment : Trade_Sign_Base
	{
		scope = 2;
		hiddenSelectionsTextures[] = {"TraderSign\Data\Signs\Equipment_Trader_Sign.paa"};
	};
};
