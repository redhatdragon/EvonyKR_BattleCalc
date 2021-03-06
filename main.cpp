#include <stdint.h>
#include <memory.h>
#include <stdlib.h>  //TODO: might be included in iostream must check for that later.
#include <iostream>

constexpr uint32_t MAX_ROUNDS = 10;
constexpr uint32_t MAX_TROOP_TYPES = 4;
constexpr uint32_t MAX_TIER_TYPES = 15;
constexpr uint32_t MAX_FORT_TYPES = 4;

enum TROOP_TYPE {

	MOUNTED,
	GROUND,
	RANGED,
	SIEGE
};

enum FORT_TYPE {

	TRAP,
	ROCK,
	ABATIS,
	ARROW
};

enum ROLE {

	DEFENDER,
	ATTACKER
};

struct Team {



		uint32_t troopCount[MAX_TROOP_TYPES][MAX_TIER_TYPES];
		uint32_t buffFlat[MAX_TROOP_TYPES][3];
		float buffPercent[MAX_TROOP_TYPES][3];
		unsigned int buffSpeed[2];
		unsigned int buffRange[2];

};

//Devoid of all notions of tiers or numbers, meant purely to be used in battle calculations.
//This is the combined totals of all tiers of troops of one type (after buffs).
//Speed and range are not totaled, rhealth and attack however are.

struct Troop {

	uint64_t health, attack;
	uint16_t speed, range;  //Needed as these may be modified by walls and other odd variables.
};

//Similar in layout to Troop, meant to be the calculated sum of all tiers of the same type.
struct Fortification {
	uint64_t health, attack;
	uint16_t range;  //TODO: Is this needed?  Or is it constant?
};

struct TroopSlot {  //TODO:  Need better name, used to describe client visable troops before they get bundled for battle calculations.

	uint32_t count;
	uint8_t tier;
};  //TODO: make this a struct of arrays in memory layout somehow.

struct FortSlot {  //TODO: Same issues as TroopSlot.

	uint32_t count;
	uint8_t tier;
};

struct Hero {
	uint16_t pol, attack, defense, leadership;  //TODO: Are all these one byte large?
};

//TODO: Impliment gear and tech modifier layouts.

struct Battlefield {

	uint32_t startingRange;  //Might fit in 2 bytes?
	Fortification forts[MAX_FORT_TYPES];  //[FORT_TYPE]

	//TODO: Should this be type,team or team,type?  (optimization problem not an essential one >.<)
	Troop troops[MAX_TROOP_TYPES][2];  //[{TROOP_TYPE, TEAM]

	//May need this to help avoid refactoring if the data is re arranged.
	__forceinline Troop& getTroop(TROOP_TYPE type, int team) {
		return troops[type][team];
	}
};

void setTroopCount(Team* team, TROOP_TYPE type, unsigned int tier, unsigned int count);
void setTroopBuffPercent(Team* team, TROOP_TYPE type, float buffAttack, float buffDefense, float buffHP);
void setTroopBuffFlat(Team* team, TROOP_TYPE type, unsigned int buffAttack, unsigned int buffDefense, unsigned int buffHP);
void combat(Team* attacker, Team* defender);

unsigned int troopStats[MAX_TROOP_TYPES][MAX_TIER_TYPES][3];

// Attacker Stats
unsigned int attackerTroopStats[MAX_TROOP_TYPES][3] = { 0 };  //48 bytes
//memset(attackerTroopStats, 0, sizeof(attackerTroopStats));

// Defender Stats
unsigned int defenderTroopStats[MAX_TROOP_TYPES][3] = { 0 };  //48 bytes
//memset(defenderTroopStats, 0, sizeof(defenderTroopStats));

int main() {

	

	Battlefield battlefield = {};
	//Grab data from battlefield.txt...  (Or stick to hard coded data)

	Team* attacker = new Team(); memset(attacker, 0, sizeof(Team));
	Team* defender = new Team(); memset(defender, 0, sizeof(Team));

	// Set The Attackers Troops
	setTroopCount(attacker, MOUNTED, 12, 48292);
	setTroopCount(attacker, GROUND, 11, 261674);
	setTroopCount(attacker, RANGED, 11, 196495);
	setTroopCount(attacker, RANGED, 12, 24410);

	uint32_t example[MAX_TIER_TYPES] = { 0, 2148 , 1398 , 5010, 4710, 13323, 10300, 34908 };

	// Set The Defenders Troops
	setTroopCount(defender, SIEGE, 2, 2148);
	setTroopCount(defender, SIEGE, 3, 1398);
	setTroopCount(defender, SIEGE, 4, 5010);
	setTroopCount(defender, SIEGE, 5, 4710);
	setTroopCount(defender, SIEGE, 6, 13323);
	setTroopCount(defender, SIEGE, 7, 10300);
	setTroopCount(defender, SIEGE, 8, 34908);
	setTroopCount(defender, MOUNTED, 8, 34908);
	setTroopCount(defender, MOUNTED, 7, 11326);
	setTroopCount(defender, GROUND, 8, 29964);
	setTroopCount(defender, RANGED, 8, 8705);
	setTroopCount(defender, SIEGE, 8, 25110);
	setTroopCount(defender, MOUNTED, 9, 104420);
	setTroopCount(defender, GROUND, 9, 29216);
	setTroopCount(defender, RANGED, 9, 19097);
	setTroopCount(defender, SIEGE, 9, 25011);
	setTroopCount(defender, MOUNTED, 10, 128627);
	setTroopCount(defender, RANGED, 10, 17410);
	setTroopCount(defender, GROUND, 10, 24347);
	setTroopCount(defender, SIEGE, 10, 38801);

	// Set The Attackers Troop Buffs
	setTroopBuffPercent(attacker, GROUND, 4.11, 3.72, 3.45);
	setTroopBuffPercent(attacker, RANGED, 4.21, 3.72, 3.87);
	setTroopBuffPercent(attacker, MOUNTED, 5.27, 4.89, 3.96);
	setTroopBuffPercent(attacker, SIEGE, 4.23, 4.07, 2.82);

	// Set The Defenders Troop Buffs
	setTroopBuffPercent(defender, RANGED, 0.75, 0.61, 0.35);
	setTroopBuffPercent(defender, MOUNTED, 0.85, 0.56, 0.39);
	setTroopBuffPercent(defender, GROUND, 0.57, 0.81, 0.35);
	setTroopBuffPercent(defender, SIEGE, 0.87, 0.61, 0.35);

	Team attackerClone = *attacker;
	Team defenderClone = *defender;

	//Run calculation...
	combat(&attackerClone, &defenderClone);

	//Print results...
	return 0;
}

