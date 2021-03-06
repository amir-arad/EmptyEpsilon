#include "hotkeyConfig.h"
#include "preferenceManager.h"
#include "shipTemplate.h"
#include "gameGlobalInfo.h"

HotkeyConfig hotkeys;

string digit(string prefix, int number){
    return number > 0 && number < 10 ? prefix + string(number) : "";
}
HotkeyConfig::HotkeyConfig()
{  // this list includes all Hotkeys and their standard configuration
    newCategory("GENERAL", "General");
    newKey("NEXT_STATION", std::make_tuple("Switch to next crew station", "Tab"));
    newKey("PREV_STATION", std::make_tuple("Switch to previous crew station", ""));
    newKey("STATION_HELMS", std::make_tuple("Switch to helms station", "F2"));
    newKey("STATION_WEAPONS", std::make_tuple("Switch to weapons station", "F3"));
    newKey("STATION_ENGINEERING", std::make_tuple("Switch to engineering station", "F4"));
    newKey("STATION_SCIENCE", std::make_tuple("Switch to science station", "F5"));
    newKey("STATION_RELAY", std::make_tuple("Switch to relay station", "F6"));
    
    newCategory("HELMS", "Helms");
    newKey("INC_IMPULSE", std::make_tuple("Increase impulse", "Up"));
    newKey("DEC_IMPULSE", std::make_tuple("Decrease impulse", "Down"));
    newKey("ZERO_IMPULSE", std::make_tuple("Zero impulse", "Space"));
    newKey("MAX_IMPULSE", std::make_tuple("Max impulse", ""));
    newKey("MIN_IMPULSE", std::make_tuple("Max reverse impulse", ""));
    newKey("TURN_LEFT", std::make_tuple("Turn left", "Left"));
    newKey("TURN_RIGHT", std::make_tuple("Turn right", "Right"));
    newKey("WARP_0", std::make_tuple("Warp off", ""));
    newKey("WARP_1", std::make_tuple("Warp 1", ""));
    newKey("WARP_2", std::make_tuple("Warp 2", ""));
    newKey("WARP_3", std::make_tuple("Warp 3", ""));
    newKey("WARP_4", std::make_tuple("Warp 4", ""));
    newKey("DOCK_ACTION", std::make_tuple("Dock request/abort/undock", "D"));
    newKey("DOCK_REQUEST", std::make_tuple("Initiate docking", ""));
    newKey("DOCK_ABORT", std::make_tuple("Abort docking", ""));
    newKey("UNDOCK", std::make_tuple("Undock", "D"));
    newKey("INC_JUMP", std::make_tuple("Increase jump distance", ""));
    newKey("DEC_JUMP", std::make_tuple("Decrease jump distance", ""));
    newKey("JUMP", std::make_tuple("Initiate jump", ""));
    newKey("COMBAT_LEFT", std::make_tuple("Combat maneuver left", "[alt];Left"));
    newKey("COMBAT_RIGHT", std::make_tuple("Combat maneuver right", "[alt];Right"));
    newKey("COMBAT_BOOST", std::make_tuple("Combat maneuver boost", "[alt];Up"));
    newKey("COMBAT_BACK", std::make_tuple("Combat maneuver reverse", "[alt];Down"));

    newCategory("SCIENCE", "Science Officer");
    newKey("ZOOM_IN", std::make_tuple("increase radar zoom", "9"));
    newKey("ZOOM_OUT", std::make_tuple("decrease radar zoom", "0"));
    newKey("POV_SHIP", std::make_tuple("set radar point of view to main ship", "Q"));
    newKey("POV_PROBE", std::make_tuple("set radar point of view to probe", "A"));
    newKey("CLEAR_TASKS", std::make_tuple("abort all tasks", "[alt];C"));
    newKey("SCAN", std::make_tuple("Scan target", "S"));
    newKey("OPEN_TYPE_IN_DB", std::make_tuple("Open selected ship type in DB", "D"));
    for(int n=0; n<SYS_COUNT; n++) {
        string systemName = getSystemName(ESystem(n));
        newKey(std::string("HACK_") + systemName, std::make_tuple(std::string("Hack ") + systemName, string(n, 0)));
    }

    newCategory("SCIENCE_TASK", "Science Task");
    newKey("SIMULATE_HACK", std::make_tuple("simulate hack task", "[alt];F11"));
    newKey("SIMULATE_SCAN", std::make_tuple("simulate scan task", "[alt];F12"));
    newKey("ABORT_TASK", std::make_tuple("abort task", "[alt];Q"));
    for(int n = 0; n < PlayerSpaceship::max_science_tasks; n++){
        newKey("DO_TASK_" + string(n, 0), std::make_tuple("Execute task "+ string(n+1, 0), string((n+1) % 10, 0)));
    }

    newCategory("TARGET", "Target");
    newKey("NEXT_ENEMY_TARGET", std::make_tuple("Select next enemy target", ""));
    newKey("PREV_ENEMY_TARGET", std::make_tuple("Select previous enemy target", ""));
    newKey("NEXT_TARGET", std::make_tuple("Select next target", "RBracket"));
    newKey("PREV_TARGET", std::make_tuple("Select previous target", "LBracket"));
    newKey("TARGET_NEAR_TOGGLE", std::make_tuple("select only near targets", "[alt];RBracket"));
    newKey("TARGET_ENEMY_TOGGLE", std::make_tuple("select only enemy targets", "[alt];LBracket"));
    newKey("TARGET_WAYPOINTS_TOGGLE", std::make_tuple("select only waypoints", "W"));
    
    newCategory("WEAPONS", "Weapons");
    for(int n=0; n<MW_Count; n++)
        newKey("SELECT_MISSILE_TYPE_" + getMissileWeaponName(EMissileWeapons(n)).upper(), 
            std::make_tuple("Select " + getMissileWeaponName(EMissileWeapons(n)), digit("", n+1)));
    for(int n=0; n<max_weapon_tubes; n++)
        newKey(std::string("LOAD_TUBE_") + string(n+1), std::make_tuple(std::string("Load tube ") + string(n+1), digit("[alt];", n+1)));
    for(int n=0; n<max_weapon_tubes; n++)
        newKey(std::string("UNLOAD_TUBE_") + string(n+1), std::make_tuple(std::string("Unload tube ") + string(n+1), digit("[alt];[shift];", n+1)));
    for(int n=0; n<max_weapon_tubes; n++)
        newKey(std::string("FIRE_TUBE_") + string(n+1), std::make_tuple(std::string("Fire tube ") + string(n+1), digit("[alt];", n+1)));

    newKey("TOGGLE_SHIELDS", std::make_tuple("Toggle shields", "S"));
    newKey("ENABLE_SHIELDS", std::make_tuple("Enable shields", ""));
    newKey("DISABLE_SHIELDS", std::make_tuple("Disable shields", ""));
    newKey("BEAM_SUBSYSTEM_TARGET_NEXT", std::make_tuple("Next beam subsystem target type", "[alt];Up"));
    newKey("BEAM_SUBSYSTEM_TARGET_PREV", std::make_tuple("Previous beam subsystem target type", "[alt];Down"));
    newKey("BEAM_FREQUENCY_INCREASE", std::make_tuple("Increase beam frequency", "[alt];[shift];Up"));
    newKey("BEAM_FREQUENCY_DECREASE", std::make_tuple("Decrease beam frequency", "[alt];[shift];Down"));
    newKey("TOGGLE_AIM_LOCK", std::make_tuple("Toggle missile aim lock", ""));
    newKey("ENABLE_AIM_LOCK", std::make_tuple("Enable missile aim lock", ""));
    newKey("DISABLE_AIM_LOCK", std::make_tuple("Disable missile aim lock", ""));
    newKey("AIM_MISSILE_LEFT", std::make_tuple("Turn missile aim to the left", ""));
    newKey("AIM_MISSILE_RIGHT", std::make_tuple("Turn missile aim to the right", ""));
    
    newCategory("ENGINEERING", "Engineering");
    newKey("SELECT_REACTOR", std::make_tuple("Select reactor system", "Num1"));
    newKey("SELECT_BEAM_WEAPONS", std::make_tuple("Select beam weapon system", "Num2"));
    newKey("SELECT_MISSILE_SYSTEM", std::make_tuple("Select missile weapon system", "Num3"));
    newKey("SELECT_MANEUVER", std::make_tuple("Select maneuvering system", "Num4"));
    newKey("SELECT_IMPULSE", std::make_tuple("Select impulse system", "Num5"));
    newKey("SELECT_WARP", std::make_tuple("Select warp system", "Num6"));
    newKey("SELECT_JUMP_DRIVE", std::make_tuple("Select jump drive system", "Num7"));
    newKey("SELECT_FRONT_SHIELDS", std::make_tuple("Select front shields system", "Num8"));
    newKey("SELECT_REAR_SHIELDS", std::make_tuple("Select rear shields system", "Num9"));
    newKey("INCREASE_POWER", std::make_tuple("Increase system power", "Up"));
    newKey("DECREASE_POWER", std::make_tuple("Decrease system power", "Down"));
    newKey("INCREASE_COOLANT", std::make_tuple("Increase system coolant", "Right"));
    newKey("DECREASE_COOLANT", std::make_tuple("Decrease system coolant", "Left"));
    newKey("NEXT_REPAIR_CREW", std::make_tuple("Next repair crew", ""));
    newKey("REPAIR_CREW_MOVE_UP", std::make_tuple("Crew move up", ""));
    newKey("REPAIR_CREW_MOVE_DOWN", std::make_tuple("Crew move down", ""));
    newKey("REPAIR_CREW_MOVE_LEFT", std::make_tuple("Crew move left", ""));
    newKey("REPAIR_CREW_MOVE_RIGHT", std::make_tuple("Crew move right", ""));
    newKey("SHIELD_CAL_INC", std::make_tuple("Increase shield frequency target", ""));
    newKey("SHIELD_CAL_DEC", std::make_tuple("Decrease shield frequency target", ""));
    newKey("SHIELD_CAL_START", std::make_tuple("Start shield calibration", ""));
    newKey("SELF_DESTRUCT_START", std::make_tuple("Start self-destruct", ""));
    newKey("SELF_DESTRUCT_CONFIRM", std::make_tuple("Confirm self-destruct", ""));
    newKey("SELF_DESTRUCT_CANCEL", std::make_tuple("Cancel self-destruct", ""));
    newKey("REPAIR_NONE", std::make_tuple("Stop Auto-repair", ""));
    for(int n=0; n<SYS_COUNT; n++) {
        string systemName = getSystemName(ESystem(n));
        newKey(std::string("REPAIR_") + systemName, std::make_tuple(std::string("Auto-repair ") + systemName, ""));
    }
    newKey("SET_CONTROL_BRIDGE", std::make_tuple("Set control of engineering to bridge", ""));
    newKey("SET_CONTROL_ECR", std::make_tuple("Set control of engineering to ECR", ""));
    newKey("WARP_CAL_INC", std::make_tuple("Increase warp frequency target", ""));
    newKey("WARP_CAL_DEC", std::make_tuple("Decrease warp frequency target", ""));
    newKey("WARP_CAL_START", std::make_tuple("Start warp calibration", ""));

    newCategory("TRACTOR_BEAM", "Tractor Beam");
    newKey("MODE", std::make_tuple("Toggle tractor beam mode", ""));
    newKey("MODE_OFF", std::make_tuple("Switch tractor beam off", "1"));
    newKey("MODE_PULL", std::make_tuple("Switch tractor beam to pull mode", "2"));
    newKey("MODE_PUSH", std::make_tuple("Switch tractor beam to push mode", "3"));
    newKey("MODE_HOLD", std::make_tuple("Switch tractor beam to hold mode", "4"));

    newCategory("GM", "Game Master");
    newKey("DESTROY", std::make_tuple("Destroy delected targets", "Delete"));
    newKey("COPY_SCENARIO", std::make_tuple("Copy scenario to clipboard", "F5"));
    newKey("TOGGLE_PAUSE", std::make_tuple("Toggle Pause/unpause", "P"));
    newKey("FIRE_TUBE", std::make_tuple("Fire selected Tube", "[B0]"));
    newKey("PREV_TUBE", std::make_tuple("Select previous tube to fire", "[B4]"));
    newKey("NEXT_TUBE", std::make_tuple("Select next tube to fire", "[B6]"));
    newKey("TOGGLE_MANAGE_LAYERS", std::make_tuple("Toggle layers controls", "L"));
    
    newCategory("NAVIGATION", "Navigation");
    newKey("PLACE_WAYPOINTS", std::make_tuple("Place waypoints using mouse", "Q"));
    newKey("WAYPOINT_PLACE_AT_CENTER", std::make_tuple("Place waypoint at center of the screen", "W"));
    for (int n = 0; n < GameGlobalInfo::max_map_layers; n++){
        newKey("LAYER_TOGGLE_" + string(n, 0), std::make_tuple("toggle layer "+ string(n, 0), "Num" + string(n, 0)));
    }
    newKey("WAYPOINT_DELETE", std::make_tuple("Delete selected waypoint", "Delete"));

    newCategory("DRONE_OPERATOR", "Drone Operator");
    newKey("DISCONNECT", std::make_tuple("disconnect from drone", "[B5]"));
    newKey("CONNECT", std::make_tuple("connect to drone", "[B5]"));
    newKey("PREV_DRONE", std::make_tuple("Select previous drone to connect", "[B4]"));
    newKey("NEXT_DRONE", std::make_tuple("Select next drone to connect", "[B6]"));
    
    
    newCategory("RELAY", "Relay Officer");
    for(int level=AL_Normal; level < AL_MAX; level++){
        string alertname = alertLevelToString(EAlertLevel(level));
        newKey(std::string("SET_ALERT_") + alertname, std::make_tuple(std::string("set alert level ") + alertname, string(level, 0)));
    }
}

