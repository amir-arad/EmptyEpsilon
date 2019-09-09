#include "targetsContainer.h"
#include "playerInfo.h"
#include "spaceObjects/playerSpaceship.h"

TargetsContainer::TargetsContainer()
{
    waypoint_selection_index = -1;
    allow_waypoint_selection = false;
}

void TargetsContainer::clear()
{
    waypoint_selection_index = -1;
    entries.clear();
}

void TargetsContainer::add(P<SpaceObject> obj)
{
    if (obj && !entries.has(obj))
        entries.push_back(obj);
}

void TargetsContainer::set(P<SpaceObject> obj)
{
    if (obj)
    {
        if (entries.size() > 0)
        {
            entries[0] = obj;
            if (entries.size() > 1)
                entries.resize(1);
        }else{
            entries.push_back(obj);
        }
    }
    else
    {
        clear();
    }
    waypoint_selection_index = -1;
}

void TargetsContainer::set(PVector<SpaceObject> objs)
{
    waypoint_selection_index = -1;
    entries = objs;
}

void TargetsContainer::setToClosestTo(sf::Vector2f position, float max_range, ESelectionType selection_type)
{
    P<SpaceObject> target;
    PVector<Collisionable> list = CollisionManager::queryArea(position - sf::Vector2f(max_range, max_range), position + sf::Vector2f(max_range, max_range));
    foreach(Collisionable, obj, list)
    {
        P<SpaceObject> spaceObject = obj;
        if (spaceObject && spaceObject != my_spaceship)
        {
            switch(selection_type)
            {
            case Selectable:
                if (!spaceObject->canBeSelectedBy(my_spaceship))
                    continue;
                break;
            case Targetable:
                if (!spaceObject->canBeTargetedBy(my_spaceship))
                    continue;
                break;
            }
            if (!target || sf::length(position - spaceObject->getPosition()) < sf::length(position - target->getPosition()))
                target = spaceObject;
        }
    }
    
    
    if (my_spaceship && allow_waypoint_selection)
    {
        for(int n=0; n<my_spaceship->getWaypointCount(); n++)
        {
            if ((my_spaceship->waypoints[n] - position) < max_range)
            {
                if (!target || sf::length(position - my_spaceship->waypoints[n]) < sf::length(position - target->getPosition()))
                {
                    clear();
                    waypoint_selection_index = n;
                    waypoint_selection_position = my_spaceship->waypoints[n];
                    return;
                }
            }
        }
    }
    set(target);
}

void TargetsContainer::nextWaypoint(bool forward){
    if (my_spaceship && allow_waypoint_selection) {
        entries.clear();
        int current = getWaypointIndex();
        int next;
        if (current == -1){
            next = forward? 0: my_spaceship->waypoints.size() - 1;
        } else {
            next = (current + my_spaceship->waypoints.size() + (forward ? 1 : -1)) % my_spaceship->waypoints.size();
        }
        setWaypointIndex(next);
    }
}

void TargetsContainer::next(PVector<SpaceObject> potentials, bool forward){
    P<SpaceObject> found = nullptr;
    bool current_reached = false;
    P<SpaceObject> lastSeen = nullptr; // for reverse logic edge case
    P<SpaceObject> firstSeen = nullptr; // for forward logic edge case
    foreach(SpaceObject, obj, potentials) {
        if (found) // no need to iterate more
            break;
        if (!obj) // should never happen but helps debug confidence
            continue;
        if(!firstSeen)
            firstSeen = obj;
        if (obj == get()) { // reached current target
            current_reached = true;
            if (!forward && lastSeen){
                found = lastSeen;
            }
        } else if (forward && current_reached) { // 1 after current
            found = obj;
        } 
        lastSeen = obj;
    } // end of loop
    /*
    LOG(INFO) << "size : " << string(potentials.size(),0);
    LOG(INFO) << "found : " << (found ? found->getCallSign() : "NULL");
    LOG(INFO) << "firstSeen : " << (firstSeen ? firstSeen->getCallSign() : "NULL");
    LOG(INFO) << "lastSeen : " << (lastSeen ? lastSeen->getCallSign() : "NULL");
    */
    if (!found){
        // current target might be the first or last element
        found = forward? firstSeen : lastSeen;
    } 
    set(found);
}

int TargetsContainer::getWaypointIndex()
{
    if (!my_spaceship || waypoint_selection_index < 0)
        waypoint_selection_index = -1;
    else if (waypoint_selection_index >= my_spaceship->getWaypointCount())
        waypoint_selection_index = -1;
    else if (my_spaceship->waypoints[waypoint_selection_index] != waypoint_selection_position)
        waypoint_selection_index = -1;
    return waypoint_selection_index;
}

void TargetsContainer::setWaypointIndex(int index)
{
    waypoint_selection_index = index;
    if (my_spaceship && index >= 0 && index < (int)my_spaceship->waypoints.size())
        waypoint_selection_position = my_spaceship->waypoints[index];
}

sf::Vector2f TargetsContainer::getWaypointPosition()
{
    if (my_spaceship && waypoint_selection_index >= 0)
        return waypoint_selection_position;
    else 
        return sf::Vector2f();
}
