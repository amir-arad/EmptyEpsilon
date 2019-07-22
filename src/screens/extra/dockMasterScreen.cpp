#include "dockMasterScreen.h"

#include "playerInfo.h"
#include "spaceObjects/shipTemplateBasedObject.h"
#include "spaceObjects/playerSpaceship.h"
#include "screenComponents/customShipFunctions.h"

#include "gui/gui2_listbox.h"
#include "gui/gui2_autolayout.h"
#include "gui/gui2_element.h"
#include "gui/gui2_panel.h"
#include "gui/gui2_label.h"
#include "gui/gui2_keyvaluedisplay.h"
#include "gui/gui2_slider.h"
#include "gui/gui2_progressbar.h"
#include "gui/gui2_button.h"
#include "gui/gui2_selector.h"
#include "gui/gui2_togglebutton.h"
#include "screenComponents/powerDamageIndicator.h"
#include "screenComponents/rotatingModelView.h"
#include "screenComponents/radarView.h"

const int ROW_SIZE = 4;
const int ROW_HEIGHT = 200;
const int BEAM_PANEL_HEIGHT = 290;
const int COLUMN_WIDTH = 400;


GuiTractorBeamControl::GuiTractorBeamControl(GuiContainer* owner, string id): GuiAutoLayout(owner, id, GuiAutoLayout::LayoutVerticalBottomToTop){
    this->setSize(GuiElement::GuiSizeMax, BEAM_PANEL_HEIGHT);

    arc_slider = new GuiSlider(this, "", 0.0, 90.0, 0.0, [this](float value) {
        if (my_spaceship) my_spaceship->commandSetTractorBeamArc(value);
    });
    arc_slider->addOverlay()->setSize(GuiElement::GuiSizeMax, 30);
    (new GuiPowerDamageIndicator(arc_slider, "", SYS_Docks, ATopCenter, my_spaceship))->setSize(GuiElement::GuiSizeMax, GuiElement::GuiSizeMax);
    (new GuiLabel(this, "", "Arc:", 20))->setSize(GuiElement::GuiSizeMax, 30);

    direction_slider = new GuiSlider(this, "", -179.9, 180.0, 0.0, [this](float value) {
        if (my_spaceship) my_spaceship->commandSetTractorBeamDirection(value);
    });
    direction_slider->addOverlay()->setSize(GuiElement::GuiSizeMax, 30);
    (new GuiPowerDamageIndicator(direction_slider, "", SYS_Docks, ATopCenter, my_spaceship))->setSize(GuiElement::GuiSizeMax, GuiElement::GuiSizeMax);
    (new GuiLabel(this, "", "Direction:", 20))->setSize(GuiElement::GuiSizeMax, 30);

    range_slider = new GuiSlider(this, "", 0.0, 2000.0, 0.0, [this](float value) {
        if (my_spaceship) my_spaceship->commandSetTractorBeamRange(value);
    });
    range_slider->addOverlay()->setSize(GuiElement::GuiSizeMax, 30);
    (new GuiPowerDamageIndicator(range_slider, "", SYS_Docks, ATopCenter, my_spaceship))->setSize(GuiElement::GuiSizeMax, GuiElement::GuiSizeMax);
    (new GuiLabel(this, "", "Range:", 20))->setSize(GuiElement::GuiSizeMax, 30);

    mode_slector = new GuiSelector(this, "MODE_SELECTOR", [this](int index, string value) {
        if (my_spaceship)
            my_spaceship->commandSetTractorBeamMode(ETractorBeamMode(index));
    });
    mode_slector->setOptions({"Off", "Pull", "Push", "Hold"});
    mode_slector->setSelectionIndex(0);
    mode_slector->setSize(GuiElement::GuiSizeMax, 30);
}

void GuiTractorBeamControl::onDraw(sf::RenderTarget& window)
{
    GuiAutoLayout::onDraw(window);
    if (my_spaceship){
        mode_slector->setSelectionIndex(int(my_spaceship->tractor_beam.getMode()));
        arc_slider->setValue(my_spaceship->tractor_beam.getArc());
        direction_slider->setValue(sf::angleDifference(0.0f, my_spaceship->tractor_beam.getDirection()));
        range_slider->setValue(my_spaceship->tractor_beam.getRange());
    }
}
void GuiTractorBeamControl::onHotkey(const HotkeyResult& key){
    if (key.category == "TRACTOR_BEAM" && my_spaceship)
    {
        if (key.hotkey == "MODE_OFF")
            my_spaceship->commandSetTractorBeamMode(ETractorBeamMode::TBM_Off);
        else if (key.hotkey == "MODE_PULL")
            my_spaceship->commandSetTractorBeamMode(ETractorBeamMode::TBM_Pull);
        else if (key.hotkey == "MODE_PUSH")
            my_spaceship->commandSetTractorBeamMode(ETractorBeamMode::TBM_Push);
        else if (key.hotkey == "MODE_HOLD")
            my_spaceship->commandSetTractorBeamMode(ETractorBeamMode::TBM_Hold);
        else if (key.hotkey == "MODE")
            my_spaceship->commandSetTractorBeamMode(ETractorBeamMode((my_spaceship->tractor_beam.getMode() + 1) % ETractorBeamMode::TBM_Max));
    }
}