static std::vector<std::pair<string, sf::Keyboard::Key> > sfml_key_names = {
    {"A", sf::Keyboard::A},
    {"B", sf::Keyboard::B},
    {"C", sf::Keyboard::C},
    {"D", sf::Keyboard::D},
    {"E", sf::Keyboard::E},
    {"F", sf::Keyboard::F},
    {"G", sf::Keyboard::G},
    {"H", sf::Keyboard::H},
    {"I", sf::Keyboard::I},
    {"J", sf::Keyboard::J},
    {"K", sf::Keyboard::K},
    {"L", sf::Keyboard::L},
    {"M", sf::Keyboard::M},
    {"N", sf::Keyboard::N},
    {"O", sf::Keyboard::O},
    {"P", sf::Keyboard::P},
    {"Q", sf::Keyboard::Q},
    {"R", sf::Keyboard::R},
    {"S", sf::Keyboard::S},
    {"T", sf::Keyboard::T},
    {"U", sf::Keyboard::U},
    {"V", sf::Keyboard::V},
    {"W", sf::Keyboard::W},
    {"X", sf::Keyboard::X},
    {"Y", sf::Keyboard::Y},
    {"Z", sf::Keyboard::Z},
    {"Num0", sf::Keyboard::Num0},
    {"Num1", sf::Keyboard::Num1},
    {"Num2", sf::Keyboard::Num2},
    {"Num3", sf::Keyboard::Num3},
    {"Num4", sf::Keyboard::Num4},
    {"Num5", sf::Keyboard::Num5},
    {"Num6", sf::Keyboard::Num6},
    {"Num7", sf::Keyboard::Num7},
    {"Num8", sf::Keyboard::Num8},
    {"Num9", sf::Keyboard::Num9},
    {"0", sf::Keyboard::Num0},
    {"1", sf::Keyboard::Num1},
    {"2", sf::Keyboard::Num2},
    {"3", sf::Keyboard::Num3},
    {"4", sf::Keyboard::Num4},
    {"5", sf::Keyboard::Num5},
    {"6", sf::Keyboard::Num6},
    {"7", sf::Keyboard::Num7},
    {"8", sf::Keyboard::Num8},
    {"9", sf::Keyboard::Num9},
    {"Escape", sf::Keyboard::Escape},
    {"LControl", sf::Keyboard::LControl},
    {"LShift", sf::Keyboard::LShift},
    {"LAlt", sf::Keyboard::LAlt},
    {"LSystem", sf::Keyboard::LSystem},
    {"RControl", sf::Keyboard::RControl},
    {"RShift", sf::Keyboard::RShift},
    {"RAlt", sf::Keyboard::RAlt},
    {"RSystem", sf::Keyboard::RSystem},
    {"Menu", sf::Keyboard::Menu},
    {"LBracket", sf::Keyboard::LBracket},
    {"RBracket", sf::Keyboard::RBracket},
    {"SemiColon", sf::Keyboard::SemiColon},
    {"Comma", sf::Keyboard::Comma},
    {"Period", sf::Keyboard::Period},
    {"Quote", sf::Keyboard::Quote},
    {"Slash", sf::Keyboard::Slash},
    {"BackSlash", sf::Keyboard::BackSlash},
    {"Tilde", sf::Keyboard::Tilde},
    {"Equal", sf::Keyboard::Equal},
    {"Dash", sf::Keyboard::Dash},
    {"Space", sf::Keyboard::Space},
    {"Return", sf::Keyboard::Return},
    {"BackSpace", sf::Keyboard::BackSpace},
    {"Tab", sf::Keyboard::Tab},
    {"PageUp", sf::Keyboard::PageUp},
    {"PageDown", sf::Keyboard::PageDown},
    {"End", sf::Keyboard::End},
    {"Home", sf::Keyboard::Home},
    {"Insert", sf::Keyboard::Insert},
    {"Delete", sf::Keyboard::Delete},
    {"Add", sf::Keyboard::Add},
    {"Subtract", sf::Keyboard::Subtract},
    {"Multiply", sf::Keyboard::Multiply},
    {"Divide", sf::Keyboard::Divide},
    {"Left", sf::Keyboard::Left},
    {"Right", sf::Keyboard::Right},
    {"Up", sf::Keyboard::Up},
    {"Down", sf::Keyboard::Down},
    {"Numpad0", sf::Keyboard::Numpad0},
    {"Numpad1", sf::Keyboard::Numpad1},
    {"Numpad2", sf::Keyboard::Numpad2},
    {"Numpad3", sf::Keyboard::Numpad3},
    {"Numpad4", sf::Keyboard::Numpad4},
    {"Numpad5", sf::Keyboard::Numpad5},
    {"Numpad6", sf::Keyboard::Numpad6},
    {"Numpad7", sf::Keyboard::Numpad7},
    {"Numpad8", sf::Keyboard::Numpad8},
    {"Numpad9", sf::Keyboard::Numpad9},
    {"F1", sf::Keyboard::F1},
    {"F2", sf::Keyboard::F2},
    {"F3", sf::Keyboard::F3},
    {"F4", sf::Keyboard::F4},
    {"F5", sf::Keyboard::F5},
    {"F6", sf::Keyboard::F6},
    {"F7", sf::Keyboard::F7},
    {"F8", sf::Keyboard::F8},
    {"F9", sf::Keyboard::F9},
    {"F10", sf::Keyboard::F10},
    {"F11", sf::Keyboard::F11},
    {"F12", sf::Keyboard::F12},
    {"F13", sf::Keyboard::F13},
    {"F14", sf::Keyboard::F14},
    {"F15", sf::Keyboard::F15},
    {"Pause", sf::Keyboard::Pause},
};

