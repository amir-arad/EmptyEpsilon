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

#include "PerlinNoise.h"
#include "SimplexNoise.hpp"

/*
class NavigationView : public GuiRadarView
{
    private:
    std::unique_ptr<sf::Uint8[]> pixels;
    sf::Texture noiseTexture;
    sf::Sprite noiseSprite;
    SimplexNoise noiseGenerator;
    
    int height = 2048;
    int width =2048;
  public:
    NavigationView(GuiContainer *owner, string id, float distance, TargetsContainer *targets);

  protected:
    virtual void drawBackground(sf::RenderTarget &window);
};

NavigationView::NavigationView(GuiContainer *owner, string id, float distance, TargetsContainer *targets)
    : GuiRadarView(owner, id, distance, targets)
{
    setAutoCentering(false);
    longRange();
    enableWaypoints();
    //   enableCallsigns();
    setStyle(GuiRadarView::Rectangular);
  //  setFogOfWarStyle(GuiRadarView::CosmicMap);
    
    // pixels = std::unique_ptr<sf::Uint8[]>(new sf::Uint8[width * height * 4]);
    // noiseGenerator.setOctaves(5);
    // noiseGenerator.setFrequency(2.0f);
    // noiseGenerator.setPersistence(0.45f);
    // noiseTexture.create(width, height);
    noiseTexture.loadFromFile("resources/cosmic-map-1.png");
}
void NavigationView::drawBackground(sf::RenderTarget &window)
{
    // for (std::size_t y = 0; y < height; ++y)
    // {
    //     for (std::size_t x = 0; x < width; ++x)
    //     {
    //         float xPos = float(x) / float(width) - 0.5f;
    //         float yPos = float(y) / float(height) - 0.5f;

    //         float elevation = noiseGenerator.unsignedOctave(xPos, yPos);
    //         elevation = pow(elevation, 1.5f); //redistribution
    //         sf::Color color = sf::Color(elevation * 255, elevation * 255, elevation * 255, elevation * 255);
    //         pixels[4 * (y * width + x)] = color.r;
    //         pixels[4 * (y * width + x) + 1] = color.g;
    //         pixels[4 * (y * width + x) + 2] = color.b;
    //         pixels[4 * (y * width + x) + 3] = color.a;
    //         // setPixel(x, y, elevation);
    //     }
    // }
    // noiseTexture.update(pixels.get());

    window.clear(sf::Color(100, 20, 20, 255));
    noiseSprite.setTexture(noiseTexture);
    window.draw(noiseSprite);
}
*/