bool GuiTractorBeamControl::onJoystickAxis(const AxisAction& axisAction){
    if(my_spaceship){
        if (axisAction.category == "TRACTOR_BEAM"){
            {
            if (axisAction.action == "ARC"){
                arc_slider->setValue((axisAction.value + 1) * 90.0 / 2.0);
                my_spaceship->commandSetTractorBeamArc(arc_slider->getValue());
                return true;            
            } 
            if (axisAction.action == "DIRECTION"){
                direction_slider->setValue(axisAction.value * 180.0);
                my_spaceship->commandSetTractorBeamDirection(direction_slider->getValue());
                return true;
            } 
            if (axisAction.action == "RANGE"){
                range_slider->setValue((axisAction.value + 1) * 2000.0 / 2.0);
                my_spaceship->commandSetTractorBeamRange(range_slider->getValue());
                return true;
            } 
            }
        }
    }
    return false;
}

DockMasterScreen::DockMasterScreen(GuiContainer *owner)
    : GuiOverlay(owner, "DOCK_MASTER_SCREEN", colorConfig.background)
{
    GuiOverlay *background_crosses = new GuiOverlay(this, "BACKGROUND_CROSSES", sf::Color::White);
    background_crosses->setTextureTiled("gui/BackgroundCrosses");

    GuiAutoLayout *rootLayout = new GuiAutoLayout(this, "ROOT_LAYOUT", GuiAutoLayout::LayoutHorizontalLeftToRight);
    rootLayout->setSize(GuiElement::GuiSizeMax, GuiElement::GuiSizeMax)->setPosition(0, 0, ATopLeft);

    model = new GuiRotatingModelView(this, "MODEL_VIEW", nullptr);
    model->setPosition(0, 0, ATopLeft)->setSize(GuiElement::GuiSizeMax, GuiElement::GuiSizeMax);

    docks = new GuiListbox(rootLayout, "DOCKS_LIST", [this](int index, string value) {
        selectDock(index);
    });
    docks->setMargins(20, 20, 20, 20)->setSize(COLUMN_WIDTH, GuiElement::GuiSizeMax);

    // the index in the button list is assumed to equal the index of the dock
    for (int n = 0; n < max_docks_count; n++)
    {
        if (my_spaceship && my_spaceship->docks[n].dock_type != Dock_Disabled)
        {
            string state = my_spaceship ? " (" + getDockStateName(my_spaceship->docks[n].state) + ")" : "";
            docks->addEntry("dock-" + std::to_string(n + 1) + state, "dock-" + std::to_string(n + 1) + " " + getDockTypeName(my_spaceship->docks[n].dock_type));
        }
    }

    GuiAutoLayout *rightSide = new GuiAutoLayout(rootLayout, "RIGHT_SIDE", GuiAutoLayout::LayoutVerticalTopToBottom);
    rightSide->setSize(GuiElement::GuiSizeMax, GuiElement::GuiSizeMax);
    
    title = new GuiLabel(rightSide, "TITLE", "dock x", 30);
    title->addBackground()
        ->setAlignment(ACenter)
        ->setSize(GuiElement::GuiSizeMax, 50);

    GuiElement *dockDetails = new GuiElement(rightSide, "DOCK_DETAILS");
    dockDetails->setSize(GuiElement::GuiSizeMax, 500);

    GuiAutoLayout *transferBar = new GuiAutoLayout(dockDetails, "TRANSFER_BAR", GuiAutoLayout::LayoutVerticalColumns);
    transferBar->setSize(GuiElement::GuiSizeMax, 50)->setPosition(0, 0, ATopCenter);

    GuiElement *move_dest = new GuiAutoLayout(transferBar, "", GuiAutoLayout::LayoutVerticalColumns);
    (new GuiLabel(move_dest, "MOVE_DEST_LABEL", "Destination:", 30))->setAlignment(ACenterRight);
    move_dest_selector = new GuiSelector(move_dest, "MOVE_DEST_SELECTOR", [this](int _idx, string value) {
        if (my_spaceship)
            my_spaceship->commandSetDockMoveTarget(index, value.toInt());
    });

    GuiButton *moveButton = new GuiButton(transferBar, "MOVE_BUTTON", "Deliver", [this]() {
        if (my_spaceship)
            my_spaceship->commandMoveCargo(index);
    });
    moveButton->setSize(COLUMN_WIDTH, 40);
    (new GuiPowerDamageIndicator(moveButton, "", SYS_Docks, ATopCenter, my_spaceship))->setSize(GuiElement::GuiSizeMax, GuiElement::GuiSizeMax);

    GuiElement *cargoViewParent = new GuiElement(dockDetails, "");
    cargoViewParent->setSize(GuiElement::GuiSizeMax, GuiElement::GuiSizeMax)->setPosition(0, 50, ATopRight);

    cargoView = new GuiAutoLayout(cargoViewParent, "CARGO_VIEW", GuiAutoLayout::LayoutHorizontalRightToLeft);
    cargoView->setSize(GuiElement::GuiSizeMax, GuiElement::GuiSizeMax)->setPosition(0, 0, ATopRight);

    cargoInfo = new GuiAutoLayout(cargoView, "CARGO_INFO", GuiAutoLayout::LayoutVerticalTopToBottom);
    cargoInfo->setSize(COLUMN_WIDTH, GuiElement::GuiSizeMax);

    GuiAutoLayout *cargoActions = new GuiAutoLayout(cargoView, "CARGO_ACTIONS", GuiAutoLayout::LayoutVerticalTopToBottom);
    cargoActions->setSize(COLUMN_WIDTH, GuiElement::GuiSizeMax)->setPosition(0, 100, ATopRight);

    launch_button = new GuiButton(cargoActions, "LAUNCH_DRONE_BUTTON", "Launch", [this]() {
        if (my_spaceship)
            my_spaceship->commandLaunchCargo(index);
    });
    launch_button->setSize(COLUMN_WIDTH, 40);

    GuiElement *energyControl = new GuiElement(cargoActions, "ENERGY_CONTROL");
    energyControl->setSize(COLUMN_WIDTH, 40);

    energy_slider = new GuiSlider(energyControl, "ENERGY_SLIDER", 0.0, 10.0, 0.0, [this](float value) {
        if (my_spaceship)
            my_spaceship->commandSetDockEnergyRequest(index, value);
    });
    energy_slider->setSize(GuiElement::GuiSizeMax, GuiElement::GuiSizeMax);

    energy_bar = new GuiProgressbar(energyControl, "ENERGY_BAR", 0.0, 10.0, 0.0);
    energy_bar->setColor(sf::Color(192, 192, 32, 128))->setText("Energy")->setDrawBackground(false)->setSize(GuiElement::GuiSizeMax, GuiElement::GuiSizeMax)->setMargins(10, 0, 10, 0);
    (new GuiPowerDamageIndicator(energy_bar, "", SYS_Docks, ATopCenter, my_spaceship))->setSize(GuiElement::GuiSizeMax, GuiElement::GuiSizeMax);

    GuiAutoLayout *tacticalPanel = new GuiAutoLayout(rightSide, "TACTICAL_PANEL", GuiAutoLayout::LayoutHorizontalRightToLeft);
    tacticalPanel->setSize(GuiElement::GuiSizeMax, GuiElement::GuiSizeMax);

    // 5U tactical radar with piloting features.
    GuiRadarView *radar = new GuiRadarView(tacticalPanel, "TACTICAL_RADAR", 2000.0, nullptr, my_spaceship);
    radar->setSize(BEAM_PANEL_HEIGHT, BEAM_PANEL_HEIGHT);
    radar->setRangeIndicatorStepSize(1000.0)->shortRange()->enableCallsigns()->enableHeadingIndicators()->setStyle(GuiRadarView::Circular);
    GuiTractorBeamControl *beam_control = new GuiTractorBeamControl(tacticalPanel, "BEAM_CONFIG");
    beam_control->setSize(BEAM_PANEL_HEIGHT, BEAM_PANEL_HEIGHT);
    
    (new GuiCustomShipFunctions(this, dockMaster, "CUSTOM_FUNCTIONS", my_spaceship))->setPosition(-20, 120, ATopRight)->setSize(250, GuiElement::GuiSizeMax);

    overlay = new GuiOverlay(cargoViewParent, "OVERLAY", sf::Color(0, 0, 0, 128));
    overlay->setBlocking(true)->setPosition(0, 0, ATopLeft)->setSize(GuiElement::GuiSizeMax, GuiElement::GuiSizeMax);
    overlay_label = new GuiLabel(overlay, "OVERLAY_LABEL", "Transporting cargo out", 30);
    overlay_label->setPosition(0, 0, ACenter)->setSize(COLUMN_WIDTH, 50);
    distance_bar = new GuiProgressbar(overlay, "DISTANCE_BAR", 0.0, 1.0, 0.0);
    distance_bar->setPosition(0, 50, ACenter)->setSize(COLUMN_WIDTH, 50);
    (new GuiPowerDamageIndicator(distance_bar, "", SYS_Docks, ATopCenter, my_spaceship))->setSize(GuiElement::GuiSizeMax, GuiElement::GuiSizeMax);

    cancel_move_button = new GuiButton(overlay, "CANCEL_MOVE_BUTTON", "pull cargo back", [this]() {
        if(my_spaceship) my_spaceship->commandCancelMoveCargo(index);
    });
    cancel_move_button->setPosition(0, 100, ACenter)->setSize(COLUMN_WIDTH, 50);

    selectDock(0);
    model->moveToBack();
    background_crosses->moveToBack();
}

