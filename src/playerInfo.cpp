#include "playerInfo.h"
#include "screens/mainScreen.h"
#include "screens/crewStationScreen.h"

#include "screens/extra/navigationScreen.h"

#include "screens/crew6/helmsScreen.h"
#include "screens/crew6/weaponsScreen.h"
#include "screens/crew6/engineeringScreen.h"
#include "screens/crew6/scienceScreen.h"
#include "screens/crew6/relayScreen.h"

#include "screens/crew4/tacticalScreen.h"
#include "screens/crew4/engineeringAdvancedScreen.h"
#include "screens/crew4/operationsScreen.h"

#include "screens/crew1/singlePilotScreen.h"

#include "screens/extra/damcon.h"
#include "screens/extra/powerManagement.h"
#include "screens/extra/databaseScreen.h"
#include "screens/extra/commsScreen.h"
#include "screens/extra/droneOperatorScreen.h"
#include "screens/extra/dockMasterScreen.h"

#include "screens/extra/shipLogScreen.h"

#include "screens/extra/radarScreen.h"

#include "screens/helios/engineControlScreen.h"
#include "screens/helios/helmsHeliosScreen.h"
#include "screens/helios/weaponsHeliosScreen.h"
#include "screens/helios/databaseHeliosScreen.h"
#include "screens/helios/tractorBeamScreen.h"
#include "screens/helios/scienceHeliosScreen.h"
#include "screens/helios/scienceTaskHeliosScreen.h"

#include "screenComponents/mainScreenControls.h"
#include "screenComponents/selfDestructEntry.h"

static const int16_t CMD_UPDATE_CREW_POSITION = 0x0001;
static const int16_t CMD_UPDATE_SHIP_ID = 0x0002;
static const int16_t CMD_UPDATE_MAIN_SCREEN_CONTROL = 0x0003;

P<PlayerInfo> my_player_info;
P<PlayerSpaceship> my_spaceship;
PVector<PlayerInfo> player_info_list;

REGISTER_MULTIPLAYER_CLASS(PlayerInfo, "PlayerInfo");
PlayerInfo::PlayerInfo()
: MultiplayerObject("PlayerInfo")
{
    ship_id = -1;
    client_id = -1;
    main_screen_control = false;
    registerMemberReplication(&client_id);

    for(int n=0; n<max_crew_positions; n++)
    {
        crew_position[n] = false;
        registerMemberReplication(&crew_position[n]);
    }
    registerMemberReplication(&ship_id);
    registerMemberReplication(&main_screen_control);

    player_info_list.push_back(this);
}

void PlayerInfo::commandSetCrewPosition(ECrewPosition position, bool active)
{
    sf::Packet packet;
    packet << CMD_UPDATE_CREW_POSITION << int32_t(position) << active;
    sendClientCommand(packet);
    
    crew_position[position] = active;
}

void PlayerInfo::commandSetShipId(int32_t id)
{
    sf::Packet packet;
    packet << CMD_UPDATE_SHIP_ID << id;
    sendClientCommand(packet);
}

void PlayerInfo::commandSetMainScreenControl(bool control)
{
    sf::Packet packet;
    packet << CMD_UPDATE_MAIN_SCREEN_CONTROL << control;
    sendClientCommand(packet);
    
    main_screen_control = control;
}

void PlayerInfo::onReceiveClientCommand(int32_t client_id, sf::Packet& packet)
{
    if (client_id != this->client_id) return;
    int16_t command;
    packet >> command;
    switch(command)
    {
    case CMD_UPDATE_CREW_POSITION:
        {
            int32_t position;
            bool active;
            packet >> position >> active;
            crew_position[position] = active;

            if (isMainScreen())
                main_screen_control = false;
        }
        break;
    case CMD_UPDATE_SHIP_ID:
        packet >> ship_id;
        break;
    case CMD_UPDATE_MAIN_SCREEN_CONTROL:
        packet >> main_screen_control;
        break;
    }
}

bool PlayerInfo::isMainScreen()
{
    for(int n=0; n<max_crew_positions; n++)
        if (crew_position[n])
            return false;
    return true;
}

