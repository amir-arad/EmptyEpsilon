#ifndef GM_ACTIONS
#define GM_ACTIONS

#include "engine.h"


class GameMasterActions;
class SpaceObject;
extern P<GameMasterActions> gameMasterActions;

class GameMasterActions : public MultiplayerObject
{

public:
    PVector<SpaceObject> *gmSelectionForRunningScript;

    GameMasterActions();

    void commandRunScript(string code);
    void commandSendGlobalMessage(string message);
    void commandInterceptAllCommsToGm(bool value);
    void commandCallGmScript(int index, PVector<SpaceObject> selection);
    virtual void onReceiveClientCommand(int32_t client_id, sf::Packet& packet);
};

#endif//GM_ACTIONS
