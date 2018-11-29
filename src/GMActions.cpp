#include "GMActions.h"

#include "engine.h"
#include "gameGlobalInfo.h"

const static int16_t CMD_RUN_SCRIPT = 0x0000;
const static int16_t CMD_SEND_GLOBAL_MESSAGE = 0x0001;
const static int16_t CMD_INTERCEPT_ALL_COMMS_TO_GM = 0x0002;
const static int16_t CMD_CALL_GM_SCRIPT = 0x0003;

P<GameMasterActions> gameMasterActions;

REGISTER_MULTIPLAYER_CLASS(GameMasterActions, "GameMasterActions")
GameMasterActions::GameMasterActions()
: MultiplayerObject("GameMasterActions"), gmSelectionForRunningScript(nullptr)
{
    assert(!gameMasterActions);
    gameMasterActions = this;
}

void GameMasterActions::onReceiveClientCommand(int32_t client_id, sf::Packet& packet)
{
    int16_t command;
    packet >> command;
    switch(command)
    {
    case CMD_RUN_SCRIPT:
        {
            string code;
            packet >> code;
            if (code.length() > 0)
            {
                P<ScriptObject> so = new ScriptObject();
                so->runCode(code);
                so->destroy();
            }
        }
        break;
    case CMD_SEND_GLOBAL_MESSAGE:
        {
            string message;
            packet >> message;
            if (message.length() > 0)
            {
                gameGlobalInfo->global_message = message;
                gameGlobalInfo->global_message_timeout = 5.0;
            }
        }
        break;
    case CMD_INTERCEPT_ALL_COMMS_TO_GM:
        {
            bool value;
            packet >> value;
            gameGlobalInfo->intercept_all_comms_to_gm = value;
        }
        break;
    case CMD_CALL_GM_SCRIPT:
        {
            int index;
            packet >> index;
            int selectedItemsLeft;
            packet >> selectedItemsLeft;
            PVector<SpaceObject> selection;
            while (selectedItemsLeft--) {
                int selectedItemId;
                packet >> selectedItemId;
                P<SpaceObject> object;
                if (game_server)
                    object = game_server->getObjectById(selectedItemId);
                else 
                    object = game_client->getObjectById(selectedItemId);
                if (object)
                    selection.push_back(object);
            }
            // set selection for the possible duration of the script
            gmSelectionForRunningScript = &selection;
            int n = 0;
            for(GMScriptCallback& callback : gameGlobalInfo->gm_callback_functions)
            {
                if (n == index)
                {
                    callback.callback.call();
                    return;
                }
                n++;
            }
            // clear selection
            gmSelectionForRunningScript = nullptr;
        }
        break;
    }
}

void GameMasterActions::commandRunScript(string code)
{
    sf::Packet packet;
    packet << CMD_RUN_SCRIPT << code;
    sendClientCommand(packet);
}
void GameMasterActions::commandSendGlobalMessage(string message)
{
    sf::Packet packet;
    packet << CMD_SEND_GLOBAL_MESSAGE << message;
    sendClientCommand(packet);
}
void GameMasterActions::commandInterceptAllCommsToGm(bool value)
{
    sf::Packet packet;
    packet << CMD_INTERCEPT_ALL_COMMS_TO_GM << value;
    sendClientCommand(packet);
}
void GameMasterActions::commandCallGmScript(int index, PVector<SpaceObject> selection)
{
    sf::Packet packet;
    packet << CMD_CALL_GM_SCRIPT << index << selection.size();
    foreach(SpaceObject, s, selection)
    {
        packet << s->getMultiplayerId();
    }
    sendClientCommand(packet);
}