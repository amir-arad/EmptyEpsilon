#include "playerInfo.h"
#include "navigationScreen.h"
#include "spaceObjects/playerSpaceship.h"
#include "spaceObjects/scanProbe.h"
#include "scriptInterface.h"
#include "gameGlobalInfo.h"

#include "screenComponents/navigationView.h"
#include "screenComponents/openCommsButton.h"
#include "screenComponents/commsOverlay.h"
#include "screenComponents/shipsLogControl.h"
#include "screenComponents/hackingDialog.h"
#include "screenComponents/customShipFunctions.h"

#include "gui/gui2_autolayout.h"
#include "gui/gui2_keyvaluedisplay.h"
#include "gui/gui2_selector.h"
#include "gui/gui2_slider.h"
#include "gui/gui2_label.h"
#include "gui/gui2_togglebutton.h"
#include "gui/gui2_textentry.h"

NavigationScreen::NavigationScreen(GuiContainer *owner)
    : GuiOverlay(owner, "NAVIGATION_SCREEN", colorConfig.background), mode(TargetSelection)
{
    targets.setAllowWaypointSelection();
    radar = new NavigationView(this, "NAVIGATION_RADAR", min_distance, &targets);
    radar->setPosition(0, 0, ATopLeft)->setSize(GuiElement::GuiSizeMax, GuiElement::GuiSizeMax);
    radar->setCallbacks(
        [this](sf::Vector2f position) { //down
            if (!placeWayPoints && mode == TargetSelection && targets.getWaypointIndex() > -1 && my_spaceship)
            {
                if (sf::length(my_spaceship->waypoints[targets.getWaypointIndex()] - position) < 25.0 / radar->getScale())
                {
                    mode = MoveWaypoint;
                    drag_waypoint_index = targets.getWaypointIndex();
                }
            }
            mouse_down_position = position;
        },
        [this](sf::Vector2f position) { //drag
            if (!placeWayPoints && mode == TargetSelection)
            {
                position_text_custom = false;
                sf::Vector2f newPosition = radar->getViewPosition() - (position - mouse_down_position);
                radar->setViewPosition(newPosition);
                if (!position_text_custom)
                    position_text->setText(getStringFromPosition(newPosition));
            }
            if (!placeWayPoints && mode == MoveWaypoint && my_spaceship)
                my_spaceship->commandMoveWaypoint(drag_waypoint_index, position);
        },
        [this](sf::Vector2f position) { //up
            if (placeWayPoints){
                placeWaypoint(position);
            } else {
                switch (mode)
                {
                case TargetSelection:
                    targets.setToClosestTo(position, 25.0 / radar->getScale(), TargetsContainer::Targetable);
                    break;
                case MoveWaypoint:
                    mode = TargetSelection;
                    targets.setWaypointIndex(drag_waypoint_index);
                    break;
                }
            }
        });

    if (my_spaceship)
        radar->setViewPosition(my_spaceship->getPosition());

    placeWayPoints = false;
    // Controls for the radar view
    GuiAutoLayout* view_controls = new GuiAutoLayout(this, "VIEW_CONTROLS", GuiAutoLayout::LayoutVerticalBottomToTop);
    view_controls->setPosition(20, -70, ABottomLeft)->setSize(250, GuiElement::GuiSizeMax);
    zoom_slider = new GuiSlider(view_controls, "ZOOM_SLIDER", max_distance, min_distance, radar->getDistance(), [this](float value) {
        zoom_label->setText("Zoom: " + string(max_distance / value, 1.0f) + "x");
        radar->setDistance(value);
    });
    zoom_slider->setPosition(20, -70, ABottomLeft)->setSize(GuiElement::GuiSizeMax, 50);
    zoom_slider->setVisible(false);
    zoom_label = new GuiLabel(zoom_slider, "", "Zoom: " + string(max_distance / radar->getDistance(), 1.0f) + "x", 30);
    zoom_label->setSize(GuiElement::GuiSizeMax, GuiElement::GuiSizeMax);
    position_text_custom = false;
    position_text = new GuiTextEntry(view_controls, "SECTOR_NAME_TEXT", "");
    position_text->setSize(GuiElement::GuiSizeMax, 50);
    position_text->callback([this](string text) {
        position_text_custom = true;
    });
    position_text->validator(isValidPositionString);
    position_text->enterCallback([this](string text) {
        position_text_custom = false;
        if (position_text->isValid())
        {
            sf::Vector2f pos = getPositionFromSring(text);
            radar->setViewPosition(pos);
        }
    });
    position_text->setText(getStringFromPosition(radar->getViewPosition()));

    GuiAutoLayout* waypoint_controls = new GuiAutoLayout(this, "waypoint_controls", GuiAutoLayout::LayoutVerticalTopToBottom);
    waypoint_controls->setPosition(20, 50, ATopLeft)->setSize(250, GuiElement::GuiSizeMax);
    
    GuiAutoLayout* layers_controls = new GuiAutoLayout(this, "LAYERS_CONTROLS", GuiAutoLayout::LayoutVerticalTopToBottom);
    layers_controls->setPosition(-20, 50, ATopRight)->setSize(250, GuiElement::GuiSizeMax);
    
    //manage layers
    for (int n = 0; n < GameGlobalInfo::max_map_layers; n++){
        if (gameGlobalInfo->layer[n].defined){
            // change its state on draw?
            layerButtons[n] = new GuiButton(layers_controls, "LAYER_TOGGLE_" + string(n, 0), gameGlobalInfo->layer[n].title,  [this, n]() {
                radar->toggleTerrainLayer(n);
            });
            layerButtons[n]->setSize(GuiElement::GuiSizeMax, 50);
        } else {
            layerButtons[n] = nullptr;
        }
    }

    // Manage waypoints.
    waypoint_place_button = new GuiButton(waypoint_controls, "WAYPOINT_PLACE_BUTTON", "Place Waypoints", [this]() {
        placeWayPoints = !placeWayPoints;
    });
    waypoint_place_button->setSize(GuiElement::GuiSizeMax, 50);

    (new GuiButton(waypoint_controls, "WAYPOINT_PLACE_AT_CENTER_BUTTON", "Waypoint At View Center",  [this]() {
       placeWaypoint(radar->getViewPosition());
    }))->setSize(GuiElement::GuiSizeMax, 50);

    delete_waypoint_button = new GuiButton(waypoint_controls, "WAYPOINT_DELETE_BUTTON", "Delete Waypoint", [this]() {
        if (my_spaceship && targets.getWaypointIndex() >= 0)
        {
            my_spaceship->commandRemoveWaypoint(targets.getWaypointIndex());
        }
    });
    delete_waypoint_button->setSize(GuiElement::GuiSizeMax, 50);

    (new GuiCustomShipFunctions(this, navigation, "", my_spaceship))->setPosition(-20, 240, ATopRight)->setSize(250, GuiElement::GuiSizeMax);
}

