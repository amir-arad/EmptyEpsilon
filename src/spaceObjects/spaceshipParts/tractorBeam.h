#ifndef TRACTOR_BEAM_H
#define TRACTOR_BEAM_H

#include "SFML/System/NonCopyable.hpp"
#include "stringImproved.h"
#include "spaceObjects/spaceObject.h"
class SpaceShip;

class TractorBeam : public sf::NonCopyable
{
protected:

    //Beam configuration
    float max_area; // Value greater than or equal to 0
    float drag_per_second; // Value greater than 0
    SpaceShip* parent; //The ship that this beam weapon is attached to.

    // Beam state
    float arc; 
    float direction;
    float range;
public:
    constexpr static float energy_per_target_u = 0.05f; /*< Amount of energy it takes to drag a target (of radius 100) for 1U */

    TractorBeam();

    void setParent(SpaceShip* parent);

    float getMaxArea();
    void setMaxArea(float max_area);

    void setDragPerSecond(float drag_per_second);
    float getDragPerSecond();
    
    void setArc(float arc);
    float getArc();

    void setDirection(float direction);
    float getDirection();

    void setRange(float range);
    float getRange();

    void setPosition(sf::Vector3f position);
    sf::Vector3f getPosition();
        
    float getDragSpeed();
    float getMaxArc(float range);
    float getMaxRange(float arc);
    void update(float delta);
};

#endif//TRACTOR_BEAM_H