void PlayerInfo::spawnUI()
{
    if (my_player_info->isMainScreen())
    {
        new ScreenMainScreen();
    }else{

        CrewStationScreen* screen = new CrewStationScreen();
        // Helios
        if (crew_position[engineControlHeliosScreen])
            screen->addStationTab(new EngineControlScreen(screen, engineControlHeliosScreen), engineControlHeliosScreen, getCrewPositionName(engineControlHeliosScreen), getCrewPositionIcon(engineControlHeliosScreen));
        if (crew_position[bridgeEngineeringHeliosScreen])
            screen->addStationTab(new EngineControlScreen(screen, bridgeEngineeringHeliosScreen), bridgeEngineeringHeliosScreen, getCrewPositionName(bridgeEngineeringHeliosScreen), getCrewPositionIcon(bridgeEngineeringHeliosScreen));
        if (crew_position[helmsHeliosScreen])
            screen->addStationTab(new HelmsHeliosScreen(screen), helmsHeliosScreen, getCrewPositionName(helmsHeliosScreen), getCrewPositionIcon(helmsHeliosScreen));
        if (crew_position[weaponsHeliosScreen])
            screen->addStationTab(new WeaponsHeliosScreen(screen), weaponsHeliosScreen, getCrewPositionName(weaponsHeliosScreen), getCrewPositionIcon(weaponsHeliosScreen));
        if (crew_position[databaseHeliosScreen])
            screen->addStationTab(new DatabaseHeliosScreen(screen, false), databaseHeliosScreen, getCrewPositionName(databaseHeliosScreen), getCrewPositionIcon(databaseHeliosScreen));
        if (crew_position[bridgeDatabaseHeliosScreen])
            screen->addStationTab(new DatabaseHeliosScreen(screen, true), bridgeDatabaseHeliosScreen, getCrewPositionName(bridgeDatabaseHeliosScreen), getCrewPositionIcon(bridgeDatabaseHeliosScreen));
        if (crew_position[tractorBeamHeliosScreen])
            screen->addStationTab(new TractorBeamScreen(screen), tractorBeamHeliosScreen, getCrewPositionName(tractorBeamHeliosScreen), getCrewPositionIcon(tractorBeamHeliosScreen));
        if (crew_position[scienceHeliosScreen])
            screen->addStationTab(new ScienceHeliosScreen(screen), scienceHeliosScreen, getCrewPositionName(scienceHeliosScreen), getCrewPositionIcon(scienceHeliosScreen));
        if (crew_position[scienceTaskHeliosScreen])
            screen->addStationTab(new ScienceTaskHeliosScreen(screen), scienceTaskHeliosScreen, getCrewPositionName(scienceTaskHeliosScreen), getCrewPositionIcon(scienceTaskHeliosScreen));
      	//Crew 6/5
        if (crew_position[helmsOfficer])
            screen->addStationTab(new HelmsScreen(screen), helmsOfficer, getCrewPositionName(helmsOfficer), getCrewPositionIcon(helmsOfficer));
        if (crew_position[weaponsOfficer])
            screen->addStationTab(new WeaponsScreen(screen), weaponsOfficer, getCrewPositionName(weaponsOfficer), getCrewPositionIcon(weaponsOfficer));
        if (crew_position[engineering])
            screen->addStationTab(new EngineeringScreen(screen), engineering, getCrewPositionName(engineering), getCrewPositionIcon(engineering));
        if (crew_position[scienceOfficer])
            screen->addStationTab(new ScienceScreen(screen), scienceOfficer, getCrewPositionName(scienceOfficer), getCrewPositionIcon(scienceOfficer));
        if (crew_position[relayOfficer])
            screen->addStationTab(new RelayScreen(screen), relayOfficer, getCrewPositionName(relayOfficer), getCrewPositionIcon(relayOfficer));

        //Crew 4/3
        if (crew_position[tacticalOfficer])
            screen->addStationTab(new TacticalScreen(screen), tacticalOfficer, getCrewPositionName(tacticalOfficer), getCrewPositionIcon(tacticalOfficer));
        if (crew_position[engineeringAdvanced])
            screen->addStationTab(new EngineeringAdvancedScreen(screen), engineeringAdvanced, getCrewPositionName(engineeringAdvanced), getCrewPositionIcon(engineeringAdvanced));
        if (crew_position[operationsOfficer])
            screen->addStationTab(new OperationScreen(screen), operationsOfficer, getCrewPositionName(operationsOfficer), getCrewPositionIcon(operationsOfficer));

        //Crew 1
        if (crew_position[singlePilot])
            screen->addStationTab(new SinglePilotScreen(screen), singlePilot, getCrewPositionName(singlePilot), getCrewPositionIcon(singlePilot));
        //Extra
        if (crew_position[damageControl])
            screen->addStationTab(new DamageControlScreen(screen), damageControl, getCrewPositionName(damageControl), getCrewPositionIcon(damageControl));
        if (crew_position[powerManagement])
            screen->addStationTab(new PowerManagementScreen(screen), powerManagement, getCrewPositionName(powerManagement), getCrewPositionIcon(powerManagement));
        if (crew_position[databaseView])
            screen->addStationTab(new DatabaseScreen(screen), databaseView, getCrewPositionName(databaseView), getCrewPositionIcon(databaseView));
      	if (crew_position[relayOfficerNC])
            screen->addStationTab(new RelayScreen(screen, false), relayOfficer, getCrewPositionName(relayOfficer), getCrewPositionIcon(relayOfficer));
      	if (crew_position[relayHelios])
            screen->addStationTab(new RelayScreen(screen, false, false, false), relayHelios, getCrewPositionName(relayHelios), getCrewPositionIcon(relayHelios));
        if (crew_position[commsView])
            screen->addStationTab(new CommsScreen(screen), commsView, getCrewPositionName(commsView), getCrewPositionIcon(commsView));
        if (crew_position[tacticalRadar])
            screen->addStationTab(new RadarScreen(screen,"tactical"), tacticalRadar, getCrewPositionName(tacticalRadar), getCrewPositionIcon(tacticalRadar));
        if (crew_position[scienceRadar])
            screen->addStationTab(new RadarScreen(screen,"science"), scienceRadar, getCrewPositionName(scienceRadar), getCrewPositionIcon(scienceRadar));
        if (crew_position[relayRadar])
            screen->addStationTab(new RadarScreen(screen,"relay"), relayRadar, getCrewPositionName(relayRadar), getCrewPositionIcon(relayRadar));
        if (crew_position[navigation])
            screen->addStationTab(new NavigationScreen(screen), navigation, getCrewPositionName(navigation), getCrewPositionIcon(navigation));
        if (crew_position[dronePilot])
            screen->addStationTab(new DroneOperatorScreen(screen), dronePilot, getCrewPositionName(dronePilot), getCrewPositionIcon(dronePilot));
        if (crew_position[dockMaster])
            screen->addStationTab(new DockMasterScreen(screen), dockMaster, getCrewPositionName(dockMaster), getCrewPositionIcon(dockMaster));
        if (crew_position[externLog])
            screen->addStationTab(new ShipLogScreen(screen, "extern"), externLog, getCrewPositionName(externLog), getCrewPositionIcon(externLog));
        if (crew_position[internLog])
            screen->addStationTab(new ShipLogScreen(screen, "intern", true), internLog, getCrewPositionName(internLog), getCrewPositionIcon(internLog));
        //Ship log screen, if you have comms, you have ships log. (note this is mostly replaced by the [at the bottom of the screen openable log]
        if (crew_position[singlePilot])
            screen->addStationTab(new ShipLogScreen(screen, "intern"), max_crew_positions, "Ships log", "");
        
        GuiSelfDestructEntry* sde = new GuiSelfDestructEntry(screen, "SELF_DESTRUCT_ENTRY");
        for(int n=0; n<max_crew_positions; n++)
            if (crew_position[n])
                sde->enablePosition(ECrewPosition(n));
        if (crew_position[tacticalOfficer])
        {
            sde->enablePosition(weaponsOfficer);
            sde->enablePosition(helmsOfficer);
        }
        if (crew_position[engineeringAdvanced])
        {
            sde->enablePosition(engineering);
        }
        if (crew_position[operationsOfficer])
        {
            sde->enablePosition(scienceOfficer);
            sde->enablePosition(relayOfficer);
        }

        if (main_screen_control)
            new GuiMainScreenControls(screen);

        screen->finishCreation();
    }
}

