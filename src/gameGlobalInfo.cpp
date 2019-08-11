#include "gameGlobalInfo.h"
#include "preferenceManager.h"
#include "resources.h"
#include <regex>

static inline sf::Packet& operator << (sf::Packet& packet, const ECommsGmInterception& cgi) { return packet << int(cgi); }
static inline sf::Packet& operator >> (sf::Packet& packet, ECommsGmInterception& cgi) { 
    int value;
    packet >> value;
    cgi = ECommsGmInterception(value);
    return packet;
}

P<GameGlobalInfo> gameGlobalInfo;

REGISTER_MULTIPLAYER_CLASS(GameGlobalInfo, "GameGlobalInfo")
GameGlobalInfo::GameGlobalInfo()
: MultiplayerObject("GameGlobalInfo")
{
    assert(!gameGlobalInfo);

    callsign_counter = 0;
    victory_faction = -1;
    gameGlobalInfo = this;
    for(int n=0; n<max_player_ships; n++)
    {
        playerShipId[n] = -1;
        registerMemberReplication(&playerShipId[n]);
    }
    for(int n=0; n<max_nebulas; n++)
    {
        nebula_info[n].vector = sf::Vector3f(random(-1, 1), random(-1, 1), random(-1, 1));
        nebula_info[n].textureName = "Nebula" + string(irandom(1, 3));
        registerMemberReplication(&nebula_info[n].vector);
        registerMemberReplication(&nebula_info[n].textureName);
    }
    for(int n=0; n<max_terrain_layers; n++)
    {
        registerMemberReplication(&terrain[n].textureName);
        registerMemberReplication(&terrain[n].scale);
        registerMemberReplication(&terrain[n].coordinates);
        registerMemberReplication(&terrain[n].defined);
        terrain[n].defined = false;
    }
    global_message_timeout = 0.0;
    player_warp_jump_drive_setting = PWJ_ShipDefault;
    scanning_complexity = SC_Normal;
    long_range_radar_range = 30000;
    use_beam_shield_frequencies = true;
    use_system_damage = true;
    use_repair_crew = false;
    allow_main_screen_tactical_radar = true;
    allow_main_screen_long_range_radar = true;
    allow_main_screen_global_range_radar = true;
    allow_main_screen_ship_state = true;
    intercept_all_comms_to_gm = CGI_None;

    registerMemberReplication(&scanning_complexity);
    registerMemberReplication(&global_message);
    registerMemberReplication(&global_message_timeout, 1.0);
    registerMemberReplication(&banner_string);
    registerMemberReplication(&victory_faction);
    registerMemberReplication(&long_range_radar_range);
    registerMemberReplication(&use_beam_shield_frequencies);
    registerMemberReplication(&use_system_damage);
    registerMemberReplication(&use_repair_crew);
    registerMemberReplication(&allow_main_screen_tactical_radar);
    registerMemberReplication(&allow_main_screen_long_range_radar);
    registerMemberReplication(&allow_main_screen_global_range_radar);
    registerMemberReplication(&allow_main_screen_ship_state);
    registerMemberReplication(&gm_callback_names);
    registerMemberReplication(&intercept_all_comms_to_gm);

}

P<PlayerSpaceship> GameGlobalInfo::getPlayerShip(int index)
{
    assert(index >= 0 && index < max_player_ships);
    if (game_server)
        return game_server->getObjectById(playerShipId[index]);
    return game_client->getObjectById(playerShipId[index]);
}

void GameGlobalInfo::setPlayerShip(int index, P<PlayerSpaceship> ship)
{
    assert(index >= 0 && index < max_player_ships);
    assert(game_server);

    if (ship)
        playerShipId[index] = ship->getMultiplayerId();
    else
        playerShipId[index] = -1;
}

int GameGlobalInfo::findPlayerShip(P<PlayerSpaceship> ship)
{
    for(int n=0; n<max_player_ships; n++)
        if (getPlayerShip(n) == ship)
            return n;
    return -1;
}

int GameGlobalInfo::insertPlayerShip(P<PlayerSpaceship> ship)
{
    for(int n=0; n<max_player_ships; n++)
    {
        if (!getPlayerShip(n))
        {
            setPlayerShip(n, ship);
            return n;
        }
    }
    return -1;
}

