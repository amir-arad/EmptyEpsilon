#ifndef PLAYER_SPACESHIP_H
#define PLAYER_SPACESHIP_H

#include "spaceship.h"
#include "commsScriptInterface.h"
#include "playerInfo.h"
#include <iostream>

enum ECommsState
{
    CS_Inactive,          // No active comms
    CS_OpeningChannel,    // Opening a comms channel
    CS_BeingHailed,       // Receiving a hail from an object
    CS_BeingHailedByGM,   //                   ... the GM
    CS_ChannelOpen,       // Comms open to an object
    CS_ChannelOpenPlayer, //           ... another player
    CS_ChannelOpenGM,     //           ... the GM
    CS_ChannelFailed,     // Comms failed to connect
    CS_ChannelBroken,     // Comms broken by other side
    CS_ChannelClosed      // Comms manually closed
};

enum EAlertLevel
{
    AL_Normal,      // No alert state
    AL_YellowAlert, // Yellow
    AL_RedAlert,    // Red
    AL_MAX          // ?
};

class PlayerSpaceship : public SpaceShip
{
protected:
    static const int16_t CMD_SET_SHIELDS = 0x0009;
    static const int16_t CMD_SET_MAIN_SCREEN_SETTING = 0x000A; // Overlay is 0x0027
    static const int16_t CMD_SCAN_OBJECT = 0x000B;
    static const int16_t CMD_SCAN_DONE = 0x000C;
    static const int16_t CMD_SCAN_CANCEL = 0x000D;
    static const int16_t CMD_SET_SYSTEM_POWER_REQUEST = 0x000E;
    static const int16_t CMD_SET_SYSTEM_COOLANT_REQUEST = 0x000F;
    static const int16_t CMD_OPEN_TEXT_COMM = 0x0012; //TEXT communication
    static const int16_t CMD_CLOSE_TEXT_COMM = 0x0013;
    static const int16_t CMD_SEND_TEXT_COMM = 0x0014;
    static const int16_t CMD_SEND_TEXT_COMM_PLAYER = 0x0015;
    static const int16_t CMD_ANSWER_COMM_HAIL = 0x0016;
    static const int16_t CMD_SET_AUTO_REPAIR = 0x0017;
    static const int16_t CMD_ADD_WAYPOINT = 0x001B;
    static const int16_t CMD_REMOVE_WAYPOINT = 0x001C;
    static const int16_t CMD_MOVE_WAYPOINT = 0x001D;
    static const int16_t CMD_ACTIVATE_SELF_DESTRUCT = 0x001E;
    static const int16_t CMD_CANCEL_SELF_DESTRUCT = 0x001F;
    static const int16_t CMD_CONFIRM_SELF_DESTRUCT = 0x0020;
    static const int16_t CMD_SET_ALERT_LEVEL = 0x0024;
    static const int16_t CMD_SET_SCIENCE_LINK = 0x0025;
    static const int16_t CMD_SET_PROBE_3D_LINK = 0x0026;
    static const int16_t CMD_SET_MAIN_SCREEN_OVERLAY = 0x0028;
    static const int16_t CMD_CUSTOM_FUNCTION = 0x002A;
    static const int16_t CMD_SET_AUTO_REPAIR_SYSTEM_TARGET = 0x0030;
public:
    // Power consumption and generation base rates
    constexpr static float energy_shield_use_per_second = 1.5f;
    constexpr static float energy_warp_per_second = 1.0f;
    constexpr static float system_heatup_per_second = 0.05f;
    constexpr static float system_power_level_change_per_second = 0.3;
    // Coolant change rate
    constexpr static float system_coolant_level_change_per_second = 1.2;
    // Total coolant
    constexpr static float max_coolant = 10.0;
    // Overheat subsystem damage rate
    constexpr static float damage_per_second_on_overheat = 0.08f;
    // Base time it takes to perform an action
    constexpr static float shield_calibration_time = 25.0f;
    constexpr static float comms_channel_open_time = 2.0;
    constexpr static float scan_probe_charge_time = 10.0f;
    constexpr static float max_scanning_delay = 6.0;
    // Maximum number of self-destruction confirmation codes
    constexpr static int max_self_destruct_codes = 3;
    // Subsystem effectiveness base rates
    static float system_power_user_factor[];
    
    constexpr static int16_t CMD_PLAY_CLIENT_SOUND = 0x0001;

    class CustomShipFunction
    {
    public:
        enum class Type
        {
            Info,
            Button,
            Message
        };
        Type type;
        string name;
        string caption;
        ECrewPosition crew_position;
        ScriptSimpleCallback callback;
        
        bool operator!=(const CustomShipFunction& csf) { return type != csf.type || name != csf.name || caption != csf.caption || crew_position != csf.crew_position; }
    };

