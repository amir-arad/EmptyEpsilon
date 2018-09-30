#ifndef NAVIGATION_SCREEN_H
#define NAVIGATION_SCREEN_H

#include "screenComponents/targetsContainer.h"
#include "screenComponents/navigationView.h"
#include "gui/gui2_overlay.h"

class NavigationView;
class GuiKeyValueDisplay;
class GuiAutoLayout;
class GuiButton;
class GuiToggleButton;
class GuiSlider;
class GuiLabel;
class GuiHackingDialog;
class GuiTextEntry;

class NavigationScreen : public GuiOverlay
{
private:
    enum EMode
    {
        TargetSelection,
        WaypointPlacement,
        MoveWaypoint
    };

    EMode mode;
    TargetsContainer targets;
    int drag_waypoint_index;
    NavigationView* radar;
    GuiAutoLayout* option_buttons;
    GuiAutoLayout* view_controls;
    GuiButton* delete_waypoint_button;

    bool sector_name_custom;
    GuiTextEntry* sector_name_text;

    GuiSlider* zoom_slider;
    GuiLabel* zoom_label;
    
    sf::Vector2f mouse_down_position;
    const float max_distance = 10000000.0f;
    const float min_distance = 6250.0f;
public:
    NavigationScreen(GuiContainer* owner);

    virtual void onDraw(sf::RenderTarget& window);
};

#endif//NAVIGATION_SCREEN_H