string getCrewPositionName(ECrewPosition position)
{
    switch(position)
    {
    case helmsOfficer: case helmsHeliosScreen: return "Helms";
    case weaponsOfficer: case weaponsHeliosScreen: return "Weapons";
    case engineering: return "Engineering";
    case scienceOfficer: case scienceHeliosScreen: return "Science";
    case relayOfficer: case relayHelios: return "Relay";
    case relayOfficerNC: return "Relay (No comms)";
    case tacticalOfficer: return "Tactical";
    case engineeringAdvanced: return "Engineering+";
    case operationsOfficer: return "Operations";
    case singlePilot: return "Single Pilot";
    case damageControl: return "Damage Control";
    case powerManagement: return "Power Management";
    case databaseView: case databaseHeliosScreen: return "Database";
    case bridgeDatabaseHeliosScreen: return "Bridge Database";
    case commsView: return "Comms View";
    case tacticalRadar: return "Tactical Radar";
    case scienceRadar: return "Science Radar";
    case relayRadar: return "Relay Radar";
    case navigation: return "Navigation";
    case dronePilot: return "Drone Pilot";
    case dockMaster: return "Dock Master";
    case externLog: return "External Log";
    case internLog: return "Internal Log";
    case engineControlHeliosScreen: return "ECR";
    case bridgeEngineeringHeliosScreen: return "Bridge Engineering";
    case tractorBeamHeliosScreen: return "Tractor Beam";
    case scienceTaskHeliosScreen: return "Science Task";
    default: return "ErrUnk: " + string(position);
    }
}

