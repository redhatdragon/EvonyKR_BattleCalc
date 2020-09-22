#include <stdint.h>
#include <memory.h>
#include <stdlib.h>  //TODO: might be included in iostream must check for that later.
#include <iostream>

constexpr uint32_t MAX_ROUNDS = 10;
constexpr uint32_t MAX_TROOP_TYPES = 4;
constexpr uint32_t MAX_FORT_TYPES = 4;

enum class TROOP_TYPE {
	GROUND,
	RANGED,
	CAV,
	MECH
};
enum class FORT_TYPE {
	TRAP,
	ABBIT,
	ARROW,
	TREB
};
enum class TEAM {
	DEFENDER,
	ATTACKER
};

//Devoid of all notions of tiers or numbers, meant purely to be used in battle calculations.
//This is the combined totals of all tiers of troops of one type (after buffs).
//Speed and range are not totaled, rhealth and attack however are.
struct Troop {
	uint64_t health, attack;
	uint16_t, speed, range;  //Needed as these may be modified by walls and other odd variables.
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
	__forceinline Troop& getTroop(TROOP_TYPE type, TEAM team) {
		return troops[type][team];
	}
};

int main() {
	Battlefield battlefield = {};
	//Grab data from battlefield.txt...  (Or stick to hard coded data)

	//Run calculation...

	//Print resultsl...
	return 0;
}