    // Visual indicators of hull damage and in-progress jumps
    float hull_damage_indicator;
    float jump_indicator;
    // Target of a scan. Server-only value
    P<SpaceObject> scanning_target;
    // Time in seconds to scan an object if scanning_complexity is 0 (none)
    float scanning_delay;
    // Number of sliders during a scan
    int scanning_complexity;
    // Number of times an object must be scanned to achieve a fully scanned
    // state
    int scanning_depth;
    // Time in seconds it takes to recalibrate shields
    float shield_calibration_delay;
    // Ship automation features, mostly for single-person ships like fighters
    bool auto_repair_enabled;
    bool auto_coolant_enabled;
    // Whether shields are up (true) or down
    bool shields_active;
    // Password to join a ship. Default is empty.
    string control_code;

private:
    // soundManager index of the shield object
    int shield_sound;
    // Comms variables
    ECommsState comms_state;
    float comms_open_delay;
    string comms_target_name;
    string comms_incomming_message;
    P<SpaceObject> comms_target; // Server only
    std::vector<int> comms_reply_id;
    std::vector<string> comms_reply_message;
    CommsScriptInterface comms_script_interface; // Server only
    
public:
    std::vector<CustomShipFunction> custom_functions;

    std::vector<sf::Vector2f> waypoints;
    
    // Scan probe capacity
    int max_scan_probes;
    int scan_probe_stock;
    float scan_probe_recharge;

    // Main screen content
    EMainScreenSetting main_screen_setting;
    // Content overlaid on the main screen, such as comms
    EMainScreenOverlay main_screen_overlay;

    bool activate_self_destruct;
    uint32_t self_destruct_code[max_self_destruct_codes];
    bool self_destruct_code_confirmed[max_self_destruct_codes];
    ECrewPosition self_destruct_code_entry_position[max_self_destruct_codes];
    ECrewPosition self_destruct_code_show_position[max_self_destruct_codes];
    float self_destruct_countdown;

    EAlertLevel alert_level;

    int32_t linked_science_probe_id;

    PlayerSpaceship();

    // Comms functions
    bool isCommsInactive() { return comms_state == CS_Inactive; }
    bool isCommsOpening() { return comms_state == CS_OpeningChannel; }
    bool isCommsBeingHailed() { return comms_state == CS_BeingHailed || comms_state == CS_BeingHailedByGM; }
    bool isCommsBeingHailedByGM() { return comms_state == CS_BeingHailedByGM; }
    bool isCommsFailed() { return comms_state == CS_ChannelFailed; }
    bool isCommsBroken() { return comms_state == CS_ChannelBroken; }
    bool isCommsClosed() { return comms_state == CS_ChannelClosed; }
    bool isCommsChatOpen() { return comms_state == CS_ChannelOpenPlayer || comms_state == CS_ChannelOpenGM; }
    bool isCommsChatOpenToGM() { return comms_state == CS_ChannelOpenGM; }
    bool isCommsChatOpenToPlayer() { return comms_state == CS_ChannelOpenPlayer; }
    bool isCommsScriptOpen() { return comms_state == CS_ChannelOpen; }
    ECommsState getCommsState() { return comms_state; }
    float getCommsOpeningDelay() { return comms_open_delay; }
    const std::vector<string>& getCommsReplyOptions() const { return comms_reply_message; }
    P<SpaceObject> getCommsTarget() { return comms_target; }
    const string& getCommsTargetName() { return comms_target_name; }
    const string& getCommsIncommingMessage() { return comms_incomming_message; }
    bool hailCommsByGM(string target_name);
    bool hailByObject(P<SpaceObject> object, string opening_message);
    void setCommsMessage(string message);
    void addCommsIncommingMessage(string message);
    void addCommsOutgoingMessage(string message);
    void addCommsReply(int32_t id, string message);
    void closeComms();

    void setEnergyLevel(float amount) { energy_level = std::max(0.0f, std::min(max_energy_level, amount)); }
    void setEnergyLevelMax(float amount) { max_energy_level = std::max(0.0f, amount); energy_level = std::min(energy_level, max_energy_level); }
    float getEnergyLevel() { return energy_level; }
    float getEnergyLevelMax() { return max_energy_level; }
    
    void setScanProbeCount(int amount) { scan_probe_stock = std::max(0, std::min(amount, max_scan_probes)); }
    int getScanProbeCount() { return scan_probe_stock; }
    void setMaxScanProbeCount(int amount) { max_scan_probes = std::max(0, amount); scan_probe_stock = std::min(scan_probe_stock, max_scan_probes); }
    int getMaxScanProbeCount() { return max_scan_probes; }