void GameGlobalInfo::update(float delta)
{
    if (global_message_timeout > 0.0)
    {
        global_message_timeout -= delta;
    }
    if (my_player_info)
    {
        //Set the my_spaceship variable based on the my_player_info->ship_id
        if ((my_spaceship && my_spaceship->getMultiplayerId() != my_player_info->ship_id) || (my_spaceship && my_player_info->ship_id == -1) || (!my_spaceship && my_player_info->ship_id != -1))
        {
            if (game_server)
                my_spaceship = game_server->getObjectById(my_player_info->ship_id);
            else
                my_spaceship = game_client->getObjectById(my_player_info->ship_id);
        }
    }
}

string GameGlobalInfo::getNextShipCallsign()
{
    callsign_counter += 1;
    switch(irandom(0, 9))
    {
    case 0: return "S" + string(callsign_counter);
    case 1: return "NC" + string(callsign_counter);
    case 2: return "CV" + string(callsign_counter);
    case 3: return "SS" + string(callsign_counter);
    case 4: return "VS" + string(callsign_counter);
    case 5: return "BR" + string(callsign_counter);
    case 6: return "CSS" + string(callsign_counter);
    case 7: return "UTI" + string(callsign_counter);
    case 8: return "VK" + string(callsign_counter);
    case 9: return "CCN" + string(callsign_counter);
    }
    return "SS" + string(callsign_counter);
}

void GameGlobalInfo::addScript(P<Script> script)
{
    script_list.update();
    script_list.push_back(script);
}

void GameGlobalInfo::reset()
{
    gm_callback_functions.clear();

    foreach(GameEntity, e, entityList)
        e->destroy();
    foreach(SpaceObject, o, space_object_list)
        o->destroy();
    if (engine->getObject("scenario"))
        engine->getObject("scenario")->destroy();

    foreach(Script, s, script_list)
    {
        s->destroy();
    }
    callsign_counter = 0;
    victory_faction = -1;
    for(unsigned int n=0; n<factionInfo.size(); n++)
        factionInfo[n]->reset();
}

void GameGlobalInfo::startScenario(string filename)
{
    reset();

    P<ScriptObject> script = new ScriptObject();
    script->run(filename);
    engine->registerObject("scenario", script);

    if (PreferencesManager::get("game_logs", "1").toInt())
    {
        state_logger = new GameStateLogger();
        state_logger->start();
    }
}

void GameGlobalInfo::destroy()
{
    reset();
    MultiplayerObject::destroy();
    if (state_logger)
        state_logger->destroy();
}

string GameGlobalInfo::getExportLine(){
    string ret = "";
    for(int n=0; n<max_terrain_layers; n++)
    {
        if (terrain[n].defined){
            ret += string("setTerrain(")+
            string(n, 0) + ", "+
            "\"" + terrain[n].textureName + "\", "+
            string(terrain[n].coordinates.x, 0) + ", "+
            string(terrain[n].coordinates.y, 0) + ", "+
            string(terrain[n].scale) + ")\n";
        }
    }
    return ret;
}

void GameGlobalInfo::setTerrain(int terrainId, string textureName, sf::Vector2f coordinates, float scale){
    if (game_server){
        TerrainInfo &terrain = this->terrain[terrainId];
        // only server can effectively load terrainImage data
        P<ResourceStream> stream = getResourceStream(textureName);
        if (!stream) stream = getResourceStream(textureName + ".png");
        if (stream && terrain.image.loadFromStream(**stream)){
            terrain.defined = true;
            terrain.textureName = textureName;
            terrain.coordinates = coordinates;
            terrain.scale = scale;
        } else {
            LOG(WARNING) << "Failed to load terrain texture image: " << textureName;
        }
    } else {
        LOG(ERROR) << "GameGlobalInfo::setTerrain can only be called locally on the server";
    }
}

sf::Color GameGlobalInfo::getTerrainPixel(int terrainId, sf::Vector2f coordinates){ 
    TerrainInfo &terrain = this->terrain[terrainId];
    coordinates = (sf::Vector2f(terrain.image.getSize()) * 0.5f) + ((coordinates + terrain.coordinates) / terrain.scale);
    if (coordinates.x < 0 || coordinates.x > terrain.image.getSize().x || 
        coordinates.y < 0 || coordinates.y > terrain.image.getSize().y)
        // outside of image boundaries
        return sf::Color::Transparent;
     else 
        return terrain.image.getPixel(coordinates.x, coordinates.y);
}

string playerWarpJumpDriveToString(EPlayerWarpJumpDrive player_warp_jump_drive)
{
    switch(player_warp_jump_drive)
    {
    case PWJ_ShipDefault:
        return "Ship default";
    case PWJ_WarpDrive:
        return "Warp-drive";
    case PWJ_JumpDrive:
        return "Jump-drive";
    case PWJ_WarpAndJumpDrive:
        return "Both";
    default:
        return "?";
    }
}

