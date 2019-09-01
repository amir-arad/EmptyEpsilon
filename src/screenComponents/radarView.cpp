#include <SFML/OpenGL.hpp>

#include "main.h"
#include "gameGlobalInfo.h"
#include "spaceObjects/nebula.h"
#include "spaceObjects/blackHole.h"
#include "spaceObjects/scanProbe.h"
#include "playerInfo.h"
#include "radarView.h"
#include "missileTubeControls.h"
#include "targetsContainer.h"

sf::Color neutralMaskColor(255, 255, 255, 0);
sf::Color blockedMaskColor(0, 0, 0, 255);
sf::Color visibleMaskColor(255, 255, 255, 255);

GuiRadarView::GuiRadarView(GuiContainer* owner, string id, float distance, TargetsContainer* targets, P<SpaceShip> targetSpaceship)
: SectorsView(owner, id, distance, targets), next_ghost_dot_update(0.0), missile_tube_controls(nullptr), target_spaceship(targetSpaceship), long_range(false), show_ghost_dots(false)
, show_waypoints(false), show_target_projection(false), show_missile_tubes(false), show_callsigns(false), show_heading_indicators(false), show_game_master_data(false)
, range_indicator_step_size(0.0f), style(Circular), fog_style(NoFogOfWar), mouse_down_func(nullptr), mouse_drag_func(nullptr), mouse_up_func(nullptr)

{
    auto_center_on_my_ship = true;
    auto_orient = false;
    show_sectors = true;
}

void GuiRadarView::rotateTexture(sf::RenderTexture& texture, float rotation){
    sf::View view(rect);
    view.setViewport(texture.getView().getViewport());
    view.setRotation(rotation);
    texture.setView(view);
}

void GuiRadarView::orient(){
    if (target_spaceship && auto_orient){
        rotateTexture(background_texture, target_spaceship->getRotation() + 90);
        rotateTexture(forground_texture, target_spaceship->getRotation() + 90);
    }
}

sf::Vector2f GuiRadarView::orientVector(sf::Vector2f input){
    if (target_spaceship && auto_orient){
        return getCenterPosition() + sf::rotateVector(input - getCenterPosition(), -target_spaceship->getRotation() -90);
    } else {
        return input;
    }
}

void GuiRadarView::deOrient(){
    if (target_spaceship && auto_orient){
        rotateTexture(background_texture, 0);
        rotateTexture(forground_texture, 0);
    }
}

