#ifndef GAME_GLOBAL_INFO_H
#define GAME_GLOBAL_INFO_H

#include "spaceObjects/playerSpaceship.h"
#include "script.h"
#include "GMMessage.h"
#include "gameStateLogger.h"

class GameGlobalInfo;
extern P<GameGlobalInfo> gameGlobalInfo;

class NebulaInfo
{
public:
    sf::Vector3f vector;
    string textureName;
};

class ActionItem: public MultiplayerObject
{
public:
    typedef std::function<void(string)> func_t;
    static PVector<ActionItem> actionItems;
    string title;
    string query;
    string response;
    func_t accept; // server only
    func_t decline; // server only
public:
    ActionItem();
    ActionItem(string title, string query, func_t resolve);
    ActionItem(string title, string query, func_t accept, func_t decline);
};

class MapLayer
{
public:
    bool defined;
    string title;
    string textureName;
    sf::Vector2f coordinates;
    float scale; 
    sf::Image image; // server only (no registerMemberReplication) pixels
};

enum EPlayerWarpJumpDrive
{
    PWJ_ShipDefault = 0,
    PWJ_WarpDrive,
    PWJ_JumpDrive,
    PWJ_WarpAndJumpDrive,
    PWJ_None,
    PWJ_MAX,
};
enum EScanningComplexity
{
    SC_None = 0,
    SC_Simple,
    SC_Normal,
    SC_Advanced,
    SC_MAX
};
enum ECommsGmInterception
{
    CGI_None = 0,
    CGI_Temp,
    CGI_Always
};
class GameGlobalInfo : public MultiplayerObject, public Updatable
{
    P<GameStateLogger> state_logger;
public:
    /*!
     * \brief Maximum number of player ships.
     */
    static const int max_player_ships = 32;
    /*!
     * \brief Maximum number of visual background nebulas.
     */
    static const int max_nebulas = 32;    
    /*!
     * \brief Maximum number of map layers.
     */
    static const int max_map_layers = 10;
    /*!
     * \size of a sector.
     */
    static const int sector_size = 20000;
private:
    int victory_faction;
    int32_t playerShipId[max_player_ships];
    int callsign_counter;
    /*!
     * \brief List of known scripts
     */
    PVector<Script> script_list;
public:
    string global_message;
    float global_message_timeout;
    
    string banner_string;

    NebulaInfo nebula_info[max_nebulas];
    EPlayerWarpJumpDrive player_warp_jump_drive_setting;
    EScanningComplexity scanning_complexity;
    /*!
     * \brief Range of the science radar.
     */
    float long_range_radar_range;
    bool use_beam_shield_frequencies;
    bool use_system_damage;
    bool use_repair_crew;
    float everything_jams_warp;
    bool allow_main_screen_tactical_radar;
    bool allow_main_screen_long_range_radar;
    bool allow_main_screen_global_range_radar;
    bool allow_main_screen_ship_state;
    string variation = "None";

    //List of script functions that can be called from the GM interface (Server only!)
    std::vector<ScriptSimpleCallback> gm_callback_functions;
    //List of names of gm_callback_functions scripts (replicated to clients)
    std::vector<string> gm_callback_names;
    std::list<GMMessage> gm_messages;
    //When active, all comms request goto the GM as chat, and normal scripted converstations are disabled. This does not disallow player<->player ship comms.
    ECommsGmInterception intercept_all_comms_to_gm;

    MapLayer layer[max_map_layers];
    GameGlobalInfo();

    P<PlayerSpaceship> getPlayerShip(int index);
    P<SpaceObject> getObjectByCallSign(string callsign);
    void setPlayerShip(int index, P<PlayerSpaceship> ship);

    int findPlayerShip(P<PlayerSpaceship> ship);
    int insertPlayerShip(P<PlayerSpaceship> ship);
    bool isPlayerFaction(unsigned int faction_id);
    /*!
     * \brief Set a faction to victorious.
     * \param string Name of the faction that won.
     */
    void setVictory(string faction_name) { victory_faction = FactionInfo::findFactionId(faction_name); }
    /*!
     * \brief Get ID of faction that won.
     * \param int
     */
    int getVictoryFactionId() { return victory_faction; }

    void addScript(P<Script> script);
    //Reset the global game state (called when we want to load a new scenario, and clear out this one)
    void reset();
    void startScenario(string filename);

    virtual void update(float delta);
    virtual void destroy();

    string getNextShipCallsign();
    void setMapLayer(int terrainId, string textureName, sf::Vector2f coordinates, float scale, string title);
    // get export line of layer data
    string getExportLine();
    sf::Color getTerrainPixel(int terrainId, sf::Vector2f coordinates);
};

string playerWarpJumpDriveToString(EPlayerWarpJumpDrive player_warp_jump_drive);
string getSectorName(sf::Vector2f position);
sf::Vector2f getSectorPosition(string sectorName);
bool isValidSectorName(string sectorName);
sf::Vector2f getPositionFromSring(string sectorName);
string getStringFromPosition(sf::Vector2f position);
bool isValidPositionString(string sectorName);
P<MultiplayerObject> getObjectById(int32_t id);

REGISTER_MULTIPLAYER_ENUM(EScanningComplexity);

#endif//GAME_GLOBAL_INFO_H