void NavigationScreen::placeWaypoint(sf::Vector2f position)
{
    if (my_spaceship)
        my_spaceship->commandAddWaypoint(position);
}

void NavigationScreen::onDraw(sf::RenderTarget &window)
{
    ///Handle mouse wheel
    float mouse_wheel_delta = InputHandler::getMouseWheelDelta();
    if (mouse_wheel_delta != 0.0)
    {
        float view_distance = radar->getDistance() * (1.0 - (mouse_wheel_delta * 0.1f));
        zoom_slider->setValue(view_distance);
        view_distance = zoom_slider->getValue();
        radar->setDistance(view_distance);
        zoom_label->setText("Zoom: " + string(max_distance / view_distance, 1.0f) + "x");
    }
    ///!

    GuiOverlay::onDraw(window);

    if (targets.getWaypointIndex() >= 0)
        delete_waypoint_button->enable();
    else
        delete_waypoint_button->disable();
    
    waypoint_place_button->setActive(placeWayPoints);
    for (int n = 0; n < GameGlobalInfo::max_map_layers; n++){
        if (layerButtons[n]){
            layerButtons[n]->setActive(radar->getTerrainLayer(n));
        }
    }
}

void NavigationScreen::onHotkey(const HotkeyResult& key)
{
    if (key.category == "NAVIGATION")
    {
        if (key.hotkey == "PLACE_WAYPOINTS") {
            placeWayPoints = !placeWayPoints;
        } else if (key.hotkey == "WAYPOINT_PLACE_AT_CENTER") {
            placeWaypoint(radar->getViewPosition());
        } else if (key.hotkey == "WAYPOINT_DELETE" && my_spaceship && targets.getWaypointIndex() >= 0) {
            my_spaceship->commandRemoveWaypoint(targets.getWaypointIndex());
        } else {
            for (int n = 0; n < GameGlobalInfo::max_map_layers; n++){
                if (layerButtons[n] && key.hotkey == "LAYER_TOGGLE_" + string(n, 0)){
                    radar->toggleTerrainLayer(n);
                }
            }
        }
    }
}