void GuiRadarView::onDraw(sf::RenderTarget& window)
{
    //We need 3 textures:
    // * background
    // * forground
    // * mask
    // Depending on what type of radar we are rendering we can use the mask to mask out the forground and/or background textures before rendering them
    // to the screen.

    //New rendering method. Render to texture first, so we do not need the stencil buffer, as this causes issues with the post processing effects.
    // Render background to screen
    // Render sectors to screen
    // Render range indicators to screen
    // Clear texture with 0% alpha
    // Render objects to texture
    // Render fog to texture with 0% alpha
    //      make fog result transparent, clearing anything that is in the fog.
    //      We can use different blendmodes to get the effect we want, as we can mask out alphas with that.
    // Render objects that are not effected by fog to texture
    // Render texture to screen

    //Hacky, when not relay and we have a ship, center on it.
    if (target_spaceship && auto_center_on_my_ship)
        setViewPosition(target_spaceship->getPosition());

    //Setup our textures for rendering
    adjustRenderTexture(background_texture);
    adjustRenderTexture(forground_texture);
    adjustRenderTexture(mask_texture);
    adjustRenderTexture(temp);
    
    orient();
    ///Draw the mask texture, which will be black vs white for masking.
    // White areas will be visible, black areas will be masked away.
    if (fog_style == RadarRangeAndLineOfSight){
        mask_texture.clear(blockedMaskColor);
        // not yet ready :(
        // drawFogOfWarBlockedAreas(mask_texture);
    }

    ///Draw the background texture
    drawBackground(background_texture);
    if (show_game_master_data)
        drawTerrain(background_texture);
    if (fog_style == RadarRangeAndLineOfSight)   
        drawRenderTexture(mask_texture, background_texture, sf::Color::White, sf::BlendMultiply);
    if (show_sectors)
        drawSectorGrid(background_texture);
    drawRangeIndicators(background_texture);
    if (show_target_projection)
        drawTargetProjections(background_texture);
    if (show_missile_tubes)
        drawMissileTubes(background_texture);

    ///Start drawing of foreground
    forground_texture.clear(sf::Color::Transparent);
    //Draw things that are masked out by fog-of-war
    if (show_ghost_dots)
    {
        updateGhostDots();
        drawGhostDots(forground_texture);
    }
    drawObjects(forground_texture, background_texture);
    if (show_game_master_data)
        drawObjectsGM(forground_texture);

    //Draw the mask on the drawn objects
    if (fog_style == RadarRangeAndLineOfSight)
    {
        drawRenderTexture(mask_texture, forground_texture, sf::Color::White, sf::BlendMode(
            sf::BlendMode::Zero, sf::BlendMode::SrcAlpha, sf::BlendMode::Add
        ));
    }
    //Post masking
    if (show_waypoints)
        drawWaypoints(forground_texture);
    if (show_heading_indicators)
        drawHeadingIndicators(forground_texture);
    drawTargets(forground_texture);

    if (style == Rectangular)
    {
        if (my_spaceship){
            // TODO: extract isInView? use rect.contains?
            sf::Vector2f ship_offset = (my_spaceship->getPosition() - getViewPosition()) / getDistance() * getRadius();
            if (ship_offset.x < -rect.width / 2.0f || ship_offset.x > rect.width / 2.0f || ship_offset.y < -rect.height / 2.0f || ship_offset.y > rect.height / 2.0f)
            {
                sf::Vector2f position = getCenterPosition() + sf::normalize(ship_offset) * getRadius() * 0.8f;

                sf::Sprite arrow_sprite;
                textureManager.setTexture(arrow_sprite, "waypoint");
                arrow_sprite.setPosition(position);
                arrow_sprite.setRotation(sf::vector2ToAngle(ship_offset) - 90);
                forground_texture.draw(arrow_sprite);
            }
        }
    } else {
        // cut a mask for the shape of the radar
    //When we have a circular masked radar, use the mask_texture to clear out everything that is not part of the circle.
        deOrient();
        mask_texture.clear(sf::Color(0, 0, 0, 0));
        if(style == CircularSector){
            const int pointCnt = 15;
            const float pieAngle = M_PI / 3.0f; // 60 degrees
            const float r = getRadius() - 2.0f;
            const sf::Vector2f center(rect.width / 2.0, rect.height);
            sf::ConvexShape sector;
            sector.setPointCount(pointCnt);
            sector.setPoint(0, center);
            for (int index = 1; index < pointCnt; index++){
                // can use sfml vector from angle
                float angle = (M_PI - pieAngle) / 2  + index * pieAngle / pointCnt;
                sector.setPoint(index, center + sf::Vector2f(r * std::cos(-angle), r * std::sin(-angle)));
            }
            sector.setOrigin(0, 0);
            sector.setPosition(sf::Vector2f(rect.left, rect.top));
            sector.setFillColor(sf::Color::Black);
            sector.setOutlineColor(colorConfig.radar_outline);
            sector.setOutlineThickness(2.0);
            mask_texture.draw(sector);
        } else {
            float r = getRadius() - 2.0f;
            sf::CircleShape circle(r, 50);
            circle.setOrigin(r, r);
            circle.setPosition(getCenterPoint());
            circle.setFillColor(sf::Color::Black);
            circle.setOutlineColor(colorConfig.radar_outline);
            circle.setOutlineThickness(2.0);
            mask_texture.draw(circle);
        }

        sf::BlendMode blend_mode(
            sf::BlendMode::One, sf::BlendMode::SrcAlpha, sf::BlendMode::Add,
            sf::BlendMode::Zero, sf::BlendMode::SrcAlpha, sf::BlendMode::Add
        );
        drawRenderTexture(mask_texture, background_texture, sf::Color::White, blend_mode);
        drawRenderTexture(mask_texture, forground_texture, sf::Color::White, blend_mode);
        orient();
    }

    //Render the final radar
    drawRenderTexture(background_texture, window);
    drawRenderTexture(forground_texture, window);
}

