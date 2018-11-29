#include "GMScriptCallback.h"
#include "screens/gm/gameMasterScreen.h"
#include "gameGlobalInfo.h"
#include "GMActions.h"

GMScriptCallback::GMScriptCallback(string name)
: name(name)
{
}

static int addGMFunction(lua_State* L)
{
    const char* name = luaL_checkstring(L, 1);

    ScriptSimpleCallback callback;
    
    int idx = 2;
    convert<ScriptSimpleCallback>::param(L, idx, callback);

    gameGlobalInfo->gm_callback_names.emplace_back(name);
    gameGlobalInfo->gm_callback_functions.emplace_back(name);
    GMScriptCallback* callback_object = &gameGlobalInfo->gm_callback_functions.back();
    callback_object->callback = callback;

    return 0;
}
/// addGMFunction(name, function)
/// Add a function that can be called from the GM console. This can be used to create helper scripts for the GM.
/// Or to give the GM console certain control over the scenario.
REGISTER_SCRIPT_FUNCTION(addGMFunction);

static int removeGMFunction(lua_State* L)
{
    string name = luaL_checkstring(L, 1);

    gameGlobalInfo->gm_callback_names.erase(std::remove(gameGlobalInfo->gm_callback_names.begin(), gameGlobalInfo->gm_callback_names.end(), name), gameGlobalInfo->gm_callback_names.end());
    gameGlobalInfo->gm_callback_functions.erase(std::remove_if(gameGlobalInfo->gm_callback_functions.begin(), gameGlobalInfo->gm_callback_functions.end(), [name](const GMScriptCallback& f) { return f.name == name; }), gameGlobalInfo->gm_callback_functions.end());

    return 0;
}
/// removeGMFunction(name)
/// Remove a function from the GM console
REGISTER_SCRIPT_FUNCTION(removeGMFunction);

static int clearGMFunctions(lua_State* L)
{
    gameGlobalInfo->gm_callback_names.clear();
    gameGlobalInfo->gm_callback_functions.clear();
    return 0;
}
/// clearGMFunctions()
/// Remove all the GM functions from the GM console.
REGISTER_SCRIPT_FUNCTION(clearGMFunctions);

static int getGMSelection(lua_State* L)
{
    PVector<SpaceObject> objects;
    if (gameMasterActions->gmSelectionForRunningScript){
        objects = *gameMasterActions->gmSelectionForRunningScript;
    } 
    return convert<PVector<SpaceObject> >::returnType(L, objects);
}
/// getGMSelection()
/// Returns an list of objects that the GM currently has selected.
REGISTER_SCRIPT_FUNCTION(getGMSelection);