void HotkeyConfig::load()
{
    for(HotkeyConfigCategory& cat : categories)
    {
        for(HotkeyConfigItem& item : cat.hotkeys)
        {
            string key_config = PreferencesManager::get(std::string("HOTKEY.") + cat.key + "." + item.key, std::get<1>(item.value));
            item.load(key_config);
        }
    }
}

std::vector<HotkeyResult> HotkeyConfig::getHotkey(sf::Event::KeyEvent key)
{
    std::vector<HotkeyResult> results;
    for(HotkeyConfigCategory& cat : categories)
    {
        for(HotkeyConfigItem& item : cat.hotkeys)
        {
            if (item.hotkey.code == key.code && item.hotkey.alt == key.alt && item.hotkey.control == key.control && item.hotkey.shift == key.shift && item.hotkey.system == key.system)
            {
                results.emplace_back(cat.key, item.key);
            }
        }
    }
    return results;
}

void HotkeyConfig::newCategory(string key, string name)
{
    categories.emplace_back();
    categories.back().key = key;
    categories.back().name = name;
}

void HotkeyConfig::newKey(string key, std::tuple<string, string> value)
{
    categories.back().hotkeys.emplace_back(key, value);
}

std::vector<string> HotkeyConfig::getCategories()
{
    // Initialize return value.
    std::vector<string> ret;

    // Add each category to the return value.
    for(HotkeyConfigCategory& cat : categories)
    {
        ret.push_back(cat.name);
    }

    return ret;
}