void GuiRadarView::updateGhostDots()
{
    if (next_ghost_dot_update < engine->getElapsedTime())
    {
        next_ghost_dot_update = engine->getElapsedTime() + 5.0;
        foreach(SpaceObject, obj, space_object_list)
        {
            P<SpaceShip> ship = obj;
            if (ship && sf::length(obj->getPosition() - getViewPosition()) < getDistance())
            {
                ghost_dots.push_back(GhostDot(obj->getPosition()));
            }
        }

        for(unsigned int n=0; n < ghost_dots.size(); n++)
        {
            if (ghost_dots[n].end_of_life <= engine->getElapsedTime())
            {
                ghost_dots.erase(ghost_dots.begin() + n);
                n--;
            }
        }
    }
}

void GuiRadarView::drawBackground(sf::RenderTarget& window)
{
    window.clear(sf::Color(20, 20, 20, 255));
}

void GuiRadarView::drawFogOfWarBlockedAreas(sf::RenderTarget& window){
    if (my_spaceship)
    {
        PVector<Nebula> nebulas = Nebula::getNebulas();
        foreach(SpaceObject, obj, space_object_list)
        {
            sf::Vector2f scan_center = obj->getPosition();
            float range = obj->getRadarRange() * getScale();
            if (range > 0.0f && obj->isFriendly(my_spaceship))
            {
                temp.clear(blockedMaskColor);
                sf::CircleShape circle(range, 50);
                circle.setOrigin(range, range);
                circle.setFillColor(visibleMaskColor);

                circle.setPosition(worldToScreen(scan_center));
                temp.draw(circle, sf::BlendAdd);

                // reduce nebulas
                
                // TODO: does not render correctly. need to propperly subtract nebulas from temp
                foreach(Nebula, n, nebulas)
                {
                    sf::Vector2f diff = n->getPosition() - scan_center;
                    float diff_len = sf::length(diff);

                    if (diff_len < n->getRadius() + getDistance())
                    {
                        if (diff_len < n->getRadius())
                        {
                            sf::RectangleShape background(sf::Vector2f(rect.width, rect.height));
                            background.setPosition(rect.left, rect.top);
                            background.setFillColor(neutralMaskColor);
                            temp.draw(background, sf::BlendMultiply); //, blend);
                        } else {
                            float r = n->getRadius() * getScale();
                            sf::CircleShape circle(r, 32);
                            circle.setOrigin(r, r);
                            circle.setPosition(worldToScreen(n->getPosition()));
                            circle.setFillColor(blockedMaskColor);
                            window.draw(circle);//, blend);

                            float diff_angle = sf::vector2ToAngle(diff);
                            float angle = acosf(n->getRadius() / diff_len) / M_PI * 180.0f;

                            sf::Vector2f pos_a = n->getPosition() - sf::vector2FromAngle(diff_angle + angle) * n->getRadius();
                            sf::Vector2f pos_b = n->getPosition() - sf::vector2FromAngle(diff_angle - angle) * n->getRadius();
                            sf::Vector2f pos_c = scan_center + sf::normalize(pos_a - scan_center) * getDistance() * 3.0f;
                            sf::Vector2f pos_d = scan_center + sf::normalize(pos_b - scan_center) * getDistance() * 3.0f;
                            sf::Vector2f pos_e = scan_center + diff / diff_len * getDistance() * 3.0f;

                            sf::VertexArray a(sf::TriangleStrip, 5);
                            a[0].position = worldToScreen(pos_a);
                            a[1].position = worldToScreen(pos_b);
                            a[2].position = worldToScreen(pos_c);
                            a[3].position = worldToScreen(pos_d);
                            a[4].position = worldToScreen(pos_e);
                            for(int n=0; n<5;n++)
                                a[n].color = blockedMaskColor;
                            window.draw(a);//, blend);
                        }
                    }
                }

                // float r = 5000.0f * getScale();
                // sf::CircleShape circle(r, 32);
                // circle.setOrigin(r, r);
                // circle.setPosition(worldToScreen(scan_center));
                // circle.setFillColor(visibleMaskColor);
                // window.draw(circle);//, blend);

                drawRenderTexture(temp, window, sf::Color::White, sf::BlendAdd);
            }
        }
    }
}