void setTroopStateTables() {

	// [TROOP_TYPE: MOUNTED = 0, GROUND = 1, RANGED = 2, SIEGE = 3] [TIER] [Attack/Defense/HP = 0/1/2]

	// Mounted ---------------------------------------------------------

	// Tier 1 Mounted
	troopStats[0][1][0] = 222;
	troopStats[0][1][1] = 150;
	troopStats[0][1][2] = 400;

	// Tier 2 Mounted
	troopStats[0][2][0] = 303;
	troopStats[0][2][1] = 200;
	troopStats[0][2][2] = 540;

	// Tier 3 Mounted
	troopStats[0][3][0] = 414;
	troopStats[0][3][1] = 270;
	troopStats[0][3][2] = 730;

	// Tier 4 Mounted
	troopStats[0][4][0] = 556;
	troopStats[0][4][1] = 360;
	troopStats[0][4][2] = 990;

	// Tier 5 Mounted
	troopStats[0][5][0] = 747;
	troopStats[0][5][1] = 490;
	troopStats[0][5][2] = 1340;

	// Tier 6 Mounted
	troopStats[0][6][0] = 1010;
	troopStats[0][6][1] = 660;
	troopStats[0][6][2] = 1810;

	// Tier 7 Mounted
	troopStats[0][7][0] = 1364;
	troopStats[0][7][1] = 890;
	troopStats[0][7][2] = 2440;

	// Tier 8 Mounted
	troopStats[0][8][0] = 1834;
	troopStats[0][8][1] = 1200;
	troopStats[0][8][2] = 3290;

	// Tier 9 Mounted
	troopStats[0][9][0] = 2485;
	troopStats[0][9][1] = 1620;
	troopStats[0][9][2] = 4440;

	// Tier 10 Mounted
	troopStats[0][10][0] = 3353;
	troopStats[0][10][1] = 2190;
	troopStats[0][10][2] = 5990;

	// Tier 11 Mounted
	troopStats[0][11][0] = 4192;
	troopStats[0][11][1] = 2740;
	troopStats[0][11][2] = 7490;

	// Tier 12 Mounted
	troopStats[0][12][0] = 5239;
	troopStats[0][12][1] = 3425;
	troopStats[0][12][2] = 9362;

	// Tier 13 Mounted
	troopStats[0][13][0] = 5858;
	troopStats[0][13][1] = 3830;
	troopStats[0][13][2] = 10480;

	// Tier 14 Mounted
	troopStats[0][14][0] = 6737;
	troopStats[0][14][1] = 4400;
	troopStats[0][14][2] = 12050;

	// GROUND ----------------------------------------------------------

	// Tier 1 Ground
	troopStats[1][1][0] = 106;
	troopStats[1][1][1] = 300;
	troopStats[1][1][2] = 600;

	// Tier 2 Ground
	troopStats[1][2][0] = 147;
	troopStats[1][2][1] = 410;
	troopStats[1][2][2] = 810;

	// Tier 3 Ground
	troopStats[1][3][0] = 199;
	troopStats[1][3][1] = 550;
	troopStats[1][3][2] = 1090;

	// Tier 4 Ground
	troopStats[1][4][0] = 273;
	troopStats[1][4][1] = 740;
	troopStats[1][4][2] = 1470;

	// Tier 5 Ground
	troopStats[1][5][0] = 377;
	troopStats[1][5][1] = 1000;
	troopStats[1][5][2] = 1980;

	// Tier 6 Ground
	troopStats[1][6][0] = 494;
	troopStats[1][6][1] = 1350;
	troopStats[1][6][2] = 2670;

	// Tier 7 Ground
	troopStats[1][7][0] = 662;
	troopStats[1][7][1] = 1820;
	troopStats[1][7][2] = 3600;

	// Tier 8 Ground
	troopStats[1][8][0] = 892;
	troopStats[1][8][1] = 2460;
	troopStats[1][8][2] = 4860;

	// Tier 9 Ground
	troopStats[1][9][0] = 1208;
	troopStats[1][9][1] = 3320;
	troopStats[1][9][2] = 6560;

	// Tier 10 Ground
	troopStats[1][10][0] = 1627;
	troopStats[1][10][1] = 4480;
	troopStats[1][10][2] = 8860;

	// Tier 11 Ground
	troopStats[1][11][0] = 2037;
	troopStats[1][11][1] = 5600;
	troopStats[1][11][2] = 11080;

	// Tier 12 Ground
	troopStats[1][12][0] = 2546;
	troopStats[1][12][1] = 7000;
	troopStats[1][12][2] = 13850;

	// Tier 13 Ground
	troopStats[1][13][0] = 3055;
	troopStats[1][13][1] = 8400;
	troopStats[1][13][2] = 16620;

	// Tier 14 Ground
	troopStats[1][14][0] = 3748;
	troopStats[1][14][1] = 10330;
	troopStats[1][14][2] = 20440;

	// RANGED ---------------------------------------------------

	// Tier 1 Ranged
	troopStats[2][1][0] = 131;
	troopStats[2][1][1] = 101;
	troopStats[2][1][2] = 252;

	// Tier 2 Ranged
	troopStats[2][2][0] = 180;
	troopStats[2][2][1] = 140;
	troopStats[2][2][2] = 340;

	// Tier 3 Ranged
	troopStats[2][3][0] = 240;
	troopStats[2][3][1] = 190;
	troopStats[2][3][2] = 460;

	// Tier 4 Ranged
	troopStats[2][4][0] = 320;
	troopStats[2][4][1] = 260;
	troopStats[2][4][2] = 620;

	// Tier 5 Ranged
	troopStats[2][5][0] = 430;
	troopStats[2][5][1] = 350;
	troopStats[2][5][2] = 840;

	// Tier 6 Ranged
	troopStats[2][6][0] = 580;
	troopStats[2][6][1] = 470;
	troopStats[2][6][2] = 1130;

	// Tier 7 Ranged
	troopStats[2][7][0] = 959;
	troopStats[2][7][1] = 630;
	troopStats[2][7][2] = 1530;

	// Tier 8 Ranged
	troopStats[2][8][0] = 1050;
	troopStats[2][8][1] = 850;
	troopStats[2][8][2] = 2070;

	// Tier 9 Ranged
	troopStats[2][9][0] = 1420;
	troopStats[2][9][1] = 1150;
	troopStats[2][9][2] = 2790;

	// Tier 10 Ranged
	troopStats[2][10][0] = 1920;
	troopStats[2][10][1] = 1550;
	troopStats[2][10][2] = 3770;

	// Tier 11 Ranged
	troopStats[2][11][0] = 2400;
	troopStats[2][11][1] = 1940;
	troopStats[2][11][2] = 4720;

	// Tier 12 Ranged
	troopStats[2][12][0] = 3000;
	troopStats[2][12][1] = 2425;
	troopStats[2][12][2] = 5900;

	// Tier 13 Ranged
	troopStats[2][13][0] = 3450;
	troopStats[2][13][1] = 2780;
	troopStats[2][13][2] = 6780;

	// Tier 14 Ranged
	troopStats[2][14][0] = 4070;
	troopStats[2][14][1] = 3280;
	troopStats[2][14][2] = 8000;

	// SIEGE --------------------------------------------------------

	// Tier 1 Siege
	troopStats[3][1][0] = 100;
	troopStats[3][1][1] = 50;
	troopStats[3][1][2] = 100;

	// Tier 2 Siege
	troopStats[3][2][0] = 140;
	troopStats[3][2][1] = 70;
	troopStats[3][2][2] = 140;

	// Tier 3 Siege
	troopStats[3][3][0] = 190;
	troopStats[3][3][1] = 90;
	troopStats[3][3][2] = 190;

	// Tier 4 Siege
	troopStats[3][4][0] = 260;
	troopStats[3][4][1] = 120;
	troopStats[3][4][2] = 260;

	// Tier 5 Siege
	troopStats[3][5][0] = 350;
	troopStats[3][5][1] = 160;
	troopStats[3][5][2] = 350;

	// Tier 6 Siege
	troopStats[3][6][0] = 470;
	troopStats[3][6][1] = 220;
	troopStats[3][6][2] = 470;

	// Tier 7 Siege
	troopStats[3][7][0] = 630;
	troopStats[3][7][1] = 300;
	troopStats[3][7][2] = 630;

	// Tier 8 Siege
	troopStats[3][8][0] = 850;
	troopStats[3][8][1] = 410;
	troopStats[3][8][2] = 850;

	// Tier 9 Siege
	troopStats[3][9][0] = 1150;
	troopStats[3][9][1] = 550;
	troopStats[3][9][2] = 1150;

	// Tier 10 Siege
	troopStats[3][10][0] = 1550;
	troopStats[3][10][1] = 740;
	troopStats[3][10][2] = 1550;

	// Tier 11 Siege
	troopStats[3][11][0] = 1940;
	troopStats[3][11][1] = 930;
	troopStats[3][11][2] = 1940;

	// Tier 12 Siege
	troopStats[3][12][0] = 2425;
	troopStats[3][12][1] = 1162;
	troopStats[3][12][2] = 2425;

	// Tier 13 Siege
	troopStats[3][13][0] = 2780;
	troopStats[3][13][1] = 1330;
	troopStats[3][13][2] = 2780;

	// Tier 14 Siege
	troopStats[3][14][0] = 3280;
	troopStats[3][14][1] = 1560;
	troopStats[3][14][2] = 3280;
}

