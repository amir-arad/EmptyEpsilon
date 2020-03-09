#include "shipCargo.h"
#include "spaceObjects/spaceship.h"
#include "spaceObjects/playerSpaceship.h"
#include "gameGlobalInfo.h"
#include "tween.h"

REGISTER_MULTIPLAYER_CLASS(ShipCargo, "ShipCargo");

ShipCargo::ShipCargo() : Cargo("ShipCargo")
{
    registerMemberReplication(&callsign);
    registerMemberReplication(&template_name);
    registerMemberReplication(&hull_strength);
    for(int n=0; n<SYS_COUNT; n++) {
        registerMemberReplication(&systems_health[n]);
    }
}

ShipCargo::ShipCargo(P<ShipTemplate> ship_template) : ShipCargo()
{
    template_name = ship_template->getName();
    callsign = "DRN-" + gameGlobalInfo->getNextShipCallsign();
    setEnergy(ship_template->energy_storage_amount);
    hull_strength = ship_template->hull;
    for(int n=0; n<SYS_COUNT; n++) {
        systems_health[n] = 1;
    }
}

ShipCargo::ShipCargo(P<SpaceShip> ship) : ShipCargo()
{
    float totalHeat = 0;
    for (unsigned int n = 0; n < SYS_COUNT; n++)
        totalHeat += ship->getSystemHeat(ESystem(n));
    setHeat(totalHeat);
    template_name = ship->getTypeName();
    callsign = ship->getCallSign();
    setEnergy(ship->getEnergy());
    hull_strength = ship->getHull();
    for(int n=0; n<SYS_COUNT; n++) {
        systems_health[n] = ship->systems[n].health;
    }
}

P<ModelData> ShipCargo::getModel()
{
    P<ShipTemplate> ship_template = ShipTemplate::getTemplate(template_name);
    if (ship_template)
    {
        return ship_template->model_data;
    }
    else
        return nullptr;
}

float ShipCargo::getHealth()
{
    const float maxHull = getTemplate()->hull;
    float health = hull_strength;
    for(int n=0; n<SYS_COUNT; n++) {
        health += Tween<float>::linear(systems_health[n], -1, 1, 0, maxHull);
    }
    return health;
}

void ShipCargo::addHealth(float amount)
{
    const float maxHull = getTemplate()->hull;
    const float normAmount = amount / getMaxHealth();
    for(int n=0; n<SYS_COUNT; n++) {
        systems_health[n] = std::min(1.0f, systems_health[n] + (2 * normAmount));
    }
    hull_strength = std::min(maxHull, hull_strength + (maxHull * normAmount));
}

bool ShipCargo::onLaunch(Dock &source)
{
    if (game_server)
    {
        P<PlayerSpaceship> ship = new PlayerSpaceship();
        if (ship)
        {
            ship->setFactionId(source.getFactionId());
            ship->setTemplate(template_name);
            ship->setCallSign(callsign);
            ship->setEnergyLevel(getEnergy());
            ship->setPosition(source.getLaunchPosition(ship->getRadius()));
            ship->setRotation(source.getLaunchRotation());
            ship->setHull(hull_strength);
            ship->impulse_request = -0.1;
            int systemsCount = 0;
            for (unsigned int n = 0; n < SYS_COUNT; n++){
                if (ship->hasSystem(ESystem(n)))
                    systemsCount++;
                ship->systems[n].health = systems_health[n];
            }
            for (unsigned int n = 0; n < SYS_COUNT; n++)
                if (ship->hasSystem(ESystem(n)))
                    ship->addHeat(ESystem(n), getHeat() / systemsCount);
            return true;
        }
    }
    return false;
}

Cargo::Entries ShipCargo::getEntries()
{
    Cargo::Entries result = Cargo::getEntries();
    P<ShipTemplate> ship_template = ShipTemplate::getTemplate(template_name);
    if (ship_template)
    {
        result.push_back(std::make_tuple("gui/icons/hull", "Hull", string(int(100 * hull_strength / ship_template->hull)) + "%"));
    }
    result.push_back(std::make_tuple("", "callsign", callsign));
    result.push_back(std::make_tuple("", "type", template_name));
    return result;
}
