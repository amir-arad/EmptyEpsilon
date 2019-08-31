#ifndef RADAR_VIEW_H
#define RADAR_VIEW_H

#include "gui/gui2_element.h"
#include "sectorsView.h"
#include "spaceObjects/playerSpaceship.h"

class MissileAim;

class GuiRadarView : public SectorsView
{
public:
    enum ERadarStyle
    {
        Rectangular,
        Circular,
        CircularMasked,
        CircularSector
    };
    enum EFogOfWarStyle
    {
        NoFogOfWar, // draw everything
        RadarRangeAndLineOfSight, // draw everything in range of friendly that is not hidden by nebula
        NoObjects // draw only and all nebulas / black holes
    };

    typedef std::function<void(sf::Vector2f position)> func_t;
    typedef std::function<void(float position)>        ffunc_t;
private:
    sf::RenderTexture background_texture;
    sf::RenderTexture forground_texture;
    sf::RenderTexture mask_texture;
    sf::RenderTexture temp;
    class GhostDot
    {
    public:
        constexpr static float total_lifetime = 60.0f;

        sf::Vector2f position;
        float end_of_life;

        GhostDot(sf::Vector2f pos) : position(pos), end_of_life(engine->getElapsedTime() + total_lifetime) {}
    };
    std::vector<GhostDot> ghost_dots;
    float next_ghost_dot_update;
    MissileAim* missile_tube_controls;

    P<SpaceShip> target_spaceship;
    bool long_range;
    bool show_ghost_dots;
    bool show_waypoints;
    bool show_target_projection;
    bool show_missile_tubes;
    bool show_callsigns;
    bool show_heading_indicators;
    bool show_game_master_data;
    bool auto_center_on_my_ship;
    bool auto_orient;
    bool show_sectors;
    float range_indicator_step_size;
    ERadarStyle style;
    EFogOfWarStyle fog_style;
    func_t mouse_down_func;
    func_t mouse_drag_func;
    func_t mouse_up_func;
public:
    GuiRadarView(GuiContainer* owner, string id, float distance, TargetsContainer* targets, P<SpaceShip> targetSpaceship);

    virtual void onDraw(sf::RenderTarget& window);

    virtual GuiRadarView* setDistance(float distance) { SectorsView::setDistance(distance); return this; }
    GuiRadarView* setRangeIndicatorStepSize(float step) { range_indicator_step_size = step; return this; }
    GuiRadarView* longRange() { long_range = true; return this; }
    GuiRadarView* shortRange() { long_range = false; return this; }
    GuiRadarView* enableGhostDots() { show_ghost_dots = true; return this; }
    GuiRadarView* disableGhostDots() { show_ghost_dots = false; return this; }
    GuiRadarView* enableWaypoints() { show_waypoints = true; return this; }
    GuiRadarView* disableWaypoints() { show_waypoints = false; return this; }
    GuiRadarView* enableTargetProjections(MissileAim* missile_tube_controls) { show_target_projection = true; this->missile_tube_controls = missile_tube_controls; return this; }
    GuiRadarView* disableTargetProjections() { show_target_projection = false; return this; }
    GuiRadarView* enableMissileTubeIndicators() { show_missile_tubes = true; return this; }
    GuiRadarView* disableMissileTubeIndicators() { show_missile_tubes = false; return this; }
    GuiRadarView* enableCallsigns() { show_callsigns = true; return this; }
    GuiRadarView* disableCallsigns() { show_callsigns = false; return this; }
    GuiRadarView* enableHeadingIndicators() { show_heading_indicators = true; return this; }
    GuiRadarView* disableHeadingIndicators() { show_heading_indicators = false; return this; }
    GuiRadarView* gameMaster() { show_game_master_data = true; return this; }
    GuiRadarView* setStyle(ERadarStyle style) { this->style = style; return this; }
    GuiRadarView* setFogOfWarStyle(EFogOfWarStyle style) { this->fog_style = style; return this; }
    bool getAutoCentering() { return auto_center_on_my_ship; }
    GuiRadarView* setAutoCentering(bool value) { this->auto_center_on_my_ship = value; return this; }
    bool getAutoOrient() { return auto_orient; }
    GuiRadarView* setAutoOrient(bool value) { this->auto_orient = value; return this; }
    bool getShowSectors() { return show_sectors; }
    GuiRadarView* setShowSectors(bool value) { this->show_sectors = value; return this; }
    virtual GuiRadarView* setCallbacks(func_t mouse_down_func, func_t mouse_drag_func, func_t mouse_up_func) { SectorsView::setCallbacks(mouse_down_func, mouse_drag_func, mouse_up_func); return this; }
    virtual GuiRadarView* setViewPosition(sf::Vector2f view_position) { SectorsView::setViewPosition(view_position); return this; }
    virtual bool onMouseDown(sf::Vector2f position);
    GuiRadarView* setTargetSpaceship(P<SpaceShip> targetSpaceship){target_spaceship = targetSpaceship; return this;}

protected:
    virtual float getScale() override;
    virtual sf::Vector2f getCenterPosition() override;
private:
    void updateGhostDots();
    void drawBackground(sf::RenderTarget& window);
    void drawFogOfWarBlockedAreas(sf::RenderTarget& window);
    void drawGhostDots(sf::RenderTarget& window);
    void drawWaypoints(sf::RenderTarget& window);
    void drawRangeIndicators(sf::RenderTarget& window);
    void drawTargetProjections(sf::RenderTarget& window);
    void drawMissileTubes(sf::RenderTarget& window);
    void drawObjects(sf::RenderTarget& window_normal, sf::RenderTarget& window_alpha);
    void drawObjectsGM(sf::RenderTarget& window);
    void drawHeadingIndicators(sf::RenderTarget& window);
    void orient();
    void deOrient();
    sf::Vector2f orientVector(sf::Vector2f input);
    void rotateTexture(sf::RenderTexture& texture, float rotation);
    float getRadius();
};

#endif//RADAR_VIEW_H