void DockMasterScreen::selectDock(int index)
{
    if (my_spaceship) {
        title->setText(docks->getEntryValue(index));
        this->index = index;
        docks->setSelectionIndex(index);
        auto &dockData = my_spaceship->docks[index];
        launch_button->setVisible(dockData.dock_type == Dock_Launcher);
        energy_bar->setVisible(dockData.dock_type == Dock_Energy);
        energy_slider->setVisible(dockData.dock_type == Dock_Energy);
    }
}

void DockMasterScreen::onDraw(sf::RenderTarget &window)
{
    GuiOverlay::onDraw(window);
    if (my_spaceship)
    {
        move_dest_selector->setOptions({});
        for (int n = 0; n < max_docks_count; n++)
        {
            Dock &dockData = my_spaceship->docks[n];
            if (dockData.dock_type != Dock_Disabled)
            {
                string state = " (" + getDockStateName(dockData.state) + ")";
                string dockName = "d" + std::to_string(n + 1) + "-" + getDockTypeName(dockData.dock_type)[0];
                docks->setEntryName(n, dockName + state);
                if (n != index)
                    move_dest_selector->addEntry(dockName, string(n));
            }
        }

        Dock &dockData = my_spaceship->docks[index];
        move_dest_selector->setSelectionIndex(move_dest_selector->indexByValue(string(dockData.move_target_index)));

        switch (dockData.state)
        {
        case Empty:
            cargoView->setVisible(false);
            model->setModel(nullptr);
            overlay->setVisible(true);
            overlay_label->setText("Empty");
            distance_bar->setVisible(false);
            cancel_move_button->setVisible(false);
            break;
        case MovingIn:
            displayDroneDetails(dockData);
            overlay->setVisible(true);
            overlay_label->setText("Incoming cargo");
            distance_bar->setVisible(true);
            distance_bar->setValue(dockData.current_distance);
            cancel_move_button->setVisible(false);
            break;
        case Docked:
            displayDroneDetails(dockData);
            overlay->setVisible(false);
            break;
        case MovingOut:
            displayDroneDetails(dockData);
            overlay->setVisible(true);
            overlay_label->setText("Outgoing cargo");
            distance_bar->setVisible(true);
            distance_bar->setValue(dockData.current_distance);
            cancel_move_button->setVisible(true);
            break;
        }
    }
}

void DockMasterScreen::displayDroneDetails(Dock &dockData)
{
    P<Cargo> cargo = dockData.getCargo();
    cargoView->setVisible(true);

    unsigned int cnt = 0;
    for(std::tuple<string, string, string> e : cargo->getEntries())
    {
        if (cnt == cargoInfoItems.size())
        {
            cargoInfoItems.push_back(new GuiKeyValueDisplay(cargoInfo, "INFO_" + string(cnt), 0.5, "", ""));
            cargoInfoItems[cnt]->setSize(COLUMN_WIDTH, 40);
        }else{
            cargoInfoItems[cnt]->show();
        }
        cargoInfoItems[cnt]->setIcon(std::get<0>(e))->setKey(std::get<1>(e))->setValue(std::get<2>(e));
        cnt++;
    }
    while(cnt < cargoInfoItems.size())
    {
        cargoInfoItems[cnt]->hide();
        cnt++;
    }

    energy_bar->setValue(cargo->getEnergy());
    energy_bar->setRange(cargo->getMinEnergy(), cargo->getMaxEnergy());
    energy_slider->setRange(cargo->getMinEnergy(), cargo->getMaxEnergy());
    energy_slider->setValue(dockData.energy_request);
    model->setModel(cargo->getModel());
}