void GuiRadarView::drawGhostDots(sf::RenderTarget& window)
{
    sf::VertexArray ghost_points(sf::Points, ghost_dots.size());
    for(unsigned int n=0; n<ghost_dots.size(); n++)
    {
        ghost_points[n].position = worldToScreen(ghost_dots[n].position);
        ghost_points[n].color = sf::Color(255, 255, 255, 255 * ((ghost_dots[n].end_of_life - engine->getElapsedTime()) / GhostDot::total_lifetime));
    }
    window.draw(ghost_points);
}

void GuiRadarView::drawWaypoints(sf::RenderTarget& window)
{
    if (!my_spaceship)
        return;

    deOrient();
    sf::Vector2f radar_screen_center = getCenterPosition();
    for(unsigned int n=0; n<my_spaceship->waypoints.size(); n++)
    {
        sf::Vector2f screen_position = orientVector(worldToScreen(my_spaceship->waypoints[n]));
        sf::Sprite object_sprite;
        textureManager.setTexture(object_sprite, "waypoint");
        object_sprite.setColor(colorConfig.ship_waypoint_background);
        object_sprite.setPosition(screen_position - sf::Vector2f(0, 10));
        object_sprite.setScale(0.8, 0.8);
        window.draw(object_sprite);

        drawText(window, sf::FloatRect(screen_position.x, screen_position.y - 10, 0, 0), string(n + 1), ACenter, 18, bold_font, colorConfig.ship_waypoint_text);

        if (style != Rectangular && sf::length(screen_position - radar_screen_center) > getRadius())
        {
            sf::Vector2f offset = my_spaceship->waypoints[n] - getViewPosition();
            screen_position = orientVector(radar_screen_center + (offset / sf::length(offset) * getRadius() * 0.8f));

            object_sprite.setPosition(screen_position);
            object_sprite.setRotation(sf::vector2ToAngle(screen_position - radar_screen_center) - 90);
            window.draw(object_sprite);

            drawText(window, sf::FloatRect(screen_position.x, screen_position.y, 0, 0), string(n + 1), ACenter, 18, bold_font, colorConfig.ship_waypoint_text);
        }
    }
    orient();
}

void GuiRadarView::drawRangeIndicators(sf::RenderTarget& window)
{
    if (range_indicator_step_size < 1.0)
        return;

    deOrient();
    sf::Vector2f radar_screen_center = getCenterPosition();
    for(float circle_size=range_indicator_step_size; circle_size < getDistance(); circle_size+=range_indicator_step_size)
    {
        float s = circle_size * getScale();
        sf::CircleShape circle(s, 50);
        circle.setOrigin(s, s);
        circle.setPosition(radar_screen_center);
        circle.setFillColor(sf::Color::Transparent);
        circle.setOutlineColor(sf::Color(255, 255, 255, 16));
        circle.setOutlineThickness(2.0);
        window.draw(circle);
        drawText(window, sf::FloatRect(radar_screen_center.x, radar_screen_center.y - s - 20, 0, 0), string(int(circle_size / 1000.0f + 0.1f)) + DISTANCE_UNIT_1K, ACenter, 20, bold_font, sf::Color(255, 255, 255, 32));
    }
    orient();
}