std::regex sector_rgx("(\\d)(\\d+)\\.(\\d+)");
std::regex location_rgx("(\\d+\\.\\d+)\\.(\\d+)\\.(\\d+)");

bool isValidSectorName(string sectorName)
{
    return std::regex_match(sectorName, sector_rgx);
}

bool isValidPositionString(string positionStr)
{
    if (isValidSectorName(positionStr)) 
        return true;
    std::smatch matches;
    if(std::regex_match(positionStr, matches, location_rgx))
    {
        return std::stoi(matches.str(2)) < GameGlobalInfo::sector_size 
            && std::stoi(matches.str(3)) < GameGlobalInfo::sector_size;
    } else {
        return false;
    }
}

sf::Vector2f getSectorPosition(string sectorName)
{
    std::smatch matches;
    if(std::regex_match(sectorName, matches, sector_rgx)) 
    {
        int quadrant = std::stoi(matches.str(1));
        int sector_y = std::stoi(matches.str(3));
        int sector_x = std::stoi(matches.str(2));
        sector_y = sector_y - 1;
        if (quadrant % 2)
            sector_x = -1 - sector_x;
        if ((quadrant /2) % 2)
            sector_y = -1 - sector_y;
        return sf::Vector2f(sector_x * GameGlobalInfo::sector_size, sector_y * GameGlobalInfo::sector_size);
    } 
    else 
    {
        return sf::Vector2f(0,0);
    }
}

sf::Vector2f getPositionFromSring(string positionStr)
{
     if (isValidSectorName(positionStr)) 
        return getSectorPosition(positionStr);
    std::smatch matches;
    if(std::regex_match(positionStr, matches, location_rgx)) 
    {
        sf::Vector2f sectorPosition = getSectorPosition(matches.str(1));
        return sectorPosition + sf::Vector2f(std::stoi(matches.str(2)), std::stoi(matches.str(3)));
    } else {
        return sf::Vector2f(0,0);
    }
}

string getStringFromPosition(sf::Vector2f position)
{
    int offset_x = fmod(fmod(position.x, GameGlobalInfo::sector_size) + GameGlobalInfo::sector_size, GameGlobalInfo::sector_size);
    int offset_y = fmod(fmod(position.y, GameGlobalInfo::sector_size) + GameGlobalInfo::sector_size, GameGlobalInfo::sector_size);
    if (offset_x < 1 && offset_y < 1) {
        return getSectorName(position);
    } else {
        return getSectorName(position) + "." + string(offset_x,0) + '.' + string(offset_y,0);
    }
}

string getSectorName(sf::Vector2f position)
{
    int sector_x = floorf(position.x / GameGlobalInfo::sector_size);
    int sector_y = floorf(position.y / GameGlobalInfo::sector_size);
    int quadrant = 0;
    if (sector_y < 0)
    {
        quadrant += 2;
        sector_y = -1 - sector_y;
    }
    if (sector_x < 0)
    {
        quadrant += 1;
        sector_x = -1 - sector_x;
    }
    return string(char('0' +quadrant)) + string(sector_x) + "." + string(sector_y);
}

static int victory(lua_State* L)
{
    gameGlobalInfo->setVictory(luaL_checkstring(L, 1));
    if (engine->getObject("scenario"))
        engine->getObject("scenario")->destroy();
    engine->setGameSpeed(0.0);
    return 0;
}
/// victory(string)
/// Called with a faction name as parameter, sets a certain faction as victor and ends the game.
REGISTER_SCRIPT_FUNCTION(victory);

static int globalMessage(lua_State* L)
{
    gameGlobalInfo->global_message = luaL_checkstring(L, 1);
    gameGlobalInfo->global_message_timeout = 5.0;
    return 0;
}
/// globalMessage(string)
/// Show a global message on the main screens of all active player ships.
REGISTER_SCRIPT_FUNCTION(globalMessage);

static int setBanner(lua_State* L)
{
    gameGlobalInfo->banner_string = luaL_checkstring(L, 1);
    return 0;
}
/// setBanner(string)
/// Show a scrolling banner containing this text on the cinematic and top down views.
REGISTER_SCRIPT_FUNCTION(setBanner);