NavigationScreen::NavigationScreen(GuiContainer *owner)
    : GuiOverlay(owner, "NAVIGATION_SCREEN", colorConfig.background), mode(TargetSelection)
{
    targets.setAllowWaypointSelection();
    radar = new NavigationView(this, "NAVIGATION_RADAR", 50000.0f, &targets);
    radar->setPosition(0, 0, ATopLeft)->setSize(GuiElement::GuiSizeMax, GuiElement::GuiSizeMax);
    radar->setCallbacks(
        [this](sf::Vector2f position) { //down
            if (mode == TargetSelection && targets.getWaypointIndex() > -1 && my_spaceship)
            {
                if (sf::length(my_spaceship->waypoints[targets.getWaypointIndex()] - position) < 1000.0)
                {
                    mode = MoveWaypoint;
                    drag_waypoint_index = targets.getWaypointIndex();
                }
            }
            mouse_down_position = position;
        },
        [this](sf::Vector2f position) { //drag
            if (mode == TargetSelection)
            {
                sector_name_custom = false;
                sf::Vector2f newPosition = radar->getViewPosition() - (position - mouse_down_position);
                radar->setViewPosition(newPosition);
                if (!sector_name_custom)
                    sector_name_text->setText(getSectorName(newPosition));
            }
            if (mode == MoveWaypoint && my_spaceship)
                my_spaceship->commandMoveWaypoint(drag_waypoint_index, position);
        },
        [this](sf::Vector2f position) { //up
            switch (mode)
            {
            case TargetSelection:
                targets.setToClosestTo(position, 1000, TargetsContainer::Targetable);
                break;
            case WaypointPlacement:
                if (my_spaceship)
                    my_spaceship->commandAddWaypoint(position);
                mode = TargetSelection;
                option_buttons->show();
                break;
            case MoveWaypoint:
                mode = TargetSelection;
                targets.setWaypointIndex(drag_waypoint_index);
                break;
            }
        });

    if (my_spaceship)
        radar->setViewPosition(my_spaceship->getPosition());

    // TODO terrain data
    // GuiAutoLayout* sidebar = new GuiAutoLayout(this, "SIDE_BAR", GuiAutoLayout::LayoutVerticalTopToBottom);
    // sidebar->setPosition(-20, 150, ATopRight)->setSize(250, GuiElement::GuiSizeMax);

    // info_callsign = new GuiKeyValueDisplay(sidebar, "SCIENCE_CALLSIGN", 0.4, "Callsign", "");
    // info_callsign->setSize(GuiElement::GuiSizeMax, 30);

    // info_faction = new GuiKeyValueDisplay(sidebar, "SCIENCE_FACTION", 0.4, "Faction", "");
    // info_faction->setSize(GuiElement::GuiSizeMax, 30);

    // Controls for the radar view
    view_controls = new GuiAutoLayout(this, "VIEW_CONTROLS", GuiAutoLayout::LayoutVerticalBottomToTop);
    view_controls->setPosition(20, -70, ABottomLeft)->setSize(250, GuiElement::GuiSizeMax);
    zoom_slider = new GuiSlider(view_controls, "ZOOM_SLIDER", max_distance, min_distance, radar->getDistance(), [this](float value) {
        zoom_label->setText("Zoom: " + string(max_distance / value, 1.0f) + "x");
        radar->setDistance(value);
    });
    zoom_slider->setPosition(20, -70, ABottomLeft)->setSize(GuiElement::GuiSizeMax, 50);
    zoom_label = new GuiLabel(zoom_slider, "", "Zoom: " + string(max_distance / radar->getDistance(), 1.0f) + "x", 30);
    zoom_label->setSize(GuiElement::GuiSizeMax, GuiElement::GuiSizeMax);

    sector_name_custom = false;
    sector_name_text = new GuiTextEntry(view_controls, "SECTOR_NAME_TEXT", "");
    sector_name_text->setSize(GuiElement::GuiSizeMax, 50);
    sector_name_text->callback([this](string text) {
        sector_name_custom = true;
    });
    sector_name_text->validator(isValidSectorName);
    sector_name_text->enterCallback([this](string text) {
        sector_name_custom = false;
        if (sector_name_text->isValid())
        {
            sf::Vector2f pos = getSectorPosition(text);
            radar->setViewPosition(pos);
        }
    });
    sector_name_text->setText(getSectorName(radar->getViewPosition()));
    // Option buttons for comms, waypoints, and probes.
    option_buttons = new GuiAutoLayout(this, "BUTTONS", GuiAutoLayout::LayoutVerticalTopToBottom);
    option_buttons->setPosition(20, 50, ATopLeft)->setSize(250, GuiElement::GuiSizeMax);

    // Manage waypoints.
    (new GuiButton(option_buttons, "WAYPOINT_PLACE_BUTTON", "Place Waypoint", [this]() {
        mode = WaypointPlacement;
        option_buttons->hide();
    }))
        ->setSize(GuiElement::GuiSizeMax, 50);

    delete_waypoint_button = new GuiButton(option_buttons, "WAYPOINT_DELETE_BUTTON", "Delete Waypoint", [this]() {
        if (my_spaceship && targets.getWaypointIndex() >= 0)
        {
            my_spaceship->commandRemoveWaypoint(targets.getWaypointIndex());
        }
    });
    delete_waypoint_button->setSize(GuiElement::GuiSizeMax, 50);

    (new GuiCustomShipFunctions(this, navigator, ""))->setPosition(-20, 240, ATopRight)->setSize(250, GuiElement::GuiSizeMax);
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
}