void GuiRadarView::drawTargetProjections(sf::RenderTarget& window)
{
    const float seconds_per_distance_tick = 5.0f;
    if (target_spaceship && missile_tube_controls)
    {
        for(int n=0; n<target_spaceship->weapon_tube_count; n++)
        {
            if (!target_spaceship->weapon_tube[n].isLoaded())
                continue;
            sf::Vector2f fire_position = target_spaceship->getPosition() + sf::rotateVector(target_spaceship->ship_template->model_data->getTubePosition2D(n), target_spaceship->getRotation());
            sf::Vector2f fire_draw_position = getCenterPosition() + (getViewPosition() - fire_position) * getScale();

            const MissileWeaponData& data = MissileWeaponData::getDataFor(target_spaceship->weapon_tube[n].getLoadType());
            float fire_angle = target_spaceship->getRotation() + target_spaceship->weapon_tube[n].getDirection();
            float missile_target_angle = fire_angle;
            if (data.turnrate > 0.0f)
            {
                if (missile_tube_controls->getManualAim())
                {
                    missile_target_angle = missile_tube_controls->getMissileTargetAngle();
                }else{
                    float firing_solution = target_spaceship->weapon_tube[n].calculateFiringSolution(target_spaceship->getTarget());
                    if (firing_solution != std::numeric_limits<float>::infinity())
                        missile_target_angle = firing_solution;
                }
            }

            float angle_diff = sf::angleDifference(missile_target_angle, fire_angle);
            float turn_radius = ((360.0f / data.turnrate) * data.speed) / (2.0f * M_PI);
            if (data.turnrate == 0.0f)
                turn_radius = 0.0f;

            float left_or_right = 90;
            if (angle_diff > 0)
                left_or_right = -90;

            sf::Vector2f turn_center = sf::vector2FromAngle(fire_angle + left_or_right) * turn_radius;
            sf::Vector2f turn_exit = turn_center + sf::vector2FromAngle(missile_target_angle - left_or_right) * turn_radius;

            float turn_distance = fabs(angle_diff) / 360.0 * (turn_radius * 2.0f * M_PI);
            float lifetime_after_turn = data.lifetime - turn_distance / data.speed;
            float length_after_turn = data.speed * lifetime_after_turn;

            sf::VertexArray a(sf::LinesStrip, 13);
            a[0].position = fire_draw_position;
            for(int cnt=0; cnt<10; cnt++)
                a[cnt + 1].position = fire_draw_position + (turn_center + sf::vector2FromAngle(fire_angle - angle_diff / 10.0f * cnt - left_or_right) * turn_radius) * getScale();
            a[11].position = fire_draw_position + turn_exit * getScale();
            a[12].position = fire_draw_position + (turn_exit + sf::vector2FromAngle(missile_target_angle) * length_after_turn) * getScale();
            for(int cnt=0; cnt<13; cnt++)
                a[cnt].color = sf::Color(255, 255, 255, 128);
            window.draw(a);

            float offset = seconds_per_distance_tick * data.speed;
            for(int cnt=0; cnt<floor(data.lifetime / seconds_per_distance_tick); cnt++)
            {
                sf::Vector2f p;
                sf::Vector2f n;
                if (offset < turn_distance)
                {
                    n = sf::vector2FromAngle(fire_angle - (angle_diff * offset / turn_distance) - left_or_right);
                    p = (turn_center + n * turn_radius) * getScale();
                }else{
                    p = (turn_exit + sf::vector2FromAngle(missile_target_angle) * (offset - turn_distance)) * getScale();
                    n = sf::vector2FromAngle(missile_target_angle + 90.0f);
                }
                sf::VertexArray a(sf::Lines, 2);
                a[0].position = fire_draw_position + p - n * 10.0f;
                a[1].position = fire_draw_position + p + n * 10.0f;
                window.draw(a);

                offset += seconds_per_distance_tick * data.speed;
            }
        }
    }

    if (getTargets())
    {
        for(P<SpaceObject> obj : getTargets()->getTargets())
        {
            if (obj->getVelocity() < 1.0f)
                continue;

            sf::VertexArray a(sf::Lines, 12);
            a[0].position = worldToScreen(obj->getPosition());
            a[0].color = sf::Color(255, 255, 255, 128);
            a[1].position = a[0].position + (obj->getVelocity() * 60.0f) * getScale();
            a[1].color = sf::Color(255, 255, 255, 0);
            sf::Vector2f n = sf::normalize(sf::Vector2f(-obj->getVelocity().y, obj->getVelocity().x));
            for(int cnt=0; cnt<5; cnt++)
            {
                sf::Vector2f p = (obj->getVelocity() * (seconds_per_distance_tick + seconds_per_distance_tick * cnt)) * getScale();
                a[2 + cnt * 2].position = a[0].position + p + n * 10.0f;
                a[3 + cnt * 2].position = a[0].position + p - n * 10.0f;
                a[2 + cnt * 2].color = a[3 + cnt * 2].color = sf::Color(255, 255, 255, 128 - cnt * 20);
            }
            window.draw(a);
        }
    }
}

