#include <SFML/OpenGL.hpp>
#include "scanProbe.h"
#include "explosionEffect.h"
#include "main.h"
#include "gameGlobalInfo.h"

#include "scriptInterface.h"
REGISTER_SCRIPT_SUBCLASS(ScanProbe, SpaceObject)
{    
    /// set owner for this probe, by callsign
    REGISTER_SCRIPT_CLASS_FUNCTION(ScanProbe, setOwner);
    /// set target position for this probe.
    REGISTER_SCRIPT_CLASS_FUNCTION(ScanProbe, setTarget);
    /// set time remaining for for this probe.
    REGISTER_SCRIPT_CLASS_FUNCTION(ScanProbe, setLifetime);
}

REGISTER_MULTIPLAYER_CLASS(ScanProbe, "ScanProbe");
ScanProbe::ScanProbe()
: SpaceObject(100, "ScanProbe")
{
    lifetime = 60 * 10;

    registerMemberReplication(&owner_id);
    registerMemberReplication(&target_position);
    registerMemberReplication(&lifetime, 60.0);
    setRadarSignatureInfo(0.0, 0.2, 0);
    
    switch(irandom(1, 3))
    {
    case 1:
        model_info.setData("SensorBuoyMKI");
        break;
    case 2:
        model_info.setData("SensorBuoyMKII");
        break;
    default:
        model_info.setData("SensorBuoyMKIII");
        break;
    }

    setCallSign(string(getMultiplayerId()) + "P");
}

void ScanProbe::update(float delta)
{
    lifetime -= delta;
    if (lifetime <= 0.0)
        destroy();
    if ((target_position - getPosition()) > getRadius())
    {
        sf::Vector2f v = normalize(target_position - getPosition());
        setPosition(getPosition() + v * delta * probe_speed);
    }
}

bool ScanProbe::canBeTargetedBy(P<SpaceObject> other)
{
    // This (slightly odd) logic is in place 
    // to prevent AI ships from actively seeking out probes and destroying them. 
    // But only seek and destroy probes when they are near the ships.
    // see https://github.com/daid/EmptyEpsilon/pull/485#discussion_r154773755
    return (getTarget() - getPosition()) < getRadius();
}

void ScanProbe::takeDamage(float damage_amount, DamageInfo info)
{
    destroy();
}

void ScanProbe::drawOnRadar(sf::RenderTarget& window, sf::Vector2f position, float scale, bool long_range)
{
    sf::Sprite object_sprite;
    textureManager.setTexture(object_sprite, "ProbeBlip.png");
    object_sprite.setPosition(position);
    object_sprite.setColor(sf::Color(96, 192, 128));
    float size = 0.3;
    object_sprite.setScale(size, size);
    window.draw(object_sprite);
}

void ScanProbe::setOwner(P<SpaceObject> owner)
{
    if (!owner) return;

    setFactionId(owner->getFactionId());
    owner_id = owner->getMultiplayerId();
}

string ScanProbe::getExportLine() {
    string ret = "ScanProbe()";
    if (owner_id){
        P<SpaceObject> owner = getObjectById(owner_id);
        if (owner){
            ret +=  ":setOwner(getObjectByCallSign(\"" + owner->getCallSign() + "\"))";
        }
    }
    ret += ":setPosition(" + string(getPosition().x, 0) + ", " + string(getPosition().y, 0) + ")";
    ret += ":setTarget(" + string(target_position.x, 0) + ", " + string(target_position.y, 0) + ")";
    ret += ":setLifetime(" + string(lifetime, 0) + ")";
    return ret;
}