void setTroopCount(Team* team, TROOP_TYPE type, unsigned int tier, unsigned int count) {

	team->troopCount[type][tier] = count;
}

// Set Troop Buff Percent (%)
void setTroopBuffPercent(Team* team, TROOP_TYPE type, float buffAttack, float buffDefense, float buffHP) {

	team->buffPercent[type][0] = buffAttack;
	team->buffPercent[type][1] = buffDefense;
	team->buffPercent[type][2] = buffHP;
}

// Set Troop Buff Flat (+)
void setTroopBuffFlat(Team* team, TROOP_TYPE type, unsigned int buffAttack, unsigned int buffDefense, unsigned int buffHP) {

	team->buffFlat[type][0] = buffAttack;
	team->buffFlat[type][1] = buffDefense;
	team->buffFlat[type][2] = buffHP;
}

// Returns Troop Type Combined Attack For Attacker / Defender
unsigned int getAttack(ROLE inRole, TROOP_TYPE inType) {

	if (inRole = ATTACKER) {
		
		return attackerTroopStats[int(inType)][0];
	}

	else if (inRole = DEFENDER) {

		return defenderTroopStats[int(inType)][0];
	}
}

// Returns Troop Type Combined Defense For Attacker / Defender
unsigned int getDefense(ROLE inRole, TROOP_TYPE inType) {

	if (inRole = ATTACKER) {
		
		return attackerTroopStats[int(inType)][1];
	}

	else if (inRole = DEFENDER) {

		return defenderTroopStats[int(inType)][1];
	}
}

// Returns Troop Type Combined HP For Attacker / Defender
unsigned int getHP(ROLE inRole, TROOP_TYPE inType) {

	if (inRole = ATTACKER) {
		
		return attackerTroopStats[int(inType)][2];
	}

	else if (inRole = DEFENDER) {

		return defenderTroopStats[int(inType)][2];
	}
}