void GuiRadarView::drawMissileTubes(sf::RenderTarget& window)
{
    if (target_spaceship)
    {
        sf::VertexArray a(sf::LinesStrip, target_spaceship->weapon_tube_count * 2);
        for(int n=0; n<target_spaceship->weapon_tube_count; n++)
        {
            sf::Vector2f fire_position = target_spaceship->getPosition() + sf::rotateVector(target_spaceship->ship_template->model_data->getTubePosition2D(n), target_spaceship->getRotation());
            sf::Vector2f fire_draw_position = getCenterPosition() + (getViewPosition() - fire_position) * getScale();

            float fire_angle = target_spaceship->getRotation() + target_spaceship->weapon_tube[n].getDirection();
            
            a[n * 2].position = fire_draw_position;
            a[n * 2 + 1].position = fire_draw_position + (sf::vector2FromAngle(fire_angle) * 1000.0f) * getScale();
            a[n * 2].color = sf::Color(128, 128, 128, 128);
            a[n * 2 + 1].color = sf::Color(128, 128, 128, 0);
        }
        window.draw(a);
    }
}

PVector<SpaceObject> GuiRadarView::getVisibleObjects(){
    PVector<SpaceObject> visible_objects;
    switch(fog_style)
    {
    case NoFogOfWar:
        visible_objects = PVector<SpaceObject>(space_object_list);
        break;
    case RadarRangeAndLineOfSight:
        if (!target_spaceship)
            return visible_objects;
        foreach(SpaceObject, obj, space_object_list)
        {
            if (!obj->canHideInNebula())
                visible_objects.push_back(obj);
            float range = obj->getRadarRange();
            if (range > 0.0f && obj->isFriendly(target_spaceship))
            {
                sf::Vector2f position = obj->getPosition();
                PVector<Collisionable> obj_list = CollisionManager::queryArea(position - sf::Vector2f(range, range), position + sf::Vector2f(range, range));
                foreach(Collisionable, c_obj, obj_list)
                {
                    P<SpaceObject> obj2 = c_obj;
                    if (obj2 
                        && (obj->getPosition() - obj2->getPosition()) < range + obj2->getRadius()
                        && !(obj->canHideInNebula() && Nebula::blockedByNebula(obj->getPosition(), obj2->getPosition())))
                    {
                        visible_objects.push_back(obj2);
                    }
                }
            }
        }
        break;
    case NoObjects: 
        foreach(SpaceObject, obj, space_object_list)
        {
            if (P<Nebula>(obj) || P<BlackHole>(obj))
                visible_objects.push_back(obj);
        }
        break;
    }
    return visible_objects;
}

