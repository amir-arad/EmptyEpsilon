#ifndef GAME_MASTER_SCREEN_H
#define GAME_MASTER_SCREEN_H

#include "engine.h"
#include "gui/gui2_panel.h"
#include "gui/gui2_scrolltext.h"
#include "gui/gui2_canvas.h"
#include "gui/gui2_overlay.h"
#include "screenComponents/targetsContainer.h"
#include "gameGlobalInfo.h"

class GuiGlobalMessageEntry;
class GuiObjectCreationScreen;
class GuiObjectTweak;
class GuiRadarView;
class GuiOverlay;
class GuiSelector;
class GuiAutoLayout;
class GuiKeyValueDisplay;
class GuiListbox;
class GuiButton;
class GuiToggleButton;
class GuiTextEntry;
class GameMasterChatDialog;
class GuiObjectCreationView;
class GuiGlobalMessageEntryView;
class GuiFactions;
class CpuShip;
class GuiEntryList;
class ActionItemOverlay;

class GameMasterScreen : public GuiCanvas, public Updatable
{
private:
    TargetsContainer targets;
    P<SpaceObject> target;
    P<CpuShip> possession_target;
    int8_t selected_posessed_tube;
    GuiRadarView* main_radar;
    GuiOverlay* box_selection_overlay;
    GuiSelector* faction_selector;
    
    GuiElement* chat_layer;
    std::vector<GameMasterChatDialog*> chat_dialog_per_ship;
    GuiGlobalMessageEntryView* global_message_entry;
    GuiObjectCreationView* object_creation_view;
    GuiObjectTweak* player_tweak_dialog;
    GuiObjectTweak* ship_tweak_dialog;
    GuiObjectTweak* object_tweak_dialog;
    GuiFactions* factions_dialog;

    bool position_text_custom;
    bool managing_layers;
    GuiButton* manageLayersButton;
    GuiButton* layerButtons[GameGlobalInfo::max_map_layers];
    GuiListbox* actionItems;
    ActionItemOverlay* action_item_dialog;
    GuiTextEntry* position_text;
    GuiAutoLayout* info_layout;
    std::vector<GuiKeyValueDisplay*> info_items;
    GuiListbox* gm_script_options;
    GuiAutoLayout* order_layout;
    GuiAutoLayout* layers_layout;
    GuiButton* player_comms_hail;
    GuiButton* global_message_button;
    GuiToggleButton* pause_button;
    GuiToggleButton* intercept_comms_button;
    GuiToggleButton* alerts_button;
    int notification_sound = -1;
    GuiButton* tweak_button;
    GuiButton* factions_button;
    GuiToggleButton* possess_button;
    GuiButton* copy_scenario_button;
    GuiButton* copy_selected_button;
    GuiSelector* player_ship_selector;

    GuiPanel* message_frame;
    GuiScrollText* message_text;
    GuiButton* message_close_button;
    
    enum EClickAndDragState
    {
        CD_None,
        CD_DragViewOrOrder,
        CD_DragView,
        CD_BoxSelect,
        CD_DragObjects
    } click_and_drag_state;
    sf::Vector2f drag_start_position;
    sf::Vector2f drag_previous_position;
    const float max_distance = 10000000.0f;
    const float min_distance = 6250.0f;
public:
    GuiButton* create_button;
    GuiButton* cancel_create_button;

    GameMasterScreen();
    
    virtual void update(float delta);
    
    void onMouseDown(sf::Vector2f position);
    void onMouseDrag(sf::Vector2f position);
    void onMouseUp(sf::Vector2f position);

    virtual void onKey(sf::Event::KeyEvent key, int unicode);
    
    virtual void onHotkey(const HotkeyResult& key) override;
    virtual bool onJoystickAxis(const AxisAction& axisAction) override;

    PVector<SpaceObject> getSelection();
    
    string getScriptExport(bool selected_only);

    void possess(P<CpuShip> target);
    void dePossess();
};


#endif//GAME_MASTER_SCREEN_H