std::vector<std::pair<string, string>> HotkeyConfig::listHotkeysByCategory(string hotkey_category)
{
    std::vector<std::pair<string, string>> ret;

    for(HotkeyConfigCategory& cat : categories)
    {
        if (cat.name == hotkey_category)
        {
            for(HotkeyConfigItem& item : cat.hotkeys)
            {
                for(auto key_name : sfml_key_names)
                {
                    if (key_name.second == item.hotkey.code){
			string keyModifier = "";
			if (item.hotkey.shift) {
				keyModifier = "Shift+";
			} else if (item.hotkey.control) {
				keyModifier = "Ctrl+";
			} else if (item.hotkey.alt){
				keyModifier = "Alt+";
			}
                        ret.push_back({std::get<0>(item.value), keyModifier + key_name.first});
                    }
		}
            }
        }
    }

    return ret;
}

HotkeyConfigItem::HotkeyConfigItem(string key, std::tuple<string, string> value)
{
    this->key = key;
    this->value = value;
    hotkey.code = sf::Keyboard::KeyCount;
    hotkey.alt = false;
    hotkey.control = false;
    hotkey.shift = false;
    hotkey.system = false;
}

void HotkeyConfigItem::load(string key_config)
{
    for(const string& config : key_config.split(";"))
    {
        if (config == "[alt]")
            hotkey.alt = true;
        else if (config == "[control]")
            hotkey.control = true;
        else if (config == "[shift]")
            hotkey.shift = true;
        else if (config == "[system]")
            hotkey.system = true;
        else
        {
            for(auto key_name : sfml_key_names)
            {
                if (key_name.first == config)
                {
                    hotkey.code = key_name.second;
                    break;
                }
            }
        }
    }
}