    void addCustomButton(ECrewPosition position, string name, string caption, ScriptSimpleCallback callback);
    void addCustomInfo(ECrewPosition position, string name, string caption);
    void addCustomMessage(ECrewPosition position, string name, string caption);
    void addCustomMessageWithCallback(ECrewPosition position, string name, string caption, ScriptSimpleCallback callback);
    void removeCustom(string name);

    // Client command functions
    virtual void handleClientCommand(int32_t client_id, int16_t command, sf::Packet& packet) override;
    void commandSetScienceLink(int32_t id);
    void commandSetShields(bool enabled);
    void commandMainScreenSetting(EMainScreenSetting mainScreen);
    void commandMainScreenOverlay(EMainScreenOverlay mainScreen);
    void commandScan(P<SpaceObject> object);
    void commandSetSystemPowerRequest(ESystem system, float power_level);
    void commandSetSystemCoolantRequest(ESystem system, float coolant_level);
    void commandOpenTextComm(P<SpaceObject> obj);
    void commandCloseTextComm();
    void commandAnswerCommHail(bool awnser);
    void commandSendComm(uint8_t index);
    void commandSendCommPlayer(string message);
    void commandSetAutoRepair(bool enabled);
    void commandSetShieldFrequency(int32_t frequency);
    void commandAddWaypoint(sf::Vector2f position);
    void commandRemoveWaypoint(int32_t index);
    void commandMoveWaypoint(int32_t index, sf::Vector2f position);
    void commandActivateSelfDestruct();
    void commandCancelSelfDestruct();
    void commandConfirmDestructCode(int8_t index, uint32_t code);
    void commandLaunchProbe(sf::Vector2f target_position);
    void commandScanDone();
    void commandScanCancel();
    void commandSetAlertLevel(EAlertLevel level);
    void commandCustomFunction(string name);

    virtual void onReceiveServerCommand(sf::Packet& packet) override;

    // Template function
    virtual void applyTemplateValues() override;

    // Ship status functions
    virtual void executeJump(float distance) override;
    virtual void takeHullDamage(float damage_amount, DamageInfo& info) override;
    void setSystemCoolantRequest(ESystem system, float request);
    void setAutoCoolant(bool active) { auto_coolant_enabled = active; }
    int getRepairCrewCount();
    void setRepairCrewCount(int amount);
    EAlertLevel getAlertLevel() { return alert_level; }

    // Ship update functions
    virtual void update(float delta) override;
    virtual bool useEnergy(float amount) override;
    virtual void addHeat(ESystem system, float amount) override;
    
    // Call on the server to play a sound on the main screen.
    void playSoundOnMainScreen(string sound_name);

    float getNetSystemEnergyUsage();

    // Ship's crew functions
    void transferPlayersToShip(P<PlayerSpaceship> other_ship);
    void transferPlayersAtPositionToShip(ECrewPosition position, P<PlayerSpaceship> other_ship);
    bool hasPlayerAtPosition(ECrewPosition position);

    // Ship shields functions
    virtual bool getShieldsActive() override { return shields_active; }
    void setShieldsActive(bool active) { shields_active = active; }

    // Waypoint functions
    int getWaypointCount() { return waypoints.size(); }
    sf::Vector2f getWaypoint(int index) { if (index > 0 && index <= int(waypoints.size())) return waypoints[index - 1]; return sf::Vector2f(0, 0); }

    // Ship control code/password setter
    void setControlCode(string code) { control_code = code; }

    // Radar function
    virtual void drawOnGMRadar(sf::RenderTarget& window, sf::Vector2f position, float scale, bool long_range) override;

    // Script export function
    virtual string getExportLine();
};
REGISTER_MULTIPLAYER_ENUM(ECommsState);
template<> int convert<EAlertLevel>::returnType(lua_State* L, EAlertLevel l);
template<> void convert<EAlertLevel>::param(lua_State* L, int& idx, EAlertLevel& al);
REGISTER_MULTIPLAYER_ENUM(EAlertLevel);

static inline sf::Packet& operator << (sf::Packet& packet, const PlayerSpaceship::CustomShipFunction& csf) { return packet << uint8_t(csf.type) << uint8_t(csf.crew_position) << csf.name << csf.caption; } \
static inline sf::Packet& operator >> (sf::Packet& packet, PlayerSpaceship::CustomShipFunction& csf) { int8_t tmp; packet >> tmp; csf.type = PlayerSpaceship::CustomShipFunction::Type(tmp); packet >> tmp; csf.crew_position = ECrewPosition(tmp); packet >> csf.name >> csf.caption; return packet; }

string alertLevelToString(EAlertLevel level);

#ifdef _MSC_VER
#include "playerSpaceship.hpp"
#endif /* _MSC_VER */

#endif//PLAYER_SPACESHIP_H
