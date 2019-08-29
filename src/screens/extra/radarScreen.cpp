#include "radarScreen.h"
#include "playerInfo.h"
#include "gameGlobalInfo.h"
#include "spaceObjects/playerSpaceship.h"

#include "screenComponents/indicatorOverlays.h"
#include "screenComponents/selfDestructIndicator.h"
#include "screenComponents/globalMessage.h"
#include "screenComponents/jumpIndicator.h"
#include "screenComponents/commsOverlay.h"
#include "screenComponents/shipDestroyedPopup.h"

#include "screenComponents/radarView.h"
#include "screenComponents/alertOverlay.h"
#include "gui/gui2_overlay.h"

RadarScreen::RadarScreen(GuiContainer* owner,string type)
: GuiOverlay(owner, "RADAR_SCREEN", colorConfig.background)
{

    if (type == "tactical"){
      tactical_radar = new GuiRadarView(this, "TACTICAL", 5000.0f, nullptr, my_spaceship);
      tactical_radar->setPosition(0, 0, ATopLeft)->setSize(GuiElement::GuiSizeMax, GuiElement::GuiSizeMax);
      tactical_radar->setRangeIndicatorStepSize(1000.0f)->shortRange()->enableCallsigns()->show();
      tactical_radar->setFogOfWarStyle(GuiRadarView::RadarRangeAndLineOfSight);
    } else if (type == "science"){
      science_radar = new GuiRadarView(this, "SCIENCE", gameGlobalInfo->long_range_radar_range, nullptr, my_spaceship);
      science_radar->setPosition(0, 0, ATopLeft)->setSize(GuiElement::GuiSizeMax, GuiElement::GuiSizeMax);
      science_radar->setRangeIndicatorStepSize(5000.0f)->longRange()->enableCallsigns()->show();
      science_radar->setFogOfWarStyle(GuiRadarView::RadarRangeAndLineOfSight);
    } else if (type == "relay"){
      relay_radar = new GuiRadarView(this, "RELAY", 50000.0f, nullptr, my_spaceship);
      relay_radar->setPosition(0, 0, ATopLeft)->setSize(GuiElement::GuiSizeMax, GuiElement::GuiSizeMax);
      relay_radar->setAutoCentering(true);
      relay_radar->longRange()->enableWaypoints()->enableCallsigns()->setStyle(GuiRadarView::Rectangular)->setFogOfWarStyle(GuiRadarView::RadarRangeAndLineOfSight);
      relay_radar->show();
      relay_radar->setFogOfWarStyle(GuiRadarView::RadarRangeAndLineOfSight);
    } 
        
    new GuiJumpIndicator(this);
    new GuiSelfDestructIndicator(this);
    new GuiGlobalMessage(this);
    new GuiIndicatorOverlays(this);
}
