#include "tractorBeam.h"
#include "spaceObjects/spaceship.h"
#include "spaceObjects/playerSpaceship.h"
#include "spaceObjects/beamEffect.h"
#include "spaceObjects/spaceObject.h"

TractorBeam::TractorBeam() : max_area(0), drag_per_second(0), parent(nullptr), arc(0), direction(0), range(0) {}
// TractorBeam::TractorBeam() {
//     parent = nullptr;
//     max_area = 0;
//     drag_per_second = 0;
//     arc = 0;
//     direction = 0;
//     range = 0;
// }

void TractorBeam::setParent(SpaceShip* parent)
{
    assert(!this->parent);
    this->parent = parent;

    // parent->registerMemberReplication(&max_area);
    // parent->registerMemberReplication(&drag_per_second);
    parent->registerMemberReplication(&arc);
    parent->registerMemberReplication(&direction);
    parent->registerMemberReplication(&range);
}

void TractorBeam::setMaxArea(float max_area)
{
    this->max_area = max_area;
}

float TractorBeam::getMaxArea()
{
    return max_area;
}

void TractorBeam::setDragPerSecond(float drag_per_second)
{
    this->drag_per_second = drag_per_second;
}

float TractorBeam::getDragPerSecond()
{
    return drag_per_second;
}

float TractorBeam::getMaxRange(float arc)
{
    // M_PI * range * range * arc / 360 <= max_area
    return sqrtf((max_area * 360) / (M_PI * std::max(1.0f, arc)));
}

void TractorBeam::setArc(float arc)
{
    this->arc = arc;
}

float TractorBeam::getArc()
{
    return arc;
}

void TractorBeam::setDirection(float direction)
{
     while(direction < 0)
        direction += 360;
    while(direction > 360)
        direction -= 360;
    this->direction = direction;
}

float TractorBeam::getDirection()
{
    return direction;
}


float TractorBeam::getMaxArc(float range)
{
    // M_PI * range * range * arc / 360 <= max_area
    return (max_area * 360) / (M_PI * std::max(1.0f, range * range));
}
void TractorBeam::setRange(float range)
{
    this->range = range;
}

float TractorBeam::getRange()
{
    return range;
}

float TractorBeam::getDragSpeed()
{
    return getDragPerSecond() * parent->getSystemEffectiveness(SYS_Docks);
}

void TractorBeam::update(float delta)
{
    if (game_server && range > 0.0 && delta > 0)
    {
        float dragCapability = delta * getDragSpeed();
        foreach(SpaceObject, target, space_object_list)
        {
            if (target != parent) {
                // Get the angle to the target.
                sf::Vector2f diff = target->getPosition() - parent->getPosition();
                float target_distance = sf::length(diff) - (parent->getRadius() + target->getRadius()) / 2.0;
                float angle = sf::vector2ToAngle(diff);
                float angle_diff = sf::angleDifference(direction + parent->getRotation(), angle);

                // If the target is in the beam's arc and range and the beam can consume enough energy
                if (target_distance < range && fabsf(angle_diff) < arc / 2.0)
                {
                    float distanceToDrag = std::min(fabsf(target_distance - (range/2)), dragCapability);
                    if (parent->useEnergy(energy_per_target_u * distanceToDrag))
                    {
                        P<PlayerSpaceship> target_ship = target;
                        if (target_distance < dragCapability && target_ship)
                        {
                            // if tractor beam is dragging a ship into parent, force docking
                            target_ship->requestDock(parent);
                        }
                        distanceToDrag *= (100 / target->getRadius());
                        target->setPosition(target->getPosition() - (distanceToDrag * normalize(diff)));
                    }
                }
            }
        }
    }
}