void combat(Team * attacker, Team * defender) {

	// Attacker Stats
	for (unsigned int i = 1; i < 15; i++) {

		// MOUNTED ----------------------------------------------------------------------------------------------------------
		unsigned int troopCount = attacker->troopCount[0][i];
		attackerTroopStats[0][0] += troopCount * troopStats[0][i][0] * attacker->buffPercent[0][0]; // Attack
		attackerTroopStats[0][1] += troopCount * troopStats[0][i][1] * attacker->buffPercent[0][1]; // Defense
		attackerTroopStats[0][2] += troopCount * troopStats[0][i][2] * attacker->buffPercent[0][2]; // HP

		// GROUND -----------------------------------------------------------------------------------------------------------
		troopCount = attacker->troopCount[1][i];
		attackerTroopStats[1][0] += troopCount * troopStats[1][i][0] * attacker->buffPercent[1][0]; // Attack
		attackerTroopStats[1][1] += troopCount * troopStats[1][i][1] * attacker->buffPercent[1][1]; // Defense
		attackerTroopStats[1][2] += troopCount * troopStats[1][i][2] * attacker->buffPercent[1][2]; // HP

		// RANGED -----------------------------------------------------------------------------------------------------------
		troopCount = attacker->troopCount[2][i];
		attackerTroopStats[2][0] += troopCount * troopStats[2][i][0] * attacker->buffPercent[2][0]; // Attack
		attackerTroopStats[2][1] += troopCount * troopStats[2][i][1] * attacker->buffPercent[2][1]; // Defense
		attackerTroopStats[2][2] += troopCount * troopStats[2][i][2] * attacker->buffPercent[2][2]; // HP

		// SIEGE ------------------------------------------------------------------------------------------------------------
		troopCount = attacker->troopCount[3][i];
		attackerTroopStats[3][0] += troopCount * troopStats[3][i][0] * attacker->buffPercent[3][0]; // Attack
		attackerTroopStats[3][1] += troopCount * troopStats[3][i][1] * attacker->buffPercent[3][1]; // Defense
		attackerTroopStats[3][2] += troopCount * troopStats[3][i][2] * attacker->buffPercent[3][2]; // HP


	// Defender Stats
	for (unsigned int i = 1; i < 15; i++) {

		// MOUNTED ----------------------------------------------------------------------------------------------------------
		unsigned int troopCount = defender->troopCount[0][i];
		defenderTroopStats[0][0] += troopCount * troopStats[0][i][0] * defender->buffPercent[0][0]; // Attack
		defenderTroopStats[0][1] += troopCount * troopStats[0][i][1] * defender->buffPercent[0][1]; // Defense
		defenderTroopStats[0][2] += troopCount * troopStats[0][i][2] * defender->buffPercent[0][2]; // HP

		// GROUND -----------------------------------------------------------------------------------------------------------
		troopCount = defender->troopCount[1][i];
		defenderTroopStats[1][0] += troopCount * troopStats[1][i][0] * defender->buffPercent[1][0]; // Attack
		defenderTroopStats[1][1] += troopCount * troopStats[1][i][1] * defender->buffPercent[1][1]; // Defense
		defenderTroopStats[1][2] += troopCount * troopStats[1][i][2] * defender->buffPercent[1][2]; // HP

		// RANGED -----------------------------------------------------------------------------------------------------------
		troopCount = defender->troopCount[2][i];
		defenderTroopStats[2][0] += troopCount * troopStats[2][i][0] * defender->buffPercent[2][0]; // Attack
		defenderTroopStats[2][1] += troopCount * troopStats[2][i][1] * defender->buffPercent[2][1]; // Defense
		defenderTroopStats[2][2] += troopCount * troopStats[2][i][2] * defender->buffPercent[2][2]; // HP

		// SIEGE ------------------------------------------------------------------------------------------------------------
		troopCount = defender->troopCount[3][i];
		defenderTroopStats[3][0] += troopCount * troopStats[3][i][0] * defender->buffPercent[3][0]; // Attack
		defenderTroopStats[3][1] += troopCount * troopStats[3][i][1] * defender->buffPercent[3][1]; // Defense
		defenderTroopStats[3][2] += troopCount * troopStats[3][i][2] * defender->buffPercent[3][2]; // HP

	}

	// Battle Mechanic Assumptions:
	// Target Priority Based On Highest Speed
	// Attacker Attacks First
	// Troop Defense Doesnt Refresh
	// Troop Types Have Shared Stats
	// Tier Level Differences Provide No Hidden Stat Modifications

	// Rounds
	bool combatEnded = false;

	unsigned int mountedRange = 50;
	unsigned int groundRange = 50;
	unsigned int rangedRange = 500;
	unsigned int siegeRange = 1400;

	unsigned int attackerDistMounted = 600;
	unsigned int attackerDistGround = 350;
	unsigned int attackerDistRanged = 100;
	unsigned int attackerDistSiege = 75;

	unsigned int defenderDistMounted = 900;
	unsigned int defenderDistGround = 1150;
	unsigned int defenderDistRanged = 1400;
	unsigned int defenderDistSiege = 1425;

	unsigned int incomingDamage = 0;

	// Attackers Stats
	unsigned int attackerEndingRangedDefense;
	unsigned int attackerEndingRangedHP;
	unsigned int attackerEndingRangedAttack;
	unsigned int attackerEndingSiegeDefense;
	unsigned int attackerEndingSiegeHP;
	unsigned int attackerEndingSiegeAttack;
	unsigned int attackerEndingGroundDefense;
	unsigned int attackerEndingGroundHP;
	unsigned int attackerEndingGroundAttack;
	unsigned int attackerEndingMountedDefense;
	unsigned int attackerEndingMountedHP;
	unsigned int attackerEndingMountedAttack;

	// Defenders Stats
	unsigned int defenderEndingRangedDefense;
	unsigned int defenderEndingRangedHP;
	unsigned int defenderEndingRangedAttack;
	unsigned int defenderEndingSiegeDefense;
	unsigned int defenderEndingSiegeHP;
	unsigned int defenderEndingSiegeAttack;
	unsigned int defenderEndingGroundDefense;
	unsigned int defenderEndingGroundHP;
	unsigned int defenderEndingGroundAttack;
	unsigned int defenderEndingMountedDefense;
	unsigned int defenderEndingMountedHP;
	unsigned int defenderEndingMountedAttack;

	while (!combatEnded) {

		// Attackers Ending Stats -----------------------------------------------------------------------

		// Ranged
		attackerEndingRangedDefense = getDefense(ATTACKER, RANGED);
		attackerEndingRangedHP = getHP(ATTACKER, RANGED);
		attackerEndingRangedAttack = getAttack(ATTACKER, RANGED);

		// Siege
		attackerEndingSiegeDefense = getDefense(ATTACKER, SIEGE);
		attackerEndingSiegeHP = getHP(ATTACKER, SIEGE);
		attackerEndingSiegeAttack = getAttack(ATTACKER, SIEGE);

		// Ground
		attackerEndingGroundDefense = getDefense(ATTACKER, GROUND);
		attackerEndingGroundHP = getHP(ATTACKER, GROUND);
		attackerEndingGroundAttack = getAttack(ATTACKER, GROUND);

		// Mounted
		attackerEndingMountedDefense = getDefense(ATTACKER, MOUNTED);
		attackerEndingMountedHP = getHP(ATTACKER, MOUNTED);
		attackerEndingMountedAttack = getAttack(ATTACKER, MOUNTED);

		// Defenders Ending Stats -----------------------------------------------------------------------

		// Ranged
		defenderEndingRangedDefense = getDefense(DEFENDER, RANGED);
		defenderEndingRangedHP = getHP(DEFENDER, RANGED);
		defenderEndingAttack = getAttack(DEFENDER, RANGED);

		// Siege
		defenderEndingSiegeDefense = getDefense(DEFENDER, SIEGE);
		defenderEndingSiegeHP = getHP(DEFENDER, SIEGE);
		defenderEndingAttack = getAttack(DEFENDER, SIEGE);

		// Ground
		defenderEndingGroundDefense = getDefense(DEFENDER, GROUND);
		defenderEndingGroundHP = getHP(DEFENDER, GROUND);
		defenderEndingGroundAttack = getAttack(DEFENDER, GROUND);

		// Mounted
		defenderEndingMountedDefense = getDefense(DEFENDER, MOUNTED);
		defenderEndingMountedHP = getHP(DEFENDER, MOUNTED);
		defenderEndingAttack = getAttack(DEFENDER, MOUNTED);

		// Siege Attack ---------------------------------------------------------------------------------

		if (attackerCooldownSiege < 1) {
			incomingDamage = getAttack(ATTACKER, SIEGE);

			// If Defending Ranged Are In Range Of Attacking Siege
			if (attackerDistSiege < defenderDistRanged) {
				attackerCooldownSiege = 3;
				if (defenderEndingRangedDefense < incomingDamage) {
					incomingDamage -= defenderEndingRangedDefense;
					defenderEndingRangedDefense = 0;
				}
				else {

					defenderEndingRangedDefense -= incomingDamage;
					incomingDamage = 0;
				}

				if (defenderEndingRangedHP < incomingDamage) {
					incomingDamage -= defenderEndingRangedHP;
					defenderEndingRangedHP = 0;
					defenderDistRanged = -10000;
				}
				else {
					defenderEndingRangedHP -= incomingDamage;
					incomingDamage = 0;
				}
			}

			// If Defending Siege Are In Range Of Attacking Siege
			if (attackerDistSiege < defenderDistSiege) {
				attackerCooldownSiege = 3;
				if (defenderEndingSiegeDefense < incomingDamage) {
					incomingDamage -= defenderEndingSiegeDefense;
					defenderEndingSiegeDefense = 0;
				}
				else {

					defenderEndingSiegeDefense -= incomingDamage;
					incomingDamage = 0;
				}

				if (defenderEndingSiegeHP < incomingDamage) {
					incomingDamage -= defenderEndingSiegeHP;
					defenderEndingSiegeHP = 0;
					defenderDistSiege = -10000;
				}
				else {
					defenderEndingSiegeHP -= incomingDamage;
					incomingDamage = 0;
				}
			}

			// If Defending Mounted Are In Range Of Attacking Siege
			if (attackerDistSiege < defenderDistMounted) {
				attackerCooldownSiege = 3;
				if (defenderEndingMountedDefense < incomingDamage) {
					incomingDamage -= defenderEndingMountedDefense;
					defenderEndingMountedDefense = 0;
				}
				else {

					defenderEndingMountedDefense -= incomingDamage;
					incomingDamage = 0;
				}

				if (defenderEndingMountedHP < incomingDamage) {
					incomingDamage -= defenderEndingMountedHP;
					defenderEndingMountedHP = 0;
					defenderDistMounted = -10000;
				}
				else {
					defenderEndingMountedHP -= incomingDamage;
					incomingDamage = 0;
				}
			}

			// If Defending Ground Are In Range Of Attacking Siege
			if (attackerDistSiege < defenderDistGround) {
				attackerCooldownSiege = 3;
				if (defenderEndingGroundDefense < incomingDamage) {
					incomingDamage -= defenderEndingGroundDefense;
					defenderEndingGroundDefense = 0;
				}
				else {

					defenderEndingGroundDefense -= incomingDamage;
					incomingDamage = 0;
				}

				if (defenderEndingGroundHP < incomingDamage) {
					incomingDamage -= defenderEndingGroundHP;
					defenderEndingGroundHP = 0;
					defenderDistGround = -10000;
				}
				else {
					defenderEndingGroundHP -= incomingDamage;
					incomingDamage = 0;
				}
			}
		}

		// Ranged Attack ----------------------------------------------------------------------------------------------------

		if (attackerCooldownRanged < 1) {
			incomingDamage = getAttack(ATTACKER, RANGED);

			// If Defending Mounted Are In Range Of Attacking Ranged
			if (attackerDistRanged < defenderDistMounted && (defenderDistMounted - attackerDistRanged) <= 500) {
				attackerCooldownRanged = 4;
				if (defenderEndingMountedDefense < incomingDamage) {
					incomingDamage -= defenderEndingMountedDefense;
					defenderEndingMountedDefense = 0;
				}
				else {

					defenderEndingMountedDefense -= incomingDamage;
					incomingDamage = 0;
				}

				if (defenderEndingMountedHP < incomingDamage) {
					incomingDamage -= defenderEndingMountedHP;
					defenderEndingMountedHP = 0;
					defenderDistMounted = -10000;
				}
				else {
					defenderEndingMountedHP -= incomingDamage;
					incomingDamage = 0;
				}
			}

			// If Defending Ground Are In Range Of Attacking Ranged
			if (attackerDistRanged < defenderDistGround && (defenderDistGround - attackerDistRanged) <= 500) {
				attackerCooldownRanged = 4;
				if (defenderEndingGroundDefense < incomingDamage) {
					incomingDamage -= defenderEndingGroundDefense;
					defenderEndingGroundDefense = 0;
				}
				else {

					defenderEndingGroundDefense -= incomingDamage;
					incomingDamage = 0;
				}

				if (defenderEndingGroundHP < incomingDamage) {
					incomingDamage -= defenderEndingGroundHP;
					defenderEndingGroundHP = 0;
					defenderDistGround = -10000;
				}
				else {
					defenderEndingGroundHP -= incomingDamage;
					incomingDamage = 0;
				}
			}

			// If Defending Ranged Are In Range Of Attacking Ranged
			if (attackerDistRanged < defenderDistRanged && (defenderDistRanged - attackerDistRanged) <= 500) {
				attackerCooldownRanged = 4;
				if (defenderEndingRangedDefense < incomingDamage) {
					incomingDamage -= defenderEndingRangedDefense;
					defenderEndingRangedDefense = 0;
				}
				else {

					defenderEndingRangedDefense -= incomingDamage;
					incomingDamage = 0;
				}

				if (defenderEndingRangedHP < incomingDamage) {
					incomingDamage -= defenderEndingRangedHP;
					defenderEndingRangedHP = 0;
					defenderDistRanged = -10000;
				}
				else {
					defenderEndingRangedHP -= incomingDamage;
					incomingDamage = 0;
				}
			}

			// If Defending Siege Are In Range Of Attacking Ranged
			if (attackerDistRanged < defenderDistSiege && (defenderDistSiege - attackerDistRanged) <= 500) {
				attackerCooldownRanged = 4;
				if (defenderEndingSiegeDefense < incomingDamage) {
					incomingDamage -= defenderEndingSiegeDefense;
					defenderEndingSiegeDefense = 0;
				}
				else {

					defenderEndingSiegeDefense -= incomingDamage;
					incomingDamage = 0;
				}

				if (defenderEndingSiegeHP < incomingDamage) {
					incomingDamage -= defenderEndingSiegeHP;
					defenderEndingSiegeHP = 0;
					defenderDistSiege = -10000;
				}
				else {
					defenderEndingSiegeHP -= incomingDamage;
					incomingDamage = 0;
				}
			}
		}

		// Ground Attack ----------------------------------------------------------------------------------------------------

		if (attackerCooldownGround < 1) {
			incomingDamage = getAttack(ATTACKER, GROUND);

			// If Defending Mounted Are In Range Of Attacking Ground
			if (attackerDistGround < defenderDistMounted && (defenderDistMounted - attackerDistGround) <= 50) {
				attackerCooldownGround = 14;
				if (defenderEndingMountedDefense < incomingDamage) {
					incomingDamage -= defenderEndingMountedDefense;
					defenderEndingMountedDefense = 0;
				}
				else {

					defenderEndingMountedDefense -= incomingDamage;
					incomingDamage = 0;
				}

				if (defenderEndingMountedHP < incomingDamage) {
					incomingDamage -= defenderEndingMountedHP;
					defenderEndingMountedHP = 0;
					defenderDistMounted = -10000;
				}
				else {
					defenderEndingMountedHP -= incomingDamage;
					incomingDamage = 0;
				}
			}

			// If Defending Ground Are In Range Of Attacking Ground
			if (attackerDistGround < defenderDistGround && (defenderDistGround - attackerDistGround) <= 50) {
				attackerCooldownGround = 14;
				if (defenderEndingGroundDefense < incomingDamage) {
					incomingDamage -= defenderEndingGroundDefense;
					defenderEndingGroundDefense = 0;
				}
				else {

					defenderEndingGroundDefense -= incomingDamage;
					incomingDamage = 0;
				}

				if (defenderEndingGroundHP < incomingDamage) {
					incomingDamage -= defenderEndingGroundHP;
					defenderEndingGroundHP = 0;
					defenderDistGround = -10000; 
				}
				else {
					defenderEndingGroundHP -= incomingDamage;
					incomingDamage = 0;
				}
			}

			// If Defending Ranged Are In Range Of Attacking Ground
			if (attackerDistGround < defenderDistRanged && (defenderDistRanged - attackerDistGround) <= 50) {
				attackerCooldownGround = 14;
				if (defenderEndingRangedDefense < incomingDamage) {
					incomingDamage -= defenderEndingRangedDefense;
					defenderEndingRangedDefense = 0;
				}
				else {

					defenderEndingRangedDefense -= incomingDamage;
					incomingDamage = 0;
				}

				if (defenderEndingRangedHP < incomingDamage) {
					incomingDamage -= defenderEndingRangedHP;
					defenderEndingRangedHP = 0;
					defenderDistRanged = -10000;
				}
				else {
					defenderEndingRangedHP -= incomingDamage;
					incomingDamage = 0;
				}
			}

			// If Defending Siege Are In Range Of Attacking Ground
			if (attackerDistGround < defenderDistSiege && (defenderDistSiege - attackerDistGround) <= 50) {
				attackerCooldownGround = 14;
				if (defenderEndingSiegeDefense < incomingDamage) {
					incomingDamage -= defenderEndingSiegeDefense;
					defenderEndingSiegeDefense = 0;
				}
				else {

					defenderEndingSiegeDefense -= incomingDamage;
					incomingDamage = 0;
				}

				if (defenderEndingSiegeHP < incomingDamage) {
					incomingDamage -= defenderEndingSiegeHP;
					defenderEndingSiegeHP = 0;
					defenderDistSiege = -10000;
				}
				else {
					defenderEndingSiegeHP -= incomingDamage;
					incomingDamage = 0;
				}
			}
		}

		// Mounted Attack ---------------------------------------------------------------------------------------------------

		if (attackerCooldownMounted < 1) {
			incomingDamage = getAttack(ATTACKER, MOUNTED);

			// If Defending Mounted Are In Range Of Attacking Mounted
			if (attackerDistGround < defenderDistMounted && (defenderDistMounted - attackerDistMounted) <= 50) {
				attackerCooldownMounted = 24;
				if (defenderEndingMountedDefense < incomingDamage) {
					incomingDamage -= defenderEndingMountedDefense;
					defenderEndingMountedDefense = 0;
				}
				else {

					defenderEndingMountedDefense -= incomingDamage;
					incomingDamage = 0;
				}

				if (defenderEndingMountedHP < incomingDamage) {
					incomingDamage -= defenderEndingMountedHP;
					defenderEndingMountedHP = 0;
					defenderDistMounted = -10000;
				}
				else {
					defenderEndingMountedHP -= incomingDamage;
					incomingDamage = 0;
				}
			}

			// If Defending Ground Are In Range Of Attacking Mounted
			if (attackerDistGround < defenderDistGround && (defenderDistGround - attackerDistMounted) <= 50) {
				attackerCooldownMounted = 24;
				if (defenderEndingGroundDefense < incomingDamage) {
					incomingDamage -= defenderEndingGroundDefense;
					defenderEndingGroundDefense = 0;
				}
				else {

					defenderEndingGroundDefense -= incomingDamage;
					incomingDamage = 0;
				}

				if (defenderEndingGroundHP < incomingDamage) {
					incomingDamage -= defenderEndingGroundHP;
					defenderEndingGroundHP = 0;
				}
				else {
					defenderEndingGroundHP -= incomingDamage;
					incomingDamage = 0;
				}
			}

			// If Defending Ranged Are In Range Of Attacking Mounted
			if (attackerDistGround < defenderDistRanged && (defenderDistRanged - attackerDistMounted) <= 50) {
				attackerCooldownMounted = 24;
				if (defenderEndingRangedDefense < incomingDamage) {
					incomingDamage -= defenderEndingRangedDefense;
					defenderEndingRangedDefense = 0;
				}
				else {

					defenderEndingRangedDefense -= incomingDamage;
					incomingDamage = 0;
				}

				if (defenderEndingRangedHP < incomingDamage) {
					incomingDamage -= defenderEndingRangedHP;
					defenderEndingRangedHP = 0;
				}
				else {
					defenderEndingRangedHP -= incomingDamage;
					incomingDamage = 0;
				}
			}

			// If Defending Siege Are In Range Of Attacking Mounted
			if (attackerDistGround < defenderDistSiege && (defenderDistSiege - attackerDistMounted) <= 50) {
				attackerCooldownMounted = 24;
				if (defenderEndingSiegeDefense < incomingDamage) {
					incomingDamage -= defenderEndingSiegeDefense;
					defenderEndingSiegeDefense = 0;
				}
				else {

					defenderEndingSiegeDefense -= incomingDamage;
					incomingDamage = 0;
				}

				if (defenderEndingSiegeHP < incomingDamage) {
					incomingDamage -= defenderEndingSiegeHP;
					defenderEndingSiegeHP = 0;
				}
				else {
					defenderEndingSiegeHP -= incomingDamage;
					incomingDamage = 0;
				}
			}
		}

		// Defending Troops Return Fire -------------------------------------------------------------------------------

		// Siege Attack ---------------------------------------------------------------------------------

		if (defenderCooldownSiege < 1) {
			unsigned int incomingDamage = getAttack(DEFENDER, SIEGE);

			// If Attacking Ranged Are In Range Of Defending Siege
			if (defenderDistSiege > attackerDistRanged) {
				defenderCooldownSiege = 3;
				if (attackerEndingRangedDefense < incomingDamage) {
					incomingDamage -= attackerEndingRangedDefense;
					attackerEndingRangedDefense = 0;
				}
				else {

					attackerEndingRangedDefense -= incomingDamage;
					incomingDamage = 0;
				}

				if (attackerEndingRangedHP < incomingDamage) {
					incomingDamage -= attackerEndingRangedHP;
					attackerEndingRangedHP = 0;
				}
				else {
					attackerEndingRangedHP -= incomingDamage;
					incomingDamage = 0;
				}
			}

			// If Attacking Siege Are In Range Of Defending Siege
			if (defenderDistSiege > attackerDistSiege) {
				defenderCooldownSiege = 3;
				if (attackerEndingSiegeDefense < incomingDamage) {
					incomingDamage -= attackerEndingSiegeDefense;
					attackerEndingSiegeDefense = 0;
				}
				else {

					attackerEndingSiegeDefense -= incomingDamage;
					incomingDamage = 0;
				}

				if (attackerEndingSiegeHP < incomingDamage) {
					incomingDamage -= attackerEndingSiegeHP;
					attackerEndingSiegeHP = 0;
				}
				else {
					attackerEndingSiegeHP -= incomingDamage;
					incomingDamage = 0;
				}
			}

			// If Attacking Mounted Are In Range Of Defending Siege
			if (defenderDistSiege > attackerDistMounted) {
				defenderCooldownSiege = 3;
				if (attackerEndingMountedDefense < incomingDamage) {
					incomingDamage -= attackerEndingMountedDefense;
					attackerEndingMountedDefense = 0;
				}
				else {

					attackerEndingMountedDefense -= incomingDamage;
					incomingDamage = 0;
				}

				if (attackerEndingMountedHP < incomingDamage) {
					incomingDamage -= attackerEndingMountedHP;
					attackerEndingMountedHP = 0;
				}
				else {
					attackerEndingMountedHP -= incomingDamage;
					incomingDamage = 0;
				}
			}

			// If Attacking Ground Are In Range Of Defending Siege
			if (defenderDistSiege > attackerDistGround) {
				defenderCooldownSiege = 3;
				if (attackerEndingGroundDefense < incomingDamage) {
					incomingDamage -= attackerEndingGroundDefense;
					attackerEndingGroundDefense = 0;
				}
				else {

					attackerEndingGroundDefense -= incomingDamage;
					incomingDamage = 0;
				}

				if (attackerEndingGroundHP < incomingDamage) {
					incomingDamage -= attackerEndingGroundHP;
					attackerEndingGroundHP = 0;
				}
				else {
					attackerEndingGroundHP -= incomingDamage;
					incomingDamage = 0;
				}
			}
		}

		// Ranged Attack ----------------------------------------------------------------------------------------------------

		if (defenderCooldownRanged < 1) {
			incomingDamage = getAttack(DEFENDER, RANGED);

			// If Attacking Mounted Are In Range Of Defending Ranged
			if (defenderDistRanged > attackerDistMounted && (attackerDistMounted - defenderDistRanged) >= -500) {
				defenderCooldownRanged = 4;
				if (attackerEndingMountedDefense < incomingDamage) {
					incomingDamage -= attackerEndingMountedDefense;
					attackerEndingMountedDefense = 0;
				}
				else {

					attackerEndingMountedDefense -= incomingDamage;
					incomingDamage = 0;
				}

				if (attackerEndingMountedHP < incomingDamage) {
					incomingDamage -= attackerEndingMountedHP;
					attackerEndingMountedHP = 0;
				}
				else {
					attackerEndingMountedHP -= incomingDamage;
					incomingDamage = 0;
				}
			}

			// If Attacking Ground Are In Range Of Defending Ranged
			if (defenderDistRanged > attackerDistGround && (attackerDistGround - defenderDistRanged) >= -500) {
				defenderCooldownRanged = 4;
				if (attackerEndingGroundDefense < incomingDamage) {
					incomingDamage -= attackerEndingGroundDefense;
					attackerEndingGroundDefense = 0;
				}
				else {

					attackerEndingGroundDefense -= incomingDamage;
					incomingDamage = 0;
				}

				if (attackerEndingGroundHP < incomingDamage) {
					incomingDamage -= attackerEndingGroundHP;
					attackerEndingGroundHP = 0;
				}
				else {
					attackerEndingGroundHP -= incomingDamage;
					incomingDamage = 0;
				}
			}

			// If Attacking Ranged Are In Range Of Defending Ranged
			if (defenderDistRanged > attackerDistRanged && (attackerDistRanged - defenderDistRanged) >= -500) {
				defenderCooldownRanged = 4;
				if (attackerEndingRangedDefense < incomingDamage) {
					incomingDamage -= attackerEndingRangedDefense;
					attackerEndingRangedDefense = 0;
				}
				else {

					attackerEndingRangedDefense -= incomingDamage;
					incomingDamage = 0;
				}

				if (attackerEndingRangedHP < incomingDamage) {
					incomingDamage -= attackerEndingRangedHP;
					attackerEndingRangedHP = 0;
				}
				else {
					attackerEndingRangedHP -= incomingDamage;
					incomingDamage = 0;
				}
			}

			// If Attacking Siege Are In Range Of Defending Ranged
			if (defenderDistRanged > attackerDistSiege && (attackerDistSiege - defenderDistRanged) >= -500) {
				defenderCooldownRanged = 4;
				if (attackerEndingSiegeDefense < incomingDamage) {
					incomingDamage -= attackerEndingSiegeDefense;
					attackerEndingSiegeDefense = 0;
				}
				else {

					attackerEndingSiegeDefense -= incomingDamage;
					incomingDamage = 0;
				}

				if (attackerEndingSiegeHP < incomingDamage) {
					incomingDamage -= attackerEndingSiegeHP;
					attackerEndingSiegeHP = 0;
				}
				else {
					attackerEndingSiegeHP -= incomingDamage;
					incomingDamage = 0;
				}
			}
		}

		// Ground Attack ----------------------------------------------------------------------------------------------------
		if (defenderCooldownGround < 1) {
			incomingDamage = getAttack(DEFENDER, GROUND);

			// If Attacking Mounted Are In Range Of Defending Ground
			if (defenderDistGround > attackerDistMounted && (attackerDistMounted - defenderDistGround) >= -50) {
				defenderCooldownGround = 14;
				if (attackerEndingMountedDefense < incomingDamage) {
					incomingDamage -= attackerEndingMountedDefense;
					attackerEndingMountedDefense = 0;
				}
				else {

					attackerEndingMountedDefense -= incomingDamage;
					incomingDamage = 0;
				}

				if (attackerEndingMountedHP < incomingDamage) {
					incomingDamage -= attackerEndingMountedHP;
					attackerEndingMountedHP = 0;
				}
				else {
					attackerEndingMountedHP -= incomingDamage;
					incomingDamage = 0;
				}
			}

			// If Attacking Ground Are In Range Of Defending Ground
			if (defenderDistGround > attackerDistGround && (attackerDistGround - defenderDistGround) >= -50) {
				defenderCooldownGround = 14;
				if (attackerEndingGroundDefense < incomingDamage) {
					incomingDamage -= attackerEndingGroundDefense;
					attackerEndingGroundDefense = 0;
				}
				else {

					attackerEndingGroundDefense -= incomingDamage;
					incomingDamage = 0;
				}

				if (attackerEndingGroundHP < incomingDamage) {
					incomingDamage -= attackerEndingGroundHP;
					attackerEndingGroundHP = 0;
				}
				else {
					attackerEndingGroundHP -= incomingDamage;
					incomingDamage = 0;
				}
			}

			// If Attacking Ranged Are In Range Of Defending Ground
			if (defenderDistGround > attackerDistRanged && (attackerDistRanged - defenderDistGround) >= -50) {
				defenderCooldownGround = 14;
				if (attackerEndingRangedDefense < incomingDamage) {
					incomingDamage -= attackerEndingRangedDefense;
					attackerEndingRangedDefense = 0;
				}
				else {

					attackerEndingRangedDefense -= incomingDamage;
					incomingDamage = 0;
				}

				if (attackerEndingRangedHP < incomingDamage) {
					incomingDamage -= attackerEndingRangedHP;
					attackerEndingRangedHP = 0;
				}
				else {
					attackerEndingRangedHP -= incomingDamage;
					incomingDamage = 0;
				}
			}

			// If Attacking Siege Are In Range Of Defending Ground
			if (defenderDistGround > attackerDistSiege && (attackerDistSiege - defenderDistGround) >= -50) {
				defenderCooldownGround = 14;
				if (attackerEndingSiegeDefense < incomingDamage) {
					incomingDamage -= attackerEndingSiegeDefense;
					attackerEndingSiegeDefense = 0;
				}
				else {

					attackerEndingSiegeDefense -= incomingDamage;
					incomingDamage = 0;
				}

				if (attackerEndingSiegeHP < incomingDamage) {
					incomingDamage -= attackerEndingSiegeHP;
					attackerEndingSiegeHP = 0;
				}
				else {
					attackerEndingSiegeHP -= incomingDamage;
					incomingDamage = 0;
				}
			}
		}

		// Mounted Attack ---------------------------------------------------------------------------------------------------

		if (defenderCooldownMounted < 1) {
			incomingDamage = getAttack(DEFENDER, MOUNTED);

			// If Attacking Mounted Are In Range Of Defending Mounted
			if (defenderDistGround > attackerDistMounted && (attackerDistMounted - defenderDistMounted) >= -50) {
				defenderCooldownMounted = 24;
				if (attackerEndingMountedDefense < incomingDamage) {
					incomingDamage -= attackerEndingMountedDefense;
					attackerEndingMountedDefense = 0;
				}
				else {

					attackerEndingMountedDefense -= incomingDamage;
					incomingDamage = 0;
				}

				if (attackerEndingMountedHP < incomingDamage) {
					incomingDamage -= attackerEndingMountedHP;
					attackerEndingMountedHP = 0;
				}
				else {
					attackerEndingMountedHP -= incomingDamage;
					incomingDamage = 0;
				}
			}

			// If Attacking Ground Are In Range Of Defending Mounted
			if (defenderDistGround > attackerDistGround && (attackerDistGround - defenderDistMounted) >= -50) {
				defenderCooldownMounted = 24;
				if (attackerEndingGroundDefense < incomingDamage) {
					incomingDamage -= attackerEndingGroundDefense;
					attackerEndingGroundDefense = 0;
				}
				else {

					attackerEndingGroundDefense -= incomingDamage;
					incomingDamage = 0;
				}

				if (attackerEndingGroundHP < incomingDamage) {
					incomingDamage -= attackerEndingGroundHP;
					attackerEndingGroundHP = 0;
				}
				else {
					attackerEndingGroundHP -= incomingDamage;
					incomingDamage = 0;
				}
			}

			// If Attacking Ranged Are In Range Of Defending Mounted
			if (defenderDistGround > attackerDistRanged && (attackerDistRanged - defenderDistMounted) >= -50) {
				defenderCooldownMounted = 24;
				if (attackerEndingRangedDefense < incomingDamage) {
					incomingDamage -= attackerEndingRangedDefense;
					attackerEndingRangedDefense = 0;
				}
				else {

					attackerEndingRangedDefense -= incomingDamage;
					incomingDamage = 0;
				}

				if (attackerEndingRangedHP < incomingDamage) {
					incomingDamage -= attackerEndingRangedHP;
					attackerEndingRangedHP = 0;
				}
				else {
					attackerEndingRangedHP -= incomingDamage;
					incomingDamage = 0;
				}
			}

			// If Attacking Siege Are In Range Of Defending Mounted
			if (defenderDistGround > attackerDistSiege && (attackerDistSiege - defenderDistMounted) >= -50) {
				defenderCooldownMounted = 24;
				if (attackerEndingSiegeDefense < incomingDamage) {
					incomingDamage -= attackerEndingSiegeDefense;
					attackerEndingSiegeDefense = 0;
				}
				else {

					attackerEndingSiegeDefense -= incomingDamage;
					incomingDamage = 0;
				}

				if (attackerEndingSiegeHP < incomingDamage) {
					incomingDamage -= attackerEndingSiegeHP;
					attackerEndingSiegeHP = 0;
				}
				else {
					attackerEndingSiegeHP -= incomingDamage;
					incomingDamage = 0;
				}
			}
		}

		// Troops Move

		// Attacker
		attackerDistGround += 14;
		attackerDistMounted += 24;
		attackerDistRanged += 4;
		attackerDistSiege += 3;

		// Defender
		defenderDistGround += 14;
		defenderDistMounted += 24;
		defenderDistRanged += 4;
		defenderDistSiege += 3;

		// Cooldowns

		// Attacker
		attackerCooldownGround -= 14 / 25;
		attackerCooldownMounted -= 24 / 25;
		attackerCooldownRanged -= 4 / 25;
		attackerCooldownSiege -= 3 / 25;

		// Defender
		defenderCooldownGround -= 14 / 25;
		defenderCooldownMounted -= 24 / 25;
		defenderCooldownRanged -= 4 / 25;
		defenderCooldownSiege -= 3 / 25;

	
	}
}