#ifndef MISSILE_WEAPON_DATA_H
#define MISSILE_WEAPON_DATA_H

#include "engine.h"

enum EMissileWeapons
{
    MW_None = -1,
    MW_Cruise = 0,
    MW_Torpedo,
    MW_Heavy,
    MW_EMP,
    MW_Nuke,
    MW_HVLI,
    MW_Mine,
    MW_Count
};
/* Define script conversion function for the EMissileWeapons enum. */
template<> void convert<EMissileWeapons>::param(lua_State* L, int& idx, EMissileWeapons& es);
template<> int convert<EMissileWeapons>::returnType(lua_State* L, EMissileWeapons es);

/* data container for missile weapon data, contains information about different missile weapon types. */
class MissileWeaponData
{
public:
    MissileWeaponData(float speed, float turnrate, float lifetime, sf::Color color, float homing_range, string fire_sound);
    
    float speed; //meter/sec
    float turnrate; //deg/sec

    float lifetime; //sec
    sf::Color color;
    float homing_range;
    
    string fire_sound;

    static const MissileWeaponData& getDataFor(EMissileWeapons type);
};

#ifdef _MSC_VER
// MFC: GCC does proper external template instantiation, VC++ doesn't.
#include "missileWeaponData.hpp"
#endif

#endif//MISSILE_WEAPON_DATA_H