void GuiRadarView::drawObjects(sf::RenderTarget& window_normal, sf::RenderTarget& window_alpha)
{
    PVector<SpaceObject> visible_objects = getVisibleObjects();
    foreach(SpaceObject, obj, visible_objects)
    {
        sf::Vector2f object_position_on_screen = worldToScreen(obj->getPosition());
        float r = obj->getRadius() * getScale();
        sf::FloatRect object_rect(object_position_on_screen.x - r, object_position_on_screen.y - r, r * 2, r * 2);
        if (obj != *my_spaceship && rect.intersects(object_rect))
        {
            sf::RenderTarget* window = &window_normal;
            if (!obj->canHideInNebula())
                window = &window_alpha; // draw on background
            obj->drawOnRadar(*window, object_position_on_screen, getScale(), long_range);
            if (show_callsigns && obj->getCallSign() != ""){
                deOrient();
                sf::Vector2f object_position_on_screen_oriented = orientVector(worldToScreen(obj->getPosition()));
                drawText(*window, sf::FloatRect(object_position_on_screen_oriented.x, object_position_on_screen_oriented.y - 15, 0, 0), obj->getCallSign(), ACenter, 15, bold_font);
                orient();
            }
        }
    }
    if (my_spaceship)
    {
        sf::Vector2f object_position_on_screen = worldToScreen(my_spaceship->getPosition());
        my_spaceship->drawOnRadar(window_normal, object_position_on_screen, getScale(), long_range);
    }
}

void GuiRadarView::drawObjectsGM(sf::RenderTarget& window)
{
    foreach(SpaceObject, obj, space_object_list)
    {
        sf::Vector2f object_position_on_screen = worldToScreen(obj->getPosition());
        float r = obj->getRadius() * getScale();
        sf::FloatRect object_rect(object_position_on_screen.x - r, object_position_on_screen.y - r, r * 2, r * 2);
        if (rect.intersects(object_rect))
        {
            obj->drawOnGMRadar(window, object_position_on_screen, getScale(), long_range);
        }
    }
}

void GuiRadarView::drawHeadingIndicators(sf::RenderTarget& window)
{
    sf::Vector2f radar_screen_center = getCenterPosition();
    float boundingRadius = getRadius();
    sf::VertexArray tigs(sf::Lines, 360/20*2);
    for(unsigned int n=0; n<360; n+=20)
    {
        tigs[n/20*2].position = radar_screen_center + sf::vector2FromAngle(float(n) - 90) * (boundingRadius - 20);
        tigs[n/20*2+1].position = radar_screen_center + sf::vector2FromAngle(float(n) - 90) * (boundingRadius - 40);
    }
    window.draw(tigs);
    sf::VertexArray small_tigs(sf::Lines, 360/5*2);
    for(unsigned int n=0; n<360; n+=5)
    {
        small_tigs[n/5*2].position = radar_screen_center + sf::vector2FromAngle(float(n) - 90) * (boundingRadius - 20);
        small_tigs[n/5*2+1].position = radar_screen_center + sf::vector2FromAngle(float(n) - 90) * (boundingRadius - 30);
    }
    window.draw(small_tigs);
    for(unsigned int n=0; n<360; n+=20)
    {
        sf::Text text(string(n), *main_font, 15);
        text.setPosition(radar_screen_center + sf::vector2FromAngle(float(n) - 90) * (boundingRadius - 45));
        text.setOrigin(text.getLocalBounds().width / 2.0, text.getLocalBounds().height / 2.0);
        text.setRotation(n);
        window.draw(text);
    }
}

float GuiRadarView::getRadius(){
    if(style == CircularSector)
        return std::min(rect.width, rect.height) * 0.85f;  // the last 15% has artifacts from the mask rotation
    else 
        return std::min(rect.width, rect.height) / 2.0f;
}

sf::Vector2f GuiRadarView::getCenterPosition(){
    if(style == CircularSector){
        sf::Vector2f deOrientedCenter(0, std::min(rect.width, rect.height) / 2.0f);
        return SectorsView::getCenterPosition() + sf::rotateVector(deOrientedCenter, background_texture.getView().getRotation());
    } else 
        return SectorsView::getCenterPosition();
}

float GuiRadarView::getScale(){
    return getRadius() / getDistance();
}

bool GuiRadarView::onMouseDown(sf::Vector2f position)
{
    if (style == Circular || style == CircularMasked|| style == CircularSector)
    {
        if (position - getCenterPoint() > getRadius())
            return false;
    }
    return SectorsView::onMouseDown(position);
}