string getCrewPositionIcon(ECrewPosition position)
{
    switch(position)
    {
    case helmsOfficer: case helmsHeliosScreen: return "gui/icons/station-helm";
    case weaponsOfficer: case weaponsHeliosScreen: return "gui/icons/station-weapons";
    case engineering: return "gui/icons/station-engineering";
    case scienceOfficer: case scienceHeliosScreen: return "gui/icons/station-science";
    case relayOfficer: case relayHelios: return "gui/icons/station-relay";
    case relayOfficerNC: return "gui/icons/station-relay";
    case tacticalOfficer: return "";
    case engineeringAdvanced: return "";
    case operationsOfficer: return "";
    case singlePilot: return "";
    case damageControl: return "";
    case powerManagement: return "";
    case databaseView: case databaseHeliosScreen: case bridgeDatabaseHeliosScreen: return "";
    case commsView: return "";
    case tacticalRadar: return "";
    case scienceRadar: return "";
    case relayRadar: return "";
    case navigation: return "";
    case dronePilot: return "";
    case dockMaster: return "";
    case externLog: return "";
    case internLog: return "";
    case engineControlHeliosScreen: return "";
    case bridgeEngineeringHeliosScreen: return "";
    case tractorBeamHeliosScreen: return "";
    case scienceTaskHeliosScreen: return "";
    default: return "ErrUnk: " + string(position);
    }
}

/* Define script conversion function for the ECrewPosition enum. */
template<> void convert<ECrewPosition>::param(lua_State* L, int& idx, ECrewPosition& cp)
{
    string str = string(luaL_checkstring(L, idx++)).lower();

    // helios     
    if (str == "ecr")
        cp = engineControlHeliosScreen;
    else if (str == "bridge engineering")
        cp = bridgeEngineeringHeliosScreen;
    else if (str == "helmsHelios")
        cp = helmsHeliosScreen;
    else if (str == "weaponsHelios")
        cp = weaponsHeliosScreen;
    else if (str == "databaseHelios")
        cp = databaseHeliosScreen;
    else if (str == "bridgeDatabaseHelios")
        cp = bridgeDatabaseHeliosScreen;
    else if (str == "tractorBeamHelios")
        cp = tractorBeamHeliosScreen;
    else if (str == "scienceHelios")
        cp = scienceHeliosScreen;
    else if (str == "Science Task")
        cp = scienceTaskHeliosScreen;
    else if (str == "Relay Helios")
        cp = relayHelios;
    //6/5 player crew
    else if (str == "helms" || str == "helmsofficer")
        cp = helmsOfficer;
    else if (str == "weapons" || str == "weaponsofficer")
        cp = weaponsOfficer;
    else if (str == "engineering" || str == "engineeringsofficer")
        cp = engineering;
    else if (str == "science" || str == "scienceofficer")
        cp = scienceOfficer;
    else if (str == "relay" || str == "relayofficer")
        cp = relayOfficer;

    //4/3 player crew
    else if (str == "tactical" || str == "tacticalofficer")
        cp = tacticalOfficer;    //helms+weapons-shields
    else if (str == "engineering+" || str == "engineering+officer" || str == "engineeringadvanced" || str == "engineeringadvancedofficer")
        cp = engineeringAdvanced;//engineering+shields
    else if (str == "operations" || str == "operationsofficer")
        cp = operationsOfficer; //science+comms

    //1 player crew
    else if (str == "single" || str == "singlepilot")
        cp = singlePilot;

    //extras
    else if (str == "damagecontrol")
        cp = damageControl;
    else if (str == "powermanagement")
        cp = powerManagement;
    else if (str == "database" || str == "databaseview")
        cp = databaseView;
    else if (str == "comms" || str == "commsview")
        cp = commsView;
    else if (str == "tacticalradar" || str == "tacticalradarview")
        cp = tacticalRadar;
    else if (str == "scienceradar" || str == "scienceradarview")
        cp = scienceRadar;
    else if (str == "relayradar" || str == "relayradarview")
        cp = relayRadar;
    else if (str == "navigation" || str == "navigationview")
        cp = navigation;
    else if (str == "dronepilot" || str == "dronepilotview")
        cp = dronePilot;
    else if (str == "dockmaster" || str == "dockmasterview")
        cp = dockMaster;
    else if (str == "externLog" || str == "externLogview")
        cp = externLog;
    else if (str == "internLog" || str == "internLogview")
        cp = internLog;
    else
        luaL_error(L, "Unknown value for crew position: %s", str.c_str());
}