static int getPlayerShip(lua_State* L)
{
    int index = luaL_checkinteger(L, 1);
    if (index == -1)
    {
        for(index = 0; index<GameGlobalInfo::max_player_ships; index++)
        {
            P<PlayerSpaceship> ship = gameGlobalInfo->getPlayerShip(index);
            if (ship)
                return convert<P<PlayerSpaceship> >::returnType(L, ship);
        }
        return 0;
    }
    if (index < 1 || index > GameGlobalInfo::max_player_ships)
        return 0;
    P<PlayerSpaceship> ship = gameGlobalInfo->getPlayerShip(index - 1);
    if (!ship)
        return 0;
    return convert<P<PlayerSpaceship> >::returnType(L, ship);
}
/// getPlayerShip(index)
/// Return the player's ship, use -1 to get the first active player ship.
REGISTER_SCRIPT_FUNCTION(getPlayerShip);

static int getObjectsInRadius(lua_State* L)
{
    float x = luaL_checknumber(L, 1);
    float y = luaL_checknumber(L, 2);
    float r = luaL_checknumber(L, 3);

    sf::Vector2f position(x, y);

    PVector<SpaceObject> objects;
    PVector<Collisionable> objectList = CollisionManager::queryArea(position - sf::Vector2f(r, r), position + sf::Vector2f(r, r));
    foreach(Collisionable, obj, objectList)
    {
        P<SpaceObject> sobj = obj;
        if (sobj && (sobj->getPosition() - position) < r)
            objects.push_back(sobj);
    }

    return convert<PVector<SpaceObject> >::returnType(L, objects);
}
/// getObjectsInRadius(x, y, radius)
/// Return a list of all space objects at the x,y location within a certain radius.
REGISTER_SCRIPT_FUNCTION(getObjectsInRadius);

static int getAllObjects(lua_State* L)
{
    return convert<PVector<SpaceObject> >::returnType(L, space_object_list);
}
/// getAllObjects()
/// Return a list of all space objects. (Use with care, this could return a very long list which could slow down the game when called every update)
REGISTER_SCRIPT_FUNCTION(getAllObjects);

static int getScenarioVariation(lua_State* L)
{
    lua_pushstring(L, gameGlobalInfo->variation.c_str());
    return 1;
}
/// getScenarioVariation()
/// Returns the currently used scenario variation.
REGISTER_SCRIPT_FUNCTION(getScenarioVariation);

/** Short lived object to do a scenario change on the update loop. See "setScenario" for details */
class ScenarioChanger : public Updatable
{
public:
    ScenarioChanger(string script_name, string variation)
    : script_name(script_name), variation(variation)
    {
    }
    
    virtual void update(float delta)
    {
        gameGlobalInfo->variation = variation;
        gameGlobalInfo->startScenario(script_name);
        destroy();
    }
private:
    string script_name;
    string variation;
};

static int setScenario(lua_State* L)
{
    string script_name = luaL_checkstring(L, 1);
    string variation = luaL_optstring(L, 2, "");
    //This could be called from a currently active scenario script.
    // Calling GameGlobalInfo::startScenario is unsafe at this point,
    // as this will destroy the lua state that this function is running in.
    //So use the ScenarioChanger object which will do the change in the update loop. Which is safe.
    new ScenarioChanger(script_name, variation);
    return 0;
}
/// setScenario(script_name, variation_name)
/// Change the current scenario to a different one.
REGISTER_SCRIPT_FUNCTION(setScenario);

static int shutdownGame(lua_State* L)
{
    engine->shutdown();
    return 0;
}
/// Shutdown the game.
/// Calling this function will close the game. Mainly usefull for a headless server setup.
REGISTER_SCRIPT_FUNCTION(shutdownGame);

static int getLongRangeRadarRange(lua_State* L)
{
    lua_pushnumber(L, gameGlobalInfo->long_range_radar_range);
    return 1;
}
/// Return the long range radar range, normally 30.000, but can be configured per game.
REGISTER_SCRIPT_FUNCTION(getLongRangeRadarRange);

static int playSoundFile(lua_State* L)
{
    soundManager->playSound(luaL_checkstring(L, 1));
    return 0;
}
/// Play a sound file on the server. Will work with any file supported by SFML (.wav, .ogg, .flac)
/// Note that the sound is only played on the server. Not on any of the clients.
REGISTER_SCRIPT_FUNCTION(playSoundFile);

static int setTerrain(lua_State *L)
{
    string textureName = luaL_checkstring(L, 1);
    float x = luaL_checknumber(L, 2);
    float y = luaL_checknumber(L, 3);
    float scale = luaL_checknumber(L, 4);
    gameGlobalInfo->setTerrain(0, textureName, sf::Vector2f(x, y), scale);
    return 0;
}
/// setTerrain(textureName, x, y, scale)
/// set the terrain texture of the map
REGISTER_SCRIPT_FUNCTION(setTerrain);