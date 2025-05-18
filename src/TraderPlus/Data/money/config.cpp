class CfgPatches
{
	class money
	{
		units[] = {"TraderPlus_Wallet", "TraderPlus_Bitcoin", "TraderPlus_Coin", "TraderPlus_Money_Ruble1_Coin", "TraderPlus_Money_Ruble2_Coin", "TraderPlus_Money_Ruble5_Coin", "TraderPlus_Money_Euro1", "TraderPlus_Money_Euro2", "TraderPlus_Money_Dollar1", "TraderPlus_Money_Dollar2", "TraderPlus_Money_Dollar5", "TraderPlus_Money_Dollar10", "TraderPlus_Money_Dollar20", "TraderPlus_Money_Dollar50", "TraderPlus_Money_Dollar100", "TraderPlus_Money_Euro5", "TraderPlus_Money_Euro10", "TraderPlus_Money_Euro20", "TraderPlus_Money_Euro50", "TraderPlus_Money_Euro100", "TraderPlus_Money_Euro200", "TraderPlus_Money_Euro500", "TraderPlus_Money_Ruble5", "TraderPlus_Money_Ruble10", "TraderPlus_Money_Ruble50", "TraderPlus_Money_Ruble100", "TraderPlus_Money_Ruble200", "TraderPlus_Money_Ruble500", "TraderPlus_Money_Ruble1000", "TraderPlus_Money_Ruble2000", "TraderPlus_Money_Ruble5000", "TraderPlus_Money_Wad_Ruble5", "TraderPlus_Money_Wad_Ruble10", "TraderPlus_Money_Wad_Ruble50", "TraderPlus_Money_DIM_Gold"};
		weapons[] = {};
		requiredVersion = 0.1;
		requiredAddons[] = {"DZ_Data"};
	};
};
class CfgVehicles
{
	class Inventory_Base;
	class TraderPlus_Wallet : Inventory_Base
	{
		scope = 2;
		displayName = "$STR_Wallet";
		descriptionShort = "$STR_Wallet_des";
		model = "TraderPlus\Data\Wallet\Wallet.p3d";
		weight = 130;
		itemSize[] = {2, 2};
		itemsCargoSize[] = {6, 4};
		canBeDigged = 0;
		allowOwnedCargoManipulation = 1;
	};
	class TraderPlusCoin_Base : Inventory_Base
	{
		scope = 0;
		overrideDrawArea = "5.0";
		rotationFlags = 16;
		lootCategory = "Materials";
		lootTag[] = {"Civilian", "Work"};
		itemSize[] = {1, 1};
		weight = 1;
		quantityBar = 0;
		canBeSplit = 1;
		isMeleeWeapon = 0;
		absorbency = 1;
	};
	class TraderPlus_Bitcoin : TraderPlusCoin_Base
	{
		scope = 2;
		displayName = "Bitcoin";
		descriptionShort = "a physical Bitcoin 0,03B";
		model = "TraderPlus\Data\money\bitcoin.p3d";
		canBeSplit = 1;
		varQuantityInit = 1;
		varQuantityMin = 0;
		varQuantityMax = 5;
		quantityBar = 0;
	};
	class TraderPlus_Coin : TraderPlusCoin_Base
	{
		scope = 2;
		displayName = "Gold Coin";
		descriptionShort = "Old russian Gold coin";
		model = "TraderPlus\Data\money\coin.p3d";
		canBeSplit = 1;
		varQuantityInit = 1;
		varQuantityMin = 0;
		varQuantityMax = 5;
		quantityBar = 0;
	};
	class TraderPlus_Money_Ruble1_Coin : TraderPlusCoin_Base
	{
		scope = 2;
		displayName = "1 Ruble Coin";
		descriptionShort = "Old russian coins";
		model = "TraderPlus\Data\money\1ruble.p3d";
		varQuantityInit = 1;
		varQuantityMin = 0;
		varQuantityMax = 100;
		varQuantityDestroyOnMin = 1;
		destroyOnEmpty = 1;
	};
	class TraderPlus_Money_Ruble2_Coin : TraderPlusCoin_Base
	{
		scope = 2;
		displayName = "2 Ruble Coin";
		descriptionShort = "Old russian coins";
		model = "TraderPlus\Data\money\2ruble.p3d";
		varQuantityInit = 1;
		varQuantityMin = 0;
		varQuantityMax = 100;
		varQuantityDestroyOnMin = 1;
		destroyOnEmpty = 1;
	};
	class TraderPlus_Money_Ruble5_Coin : TraderPlusCoin_Base
	{
		scope = 2;
		displayName = "5 Ruble Coin";
		descriptionShort = "Old russian coins";
		model = "TraderPlus\Data\money\ruble5.p3d";
		varQuantityInit = 1;
		varQuantityMin = 0;
		varQuantityMax = 100;
		varQuantityDestroyOnMin = 1;
		destroyOnEmpty = 1;
	};
	class TraderPlus_Money_Euro1 : TraderPlusCoin_Base
	{
		scope = 2;
		displayName = "1 Euro Coin";
		descriptionShort = "European coins";
		model = "TraderPlus\Data\money\1euro.p3d";
		varQuantityInit = 1;
		varQuantityMin = 0;
		varQuantityMax = 100;
	};
	class TraderPlus_Money_Euro2 : TraderPlusCoin_Base
	{
		scope = 2;
		displayName = "2 Euro Coin";
		descriptionShort = "European coins";
		model = "TraderPlus\Data\money\2euro.p3d";
		varQuantityInit = 1;
		varQuantityMin = 0;
		varQuantityMax = 100;
	};
	class TraderPlusMoney_Base : Inventory_Base
	{
		scope = 0;
		overrideDrawArea = "5.0";
		rotationFlags = 16;
		lootCategory = "Materials";
		lootTag[] = {"Civilian", "Work"};
		itemSize[] = {2, 1};
		weight = 1;
		quantityBar = 0;
		canBeSplit = 1;
		varQuantityInit = 1;
		varQuantityMin = 0;
		varQuantityMax = 500;
		varQuantityDestroyOnMin = 1;
		destroyOnEmpty = 1;
		isMeleeWeapon = 0;
		absorbency = 1;
	};
	class TraderPlus_Money_Dollar1 : TraderPlusMoney_Base
	{
		scope = 2;
		displayName = "1 US-Dollar Note";
		descriptionShort = "Remains of the US occupation before the outbreak of the epedemia.";
		model = "TraderPlus\Data\money\1dollar.p3d";
	};
	class TraderPlus_Money_Dollar2 : TraderPlusMoney_Base
	{
		scope = 2;
		displayName = "2 US-Dollar Note";
		descriptionShort = "Remains of the US occupation before the outbreak of the epedemia.";
		model = "TraderPlus\Data\money\2dollar.p3d";
	};
	class TraderPlus_Money_Dollar5 : TraderPlusMoney_Base
	{
		scope = 2;
		displayName = "5 US-Dollar Note";
		descriptionShort = "Remains of the US occupation before the outbreak of the epedemia.";
		model = "TraderPlus\Data\money\5dollar.p3d";
	};
	class TraderPlus_Money_Dollar10 : TraderPlusMoney_Base
	{
		scope = 2;
		displayName = "10 US-Dollar Note";
		descriptionShort = "Remains of the US occupation before the outbreak of the epedemia.";
		model = "TraderPlus\Data\money\10dollar.p3d";
	};
	class TraderPlus_Money_Dollar20 : TraderPlusMoney_Base
	{
		scope = 2;
		displayName = "20 US-Dollar Note";
		descriptionShort = "Remains of the US occupation before the outbreak of the epedemia.";
		model = "TraderPlus\Data\money\20dollar.p3d";
	};
	class TraderPlus_Money_Dollar50 : TraderPlusMoney_Base
	{
		scope = 2;
		displayName = "50 US-Dollar Note";
		descriptionShort = "Remains of the US occupation before the outbreak of the epedemia.";
		model = "TraderPlus\Data\money\50dollar.p3d";
	};
	class TraderPlus_Money_Dollar100 : TraderPlusMoney_Base
	{
		scope = 2;
		displayName = "100 US-Dollar Note";
		descriptionShort = "Remains of the US occupation before the outbreak of the epedemia.";
		model = "TraderPlus\Data\money\100dollar.p3d";
	};
	class TraderPlus_Money_Euro5 : TraderPlusMoney_Base
	{
		scope = 2;
		displayName = "5 Euro Note";
		descriptionShort = "European money before the outbreak of the epedemia.";
		model = "TraderPlus\Data\money\5euro.p3d";
	};
	class TraderPlus_Money_Euro10 : TraderPlusMoney_Base
	{
		scope = 2;
		displayName = "10 Euro Note";
		descriptionShort = "European money before the outbreak of the epedemia.";
		model = "TraderPlus\Data\money\10euro.p3d";
	};
	class TraderPlus_Money_Euro20 : TraderPlusMoney_Base
	{
		scope = 2;
		displayName = "20 Euro Note";
		descriptionShort = "European money before the outbreak of the epedemia.";
		model = "TraderPlus\Data\money\20euro.p3d";
	};
	class TraderPlus_Money_Euro50 : TraderPlusMoney_Base
	{
		scope = 2;
		displayName = "50 Euro Note";
		descriptionShort = "European money before the outbreak of the epedemia.";
		model = "TraderPlus\Data\money\50euro.p3d";
	};
	class TraderPlus_Money_Euro100 : TraderPlusMoney_Base
	{
		scope = 2;
		displayName = "100 Euro Note";
		descriptionShort = "European money before the outbreak of the epedemia.";
		model = "TraderPlus\Data\money\100euro.p3d";
	};
	class TraderPlus_Money_Euro200 : TraderPlusMoney_Base
	{
		scope = 2;
		displayName = "200 Euro Note";
		descriptionShort = "European money before the outbreak of the epedemia.";
		model = "TraderPlus\Data\money\200euro.p3d";
	};
	class TraderPlus_Money_Euro500 : TraderPlusMoney_Base
	{
		scope = 2;
		displayName = "500 Euro Note";
		descriptionShort = "European money before the outbreak of the epedemia.";
		model = "TraderPlus\Data\money\500euro.p3d";
	};
	class TraderPlus_Money_Ruble5 : TraderPlusMoney_Base
	{
		scope = 2;
		displayName = "5 Russian Ruble Note";
		descriptionShort = "This Currency was used in Chernarus before the Disease. It is now only used for trading.";
		model = "TraderPlus\Data\money\5ruble.p3d";
	};
	class TraderPlus_Money_Ruble10 : TraderPlusMoney_Base
	{
		scope = 2;
		displayName = "10 Russian Ruble Note";
		descriptionShort = "This Currency was used in Chernarus before the Disease. It is now only used for trading.";
		model = "TraderPlus\Data\money\10ruble.p3d";
	};
	class TraderPlus_Money_Ruble50 : TraderPlusMoney_Base
	{
		scope = 2;
		displayName = "50 Russian Ruble Note";
		descriptionShort = "This Currency was used in Chernarus before the Disease. It is now only used for trading.";
		model = "TraderPlus\Data\money\50ruble.p3d";
	};
	class TraderPlus_Money_Ruble100 : TraderPlusMoney_Base
	{
		scope = 2;
		displayName = "100 Russian Ruble Note";
		descriptionShort = "This Currency was used in Chernarus before the Disease. It is now only used for trading.";
		model = "TraderPlus\Data\money\100ruble.p3d";
	};
	class TraderPlus_Money_Ruble200 : TraderPlusMoney_Base
	{
		scope = 2;
		displayName = "200 Russian Ruble Note";
		descriptionShort = "This Currency was used in Chernarus before the Disease. It is now only used for trading.";
		model = "TraderPlus\Data\money\200ruble.p3d";
	};
	class TraderPlus_Money_Ruble500 : TraderPlusMoney_Base
	{
		scope = 2;
		displayName = "500 Russian Ruble Note";
		descriptionShort = "This Currency was used in Chernarus before the Disease. It is now only used for trading.";
		model = "TraderPlus\Data\money\500ruble.p3d";
	};
	class TraderPlus_Money_Ruble1000 : TraderPlusMoney_Base
	{
		scope = 2;
		displayName = "1000 Russian Ruble Note";
		descriptionShort = "This Currency was used in Chernarus before the Disease. It is now only used for trading.";
		model = "TraderPlus\Data\money\1000ruble.p3d";
	};
	class TraderPlus_Money_Ruble2000 : TraderPlusMoney_Base
	{
		scope = 2;
		displayName = "2000 Russian Ruble Note";
		descriptionShort = "This Currency was used in Chernarus before the Disease. It is now only used for trading.";
		model = "TraderPlus\Data\money\2000ruble.p3d";
	};
	class TraderPlus_Money_Ruble5000 : TraderPlusMoney_Base
	{
		scope = 2;
		displayName = "5000 Russian Ruble Note";
		descriptionShort = "This Currency was used in Chernarus before the Disease. It is now only used for trading.";
		model = "TraderPlus\Data\money\5000ruble.p3d";
	};
	class Box_Base;
	class Wad_Base : Box_Base
	{
		scope = 0;
		weight = 2540;
		absorbency = 0.8;
		itemSize[] = {2, 1};
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints = 80;
					healthLevels[] = {{1, {"TraderPlus\Data\money\textures\Money.rvmat"}}, {0.7, {"TraderPlus\Data\money\textures\Money.rvmat"}}, {0.5, {"TraderPlus\Data\money\textures\Money_damage.rvmat"}}, {0.3, {"TraderPlus\Data\money\textures\Money_damage.rvmat"}}, {0, {"TraderPlus\Data\money\textures\Money_destruct.rvmat"}}};
				};
			};
		};
		class AnimEvents
		{
			class SoundWeapon
			{
				class interact
				{
					soundset = "ammoboxUnpack_SoundSet";
					id = 70;
				};
			};
		};
	};
	class TraderPlus_Money_Wad_Ruble5 : Wad_Base
	{
		scope = 2;
		displayName = "5 Russian Ruble Wad";
		descriptionShort = "This Currency was used in Chernarus before the Disease. It is now only used for trading.";
		model = "TraderPlus\Data\money\wad5.p3d";
		class Resources
		{
			class Money_Ruble5
			{
				value = 100;
				variable = "quantity";
			};
		};
	};
	class TraderPlus_Money_Wad_Ruble10 : Wad_Base
	{
		scope = 2;
		displayName = "10 Russian Ruble Wad";
		descriptionShort = "This Currency was used in Chernarus before the Disease. It is now only used for trading.";
		model = "TraderPlus\Data\money\wad10.p3d";
		class Resources
		{
			class Money_Ruble10
			{
				value = 100;
				variable = "quantity";
			};
		};
	};
	class TraderPlus_Money_Wad_Ruble50 : Wad_Base
	{
		scope = 2;
		displayName = "50 Russian Ruble Wad";
		descriptionShort = "This Currency was used in Chernarus before the Disease. It is now only used for trading.";
		model = "TraderPlus\Data\money\wad50.p3d";
		class Resources
		{
			class Money_Ruble50
			{
				value = 50;
				variable = "quantity";
			};
		};
	};
	class TraderPlus_Money_DIM_Gold : TraderPlusCoin_Base
	{
		scope = 2;
		displayName = "$STR_DIM";
		descriptionShort = "$STR_DIM_des";
		model = "TraderPlus\Data\money\DIM_Coin\DIM.p3d";
		varQuantityInit = 1;
		varQuantityMin = 0;
		varQuantityMax = 100;
		varQuantityDestroyOnMin = 1;
		destroyOnEmpty = 1;
		hiddenSelections[] = {"camo"};
		hiddenSelectionsTextures[] = {"\TraderPlus\Data\money\DIM_Coin\Coin_co.paa"};
	};
};
