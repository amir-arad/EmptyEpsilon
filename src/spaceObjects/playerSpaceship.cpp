#include "playerSpaceship.h"
#include "gui/colorConfig.h"
#include "scanProbe.h"
#include "repairCrew.h"
#include "explosionEffect.h"
#include "gameGlobalInfo.h"
#include "main.h"

#include "scriptInterface.h"

// PlayerSpaceship are ships controlled by a player crew.
REGISTER_SCRIPT_SUBCLASS(PlayerSpaceship, SpaceShip)
{
    // Returns the ship's EAlertLevel.
    REGISTER_SCRIPT_CLASS_FUNCTION(PlayerSpaceship, getAlertLevel);
    // Sets whether this ship's shields are raised or lowered.
    // Takes a Boolean value.
    REGISTER_SCRIPT_CLASS_FUNCTION(PlayerSpaceship, setShieldsActive);
    // Move all players connected to this ship to the same stations on a
    // different PlayerSpaceship. If the target isn't a PlayerSpaceship, this
    // function does nothing.
    // This can be used in scenarios to change the crew's ship.
    REGISTER_SCRIPT_CLASS_FUNCTION(PlayerSpaceship, transferPlayersToShip);
    // Transfers only the crew members who fill a specific station to another
    // PlayerSpaceship.
    REGISTER_SCRIPT_CLASS_FUNCTION(PlayerSpaceship, transferPlayersAtPositionToShip);
    // Returns true if a station is occupied by a player, and false if not.
    REGISTER_SCRIPT_CLASS_FUNCTION(PlayerSpaceship, hasPlayerAtPosition);

    // Comms functions return Boolean values if true.
    REGISTER_SCRIPT_CLASS_FUNCTION(PlayerSpaceship, isCommsInactive);
    REGISTER_SCRIPT_CLASS_FUNCTION(PlayerSpaceship, isCommsOpening);
    REGISTER_SCRIPT_CLASS_FUNCTION(PlayerSpaceship, isCommsBeingHailed);
    REGISTER_SCRIPT_CLASS_FUNCTION(PlayerSpaceship, isCommsBeingHailedByGM);
    REGISTER_SCRIPT_CLASS_FUNCTION(PlayerSpaceship, isCommsFailed);
    REGISTER_SCRIPT_CLASS_FUNCTION(PlayerSpaceship, isCommsBroken);
    REGISTER_SCRIPT_CLASS_FUNCTION(PlayerSpaceship, isCommsClosed);
    REGISTER_SCRIPT_CLASS_FUNCTION(PlayerSpaceship, isCommsChatOpen);
    REGISTER_SCRIPT_CLASS_FUNCTION(PlayerSpaceship, isCommsChatOpenToGM);
    REGISTER_SCRIPT_CLASS_FUNCTION(PlayerSpaceship, isCommsChatOpenToPlayer);
    REGISTER_SCRIPT_CLASS_FUNCTION(PlayerSpaceship, isCommsScriptOpen);

    REGISTER_SCRIPT_CLASS_FUNCTION(PlayerSpaceship, setEnergyLevel);
    REGISTER_SCRIPT_CLASS_FUNCTION(PlayerSpaceship, setEnergyLevelMax);
    REGISTER_SCRIPT_CLASS_FUNCTION(PlayerSpaceship, getEnergyLevel);
    REGISTER_SCRIPT_CLASS_FUNCTION(PlayerSpaceship, getEnergyLevelMax);

    /// Set the maximum coolant available to engineering. Default is 10.
    REGISTER_SCRIPT_CLASS_FUNCTION(PlayerSpaceship, setMaxCoolant);
    REGISTER_SCRIPT_CLASS_FUNCTION(PlayerSpaceship, getMaxCoolant);

    REGISTER_SCRIPT_CLASS_FUNCTION(PlayerSpaceship, setScanProbeCount);
    REGISTER_SCRIPT_CLASS_FUNCTION(PlayerSpaceship, getScanProbeCount);
    REGISTER_SCRIPT_CLASS_FUNCTION(PlayerSpaceship, setMaxScanProbeCount);
    REGISTER_SCRIPT_CLASS_FUNCTION(PlayerSpaceship, getMaxScanProbeCount);

    REGISTER_SCRIPT_CLASS_FUNCTION(PlayerSpaceship, addCustomButton);
    REGISTER_SCRIPT_CLASS_FUNCTION(PlayerSpaceship, addCustomInfo);
    REGISTER_SCRIPT_CLASS_FUNCTION(PlayerSpaceship, addCustomMessage);
    REGISTER_SCRIPT_CLASS_FUNCTION(PlayerSpaceship, addCustomMessageWithCallback);
    REGISTER_SCRIPT_CLASS_FUNCTION(PlayerSpaceship, removeCustom);

    // Command functions
    REGISTER_SCRIPT_CLASS_FUNCTION(PlayerSpaceship, commandSetShields);
    REGISTER_SCRIPT_CLASS_FUNCTION(PlayerSpaceship, commandMainScreenSetting);
    REGISTER_SCRIPT_CLASS_FUNCTION(PlayerSpaceship, commandMainScreenOverlay);
    REGISTER_SCRIPT_CLASS_FUNCTION(PlayerSpaceship, commandScan);
    REGISTER_SCRIPT_CLASS_FUNCTION(PlayerSpaceship, commandSetSystemPowerRequest);
    REGISTER_SCRIPT_CLASS_FUNCTION(PlayerSpaceship, commandSetSystemCoolantRequest);
    REGISTER_SCRIPT_CLASS_FUNCTION(PlayerSpaceship, commandOpenTextComm);
    REGISTER_SCRIPT_CLASS_FUNCTION(PlayerSpaceship, commandCloseTextComm);
    REGISTER_SCRIPT_CLASS_FUNCTION(PlayerSpaceship, commandAnswerCommHail);
    REGISTER_SCRIPT_CLASS_FUNCTION(PlayerSpaceship, commandSendComm);
    REGISTER_SCRIPT_CLASS_FUNCTION(PlayerSpaceship, commandSendCommPlayer);
    // Command repair crews to automatically move to damaged subsystems.
    // Use this command on ships to require less player interaction, especially
    // when combined with setAutoCoolant/auto_coolant_enabled.
    REGISTER_SCRIPT_CLASS_FUNCTION(PlayerSpaceship, commandSetAutoRepair);
    REGISTER_SCRIPT_CLASS_FUNCTION(PlayerSpaceship, commandSetShieldFrequency);
    REGISTER_SCRIPT_CLASS_FUNCTION(PlayerSpaceship, commandAddWaypoint);
    REGISTER_SCRIPT_CLASS_FUNCTION(PlayerSpaceship, commandRemoveWaypoint);
    REGISTER_SCRIPT_CLASS_FUNCTION(PlayerSpaceship, commandMoveWaypoint);
    REGISTER_SCRIPT_CLASS_FUNCTION(PlayerSpaceship, commandActivateSelfDestruct);
    REGISTER_SCRIPT_CLASS_FUNCTION(PlayerSpaceship, commandCancelSelfDestruct);
    REGISTER_SCRIPT_CLASS_FUNCTION(PlayerSpaceship, commandConfirmDestructCode);
    REGISTER_SCRIPT_CLASS_FUNCTION(PlayerSpaceship, commandSetScienceLink);
    REGISTER_SCRIPT_CLASS_FUNCTION(PlayerSpaceship, commandSetProbe3DLink);
    REGISTER_SCRIPT_CLASS_FUNCTION(PlayerSpaceship, commandSetAlertLevel);

    // Return the number of Engineering repair crews on the ship.
    REGISTER_SCRIPT_CLASS_FUNCTION(PlayerSpaceship, getRepairCrewCount);
    // Set the total number of Engineering repair crews. If this value is less
    // than the number of repair crews, this function removes repair crews.
    // If the value is greater, it adds new repair crews at random locations.
    REGISTER_SCRIPT_CLASS_FUNCTION(PlayerSpaceship, setRepairCrewCount);
    // Sets whether automatic coolant distribution is enabled. This sets the
    // amount of coolant proportionally to the amount of heat in that system.
    // Use this command on ships to require less player interaction, especially
    // when combined with commandSetAutoRepair/auto_repair_enabled.
    REGISTER_SCRIPT_CLASS_FUNCTION(PlayerSpaceship, setAutoCoolant);
    // Set a password to join the ship.
    REGISTER_SCRIPT_CLASS_FUNCTION(PlayerSpaceship, setControlCode);
}

float PlayerSpaceship::weapons_radar_range = 0;
float PlayerSpaceship::energy_warp_per_second = 0;
float PlayerSpaceship::energy_shield_use_per_second = 0;
float PlayerSpaceship::system_heatup_per_second = 0;
float PlayerSpaceship::system_power_level_change_per_second = 0;
float PlayerSpaceship::energy_transfer_per_second = 0;
float PlayerSpaceship::heat_transfer_per_second = 0;
float PlayerSpaceship::repair_per_second = 0;
float PlayerSpaceship::cargo_repair_per_second = 0;
float PlayerSpaceship::system_coolant_level_change_per_second = 0;
float PlayerSpaceship::max_coolant_per_system = 0;
float PlayerSpaceship::damage_per_second_on_overheat = 0;
float PlayerSpaceship::shield_calibration_time = 0;
float PlayerSpaceship::comms_channel_open_time = 0;
float PlayerSpaceship::scan_probe_charge_time = 0;
float PlayerSpaceship::max_scanning_delay = 0;
float PlayerSpaceship::warp_terrain_cap = 0;
float PlayerSpaceship::warp_calibration_time = 0;
float PlayerSpaceship::warp_calibration_penalty_heat_factor = 0;
float PlayerSpaceship::over_fix_heat_factor = 0;
float PlayerSpaceship::drones_energy_factor = 0;
float PlayerSpaceship::system_power_user_factor[] = {
    /*SYS_Reactor*/     -25.0 * 0.08,
    /*SYS_BeamWeapons*/   3.0 * 0.08,
    /*SYS_MissileSystem*/ 1.0 * 0.08,
    /*SYS_Maneuver*/      2.0 * 0.08,
    /*SYS_Impulse*/       4.0 * 0.08,
    /*SYS_Warp*/          5.0 * 0.08,
    /*SYS_JumpDrive*/     5.0 * 0.08,
    /*SYS_FrontShield*/   5.0 * 0.08,
    /*SYS_RearShield*/    5.0 * 0.08,
    /*SYS_Docks*/         1.0 * 0.08,
    /*SYS_Drones*/        3.0 * 0.08,
};


string alertLevelToString(EAlertLevel level)
{
    // Convert an EAlertLevel to a string.
    switch(level)
    {
    case AL_RedAlert: return "RED ALERT";
    case AL_YellowAlert: return "YELLOW ALERT";
    case AL_Normal: return "Normal";
    default:
        return "???";
    }
}

REGISTER_MULTIPLAYER_CLASS(PlayerSpaceship, "PlayerSpaceship");
PlayerSpaceship::PlayerSpaceship()
: SpaceShip("PlayerSpaceship", 10000)
{
    // Initialize ship settings
    main_screen_setting = MSS_Front;
    main_screen_overlay = MSO_HideComms;
    hull_damage_indicator = 0.0;
    jump_indicator = 0.0;
    comms_state = CS_Inactive;
    mid_range_comms_state = CS_Inactive;
    comms_open_delay = 0.0;
    shield_calibration_delay = 0.0;
    warp_calibration_delay = 0.0;
    auto_repair_enabled = false;
    auto_coolant_enabled = false;
    max_coolant = max_coolant_per_system;
    activate_self_destruct = false;
    self_destruct_countdown = 0.0;
    scanning_delay = 0.0;
    scanning_target_id = -1;
    max_scan_probes = 8;
    scan_probe_stock = max_scan_probes;
    scan_probe_recharge = 0.0;
    alert_level = AL_Normal;
    shields_active = false;
    control_code = "";
    warp_indicator = 0;
    auto_repairing_system = SYS_None;
    engineering_control_from_bridge = false;
    science_query_to_bridge_db = "";
    setFactionId(1);

    extern_log_size = PreferencesManager::get("extern_log_size", "100").toInt();
    intern_log_size = PreferencesManager::get("intern_log_size", "100").toInt();
    excalibur_log_size = PreferencesManager::get("excalibur_log_size", "100").toInt();
    intern_in_query = false;
    excalibur_in_query = false;
    // For now, set player ships to always be fully scanned to all other ships
    for(unsigned int faction_id = 0; faction_id < factionInfo.size(); faction_id++)
        setScannedStateForFaction(faction_id, SS_FullScan);

    updateMemberReplicationUpdateDelay(&target_rotation, 0.1);
    registerMemberReplication(&hull_damage_indicator, 0.5);
    registerMemberReplication(&jump_indicator, 0.5);
    registerMemberReplication(&energy_level, 0.1);
    registerMemberReplication(&max_energy_level);
    registerMemberReplication(&main_screen_setting);
    registerMemberReplication(&main_screen_overlay);
    registerMemberReplication(&scanning_delay, 0.5);
    registerMemberReplication(&scanning_target_id);
    registerMemberReplication(&shields_active);
    registerMemberReplication(&shield_calibration_delay, 0.5);
    registerMemberReplication(&warp_calibration_delay, 0.5);
    registerMemberReplication(&auto_repair_enabled);
    registerMemberReplication(&max_coolant);
    registerMemberReplication(&auto_coolant_enabled);
    registerMemberReplication(&beam_system_target);
    registerMemberReplication(&mid_range_comms_state);
    registerMemberReplication(&comms_state);
    registerMemberReplication(&comms_open_delay, 1.0);
    registerMemberReplication(&comms_reply_message);
    registerMemberReplication(&mid_range_comms_target_name);
    registerMemberReplication(&comms_target_name);
    registerMemberReplication(&comms_incomming_message);
    for(int wp = 0; wp < max_waypoints; wp++) {
        waypoints[wp] = empty_waypoint;
        registerMemberReplication(&waypoints[wp]);
    }
    for(int r = 0; r < max_routes; r++) {
        for(int wp = 0; wp < max_waypoints_in_route; wp++) {
            routes[r][wp] = empty_waypoint;
            registerMemberReplication(&routes[r][wp]);
        }
    }
    registerMemberReplication(&scan_probe_stock);
    registerMemberReplication(&activate_self_destruct);
    registerMemberReplication(&self_destruct_countdown, 0.2);
    registerMemberReplication(&alert_level);
    registerMemberReplication(&linked_science_probe_id);
    registerMemberReplication(&linked_probe_3D_id);
    registerMemberReplication(&control_code);
    registerMemberReplication(&custom_functions);
    registerMemberReplication(&auto_repairing_system);
    registerMemberReplication(&warp_calibration_delay);
    registerMemberReplication(&engineering_control_from_bridge);
    registerMemberReplication(&science_query_to_bridge_db);

    // Determine which stations must provide self-destruct confirmation codes.
    for(int n = 0; n < max_self_destruct_codes; n++)
    {
        self_destruct_code[n] = 0;
        self_destruct_code_confirmed[n] = false;
        self_destruct_code_entry_position[n] = helmsOfficer;
        self_destruct_code_show_position[n] = helmsOfficer;
        registerMemberReplication(&self_destruct_code[n]);
        registerMemberReplication(&self_destruct_code_confirmed[n]);
        registerMemberReplication(&self_destruct_code_entry_position[n]);
        registerMemberReplication(&self_destruct_code_show_position[n]);
    }

    // Initialize each subsystem to be powered with no coolant or heat.
    for(int n = 0; n < SYS_COUNT; n++)
    {
        systems[n].health = 1.0;
        systems[n].power_level = 1.0;
        systems[n].power_request = 1.0;
        systems[n].coolant_level = 0.0;
        systems[n].coolant_level = 0.0;
        systems[n].heat_level = 0.0;

        registerMemberReplication(&systems[n].power_level);
        registerMemberReplication(&systems[n].power_request);
        registerMemberReplication(&systems[n].coolant_level);
        registerMemberReplication(&systems[n].coolant_request);
        registerMemberReplication(&systems[n].heat_level, 1.0);
    }
    for(int n = 0; n < max_science_tasks; n++)
    {
        scienceTasks[n].setParent(this);
        scienceTasks[n].setIndex(n);
    }
    
    if (game_server)
    {
        if (gameGlobalInfo->insertPlayerShip(this) < 0)
        {
            destroy();
        }
    }

    // Initialize player ship callsigns with a "PL" designation.
    setCallSign("PL" + string(getMultiplayerId()));

    addToShipLog("Start of extern log", colorConfig.log_generic, "extern");
    addToShipLog("Start of intern log", colorConfig.log_generic, "intern");
    addToShipLog("Excalibur 3.0 initialized", colorConfig.log_generic, "excalibur");
}

void PlayerSpaceship::update(float delta)
{
    // If we're flashing the screen for hull damage, tick the fade-out.
    if (hull_damage_indicator > 0)
        hull_damage_indicator -= delta;

    // If we're jumping, tick the countdown timer.
    if (jump_indicator > 0)
        jump_indicator -= delta;

    // If shields are calibrating, tick the calibration delay. Factor shield
    // subsystem effectiveness when determining the tick rate.
    if (shield_calibration_delay > 0)
    {
        shield_calibration_delay -= delta * (getSystemEffectiveness(SYS_FrontShield) + getSystemEffectiveness(SYS_RearShield)) / 2.0;
    }
    // If warp is are calibrating, tick the calibration delay. Factor shield
    // subsystem effectiveness when determining the tick rate.
    if (warp_calibration_delay > 0)
    {
        warp_request = 0;
        warp_calibration_delay -= delta * getSystemEffectiveness(SYS_Warp);
    }

    // Docking actions.
    if (docking_state == DS_Docked)
    {
        P<SpaceShip> docked_with_ship = docking_target;
        if (docked_with_ship && docked_with_ship->tryDockDrone(this))
        {
            // this drone has docked with a carrier
            destroy();
        } else {
            P<ShipTemplateBasedObject> docked_with_template_based = docking_target;
            P<SpaceShip> docked_with_ship = docking_target;

            // Derive a base energy request rate from the player ship's maximum
            // energy capacity.
            float energy_request = std::min(delta * 10.0f, max_energy_level - energy_level);

            // If we're docked with a shipTemplateBasedObject, and that object is
            // set to share its energy with docked ships, transfer energy from the
            // mothership to docked ships until the mothership runs out of energy
            // or the docked ship doesn't require any.
            if (docked_with_template_based && docked_with_template_based->shares_energy_with_docked)
            {
                if (!docked_with_ship || docked_with_ship->useEnergy(energy_request))
                    energy_level += energy_request;
            }
            // If a shipTemplateBasedObject and is allowed to restock
            // scan probes with docked ships.
            if (docked_with_template_based && docked_with_template_based->restocks_scan_probes && scan_probe_stock < max_scan_probes)
            {
                scan_probe_recharge += delta;
                if (scan_probe_recharge > scan_probe_charge_time)
                {
                    P<PlayerSpaceship> docked_with_player_ship = docked_with_template_based;
                    if (docked_with_player_ship && docked_with_player_ship->scan_probe_stock > 0){
                        docked_with_player_ship->scan_probe_stock -=1;
                        scan_probe_stock += 1;
                        scan_probe_recharge = 0.0;
                    } else {
                        scan_probe_stock += 1;
                        scan_probe_recharge = 0.0;
                    }
                }
            }
        }
    }else{
        scan_probe_recharge = 0.0;
    }

    // Automate cooling if auto_coolant_enabled is true. Distributes coolant to
    // subsystems proportionally to their share of the total generated heat.
    if (auto_coolant_enabled)
    {
        float total_heat = 0.0;

        for(int n = 0; n < SYS_COUNT; n++)
        {
            if (!hasSystem(ESystem(n))) continue;
            total_heat += systems[n].heat_level;
        }
        if (total_heat > 0.0)
        {
            for(int n = 0; n < SYS_COUNT; n++)
            {
                if (!hasSystem(ESystem(n))) continue;
                systems[n].coolant_request = max_coolant * systems[n].heat_level / total_heat;
            }
        }
    }

    // Actions performed on the server only.
    if (game_server)
    {
        // Comms actions
        if (comms_state == CS_OpeningChannel)
        {
            if (comms_open_delay > 0)
            {
                comms_open_delay -= delta;
            }else{
                if (!comms_target)
                {
                    comms_state = CS_ChannelBroken;
                }else{
                    comms_reply_id.clear();
                    comms_reply_message.clear();
                    P<PlayerSpaceship> playerShip = comms_target;
                    if (playerShip)
                    {
                        comms_open_delay = PlayerSpaceship::comms_channel_open_time;

                        if (playerShip->comms_state == CS_Inactive || playerShip->comms_state == CS_ChannelFailed || playerShip->comms_state == CS_ChannelBroken || playerShip->comms_state == CS_ChannelClosed)
                        {
                            playerShip->comms_state = CS_BeingHailed;
                            playerShip->comms_target = this;
                            playerShip->comms_target_name = getCallSign();
                        }
                    }else{
                        if (gameGlobalInfo->intercept_all_comms_to_gm)
                        {
                            comms_state = CS_ChannelOpenGM;
                        }else{
                            if (comms_script_interface.openCommChannel(this, comms_target))
                                comms_state = CS_ChannelOpen;
                            else
                                comms_state = CS_ChannelFailed;
                        }
                    }
                }
            }
        }
        if (comms_state == CS_ChannelOpen || comms_state == CS_ChannelOpenPlayer)
        {
            if (!comms_target)
                comms_state = CS_ChannelBroken;
        }

        // Consume power if shields are enabled.
        if (shields_active)
            useEnergy(delta * energy_shield_use_per_second);

        // auto-repair
        if (!gameGlobalInfo->use_repair_crew){
            ESystem system = ESystem(auto_repairing_system);
            if (system > SYS_None && system < SYS_COUNT && hasSystem(system))
            {
                systems[system].health += repair_per_second * delta;
                if (systems[system].health > 1.0){
                    addHeat(system, over_fix_heat_factor * (systems[system].health - 1));
                    systems[system].health = 1.0;
                }
            }
            if (auto_repair_enabled && (system == SYS_None || !hasSystem(system) || systems[system].health == 1.0))
            {
                int n=irandom(0, SYS_COUNT - 1);

                if (hasSystem(ESystem(n)) && systems[n].health < 1.0)
                {
                    auto_repairing_system = ESystem(n);
                }
            }
        }

        // Consume power based on subsystem requests and state.
        energy_level += delta * getNetSystemEnergyUsage();

        for(int n = 0; n < SYS_COUNT; n++)
        {
            if (!hasSystem(ESystem(n))) continue;

            if (systems[n].power_request > systems[n].power_level)
            {
                systems[n].power_level += delta * system_power_level_change_per_second;
                if (systems[n].power_level > systems[n].power_request)
                    systems[n].power_level = systems[n].power_request;
            }
            else if (systems[n].power_request < systems[n].power_level)
            {
                systems[n].power_level -= delta * system_power_level_change_per_second;
                if (systems[n].power_level < systems[n].power_request)
                    systems[n].power_level = systems[n].power_request;
            }

            if (systems[n].coolant_request > systems[n].coolant_level)
            {
                systems[n].coolant_level += delta * system_coolant_level_change_per_second;
                if (systems[n].coolant_level > systems[n].coolant_request)
                    systems[n].coolant_level = systems[n].coolant_request;
            }
            else if (systems[n].coolant_request < systems[n].coolant_level)
            {
                systems[n].coolant_level -= delta * system_coolant_level_change_per_second;
                if (systems[n].coolant_level < systems[n].coolant_request)
                    systems[n].coolant_level = systems[n].coolant_request;
            }

            // Add heat to overpowered subsystems.
            addHeat(ESystem(n), delta * systems[n].getHeatingDelta() * system_heatup_per_second);
        }

        // If reactor health is worse than -90% and overheating, it explodes,
        // destroying the ship and damaging a 0.5U radius.
        if (systems[SYS_Reactor].health < -0.9 && systems[SYS_Reactor].heat_level == 1.0 && getCanBeDestroyed())
        {
            ExplosionEffect* e = new ExplosionEffect();
            e->setSize(1000.0f);
            e->setPosition(getPosition());

            DamageInfo info(this, DT_Kinetic, getPosition());
            SpaceObject::damageArea(getPosition(), 500, 30, 60, info, 0.0);

            destroy();
            return;
        }

        if (energy_level < 0.0)
            energy_level = 0.0;

        // If the ship has less than 10 energy, drop shields automatically.
        if (energy_level < 10.0)
        {
            shields_active = false;
        }

        // If a ship is jumping or warping, consume additional energy.
        if (has_warp_drive && warp_request > 0 && !(has_jump_drive && jump_delay > 0))
        {
            // If warping, consume energy at a rate of the warp request.
            // If shields are up, that rate is increased by an additional 50%.
            if (!useEnergy(energy_warp_per_second * delta * std::min<float>(warp_terrain_cap, warp_request) * (shields_active ? 1.5 : 1.0)))
                // If there's not enough energy, fall out of warp.
                warp_request = 0;
                
            for(float n=0; n<=4; n++)
            {
                if ((current_warp > n-0.1 && current_warp < n+0.1) && warp_indicator != n)
                {
                    warp_indicator = n;
                    addToShipLog("WARP " + string(abs(n)), sf::Color::White,"intern");
                }
            }
        }

        if (getScanTarget())
        {
            // If the scan setting or a target's scan complexity is none/0,
            // complete the scan after a delay.
            if (getScanTarget()->scanningComplexity(this) < 1)
            {
                scanning_delay -= delta;
                if (scanning_delay < 0)
                {
                    getScanTarget()->scannedBy(this);
                    scanning_target_id = -1;
                }
            }
        }else{
            // Otherwise, ignore the scanning_delay setting.
            scanning_delay = 0.0;
            scanning_target_id = -1;
        }

        if (activate_self_destruct)
        {
            // If self-destruct has been activated but not started ...
            if (self_destruct_countdown <= 0.0)
            {
                bool do_self_destruct = true;
                // ... wait until the confirmation codes are entered.
                for(int n = 0; n < max_self_destruct_codes; n++)
                    if (!self_destruct_code_confirmed[n])
                        do_self_destruct = false;

                // Then start and announce the 10-second countdown.
                if (do_self_destruct)
                {
                    self_destruct_countdown = 10.0f;
                    playSoundOnMainScreen("vocal_self_destruction.wav");
                }
            }else{
                // If the countdown has started, tick the clock.
                self_destruct_countdown -= delta;

                // When time runs out, blow up the ship and damage a 1.5U
                // radius.
                if (self_destruct_countdown <= 0.0)
                {
                    for(int n = 0; n < 5; n++)
                    {
                        ExplosionEffect* e = new ExplosionEffect();
                        e->setSize(1000.0f);
                        e->setPosition(getPosition() + sf::rotateVector(sf::Vector2f(0, random(0, 500)), random(0, 360)));
                    }

                    DamageInfo info(this, DT_Kinetic, getPosition());
                    SpaceObject::damageArea(getPosition(), 1500, 100, 200, info, 0.0);

                    destroy();
                    return;
                }
            }
        }
        
    }else{
        // Actions performed on the client-side only.

        // If scan settings or the scan target's complexity is 0/none, tick
        // the scan delay timer.
        if (getScanTarget() && getScanTarget()->scanningComplexity(this) < 1)
        {
            if (scanning_delay > 0.0)
                scanning_delay -= delta;
        }

        // If opening comms, tick the comms open delay timer.
        if (comms_open_delay > 0)
            comms_open_delay -= delta;
    }

    // Perform all other ship update actions.
    SpaceShip::update(delta);

    // Cap energy at the max_energy_level.
    if (energy_level > max_energy_level)
        energy_level = max_energy_level;

    for(int n = 0; n < max_science_tasks; n++) {
        scienceTasks[n].update(delta);
    }
}

void PlayerSpaceship::applyTemplateValues()
{
    // Apply default spaceship object values first.
    SpaceShip::applyTemplateValues();

    // Override whether the ship has jump and warp drives based on the server
    // setting.
    switch(gameGlobalInfo->player_warp_jump_drive_setting)
    {
    default:
        break;
    case PWJ_WarpDrive:
        setWarpDrive(true);
        setJumpDrive(false);
        break;
    case PWJ_JumpDrive:
        setWarpDrive(false);
        setJumpDrive(true);
        break;
    case PWJ_WarpAndJumpDrive:
        setWarpDrive(true);
        setJumpDrive(true);
        break;
    case PWJ_None:
        setWarpDrive(false);
        setJumpDrive(false);
        break;
    }

    if (gameGlobalInfo->use_repair_crew){
        // Set the ship's number of repair crews in Engineering from the ship's
        // template.
        setRepairCrewCount(ship_template->repair_crew_count);
    }
}

void PlayerSpaceship::executeJump(float distance)
{
    // When jumping, reset the jump effect and move the ship.
    jump_indicator = 2.0;
    SpaceShip::executeJump(distance);
}

P<SpaceObject> PlayerSpaceship::getScanTarget()
{
    if (scanning_target_id == -1)
        return NULL;
    return getObjectById(scanning_target_id);
}

void PlayerSpaceship::takeHullDamage(float damage_amount, DamageInfo& info)
{
    // If taking non-EMP damage, light up the hull damage overlay.
    if (info.type != DT_EMP)
    {
        hull_damage_indicator = 1.5;
    }

    // Take hull damage like any other ship.
    
    float systems_diff[SYS_COUNT];
    for(int n=0; n<SYS_COUNT; n++)
    {
        systems_diff[n] = systems[n].health;
    }
    SpaceShip::takeHullDamage(damage_amount, info);
    
    // Infos for log intern
    string system_damage = string(abs(damage_amount)) + string(" damages to hull. System affected : ");
    for(int n=0; n<SYS_COUNT; n++)
    {
        if(systems_diff[n] != systems[n].health)
            system_damage = system_damage + string(getSystemName(ESystem(n))) + string(" ");
    }
    addToShipLog(system_damage,sf::Color::Red,"intern");
}

void PlayerSpaceship::setMaxCoolant(float coolant)
{
    max_coolant = std::max(coolant, 0.0f);
    float total_coolant = 0;

    for(int n = 0; n < SYS_COUNT; n++)
    {
        if (!hasSystem(ESystem(n))) continue;

        total_coolant += systems[n].coolant_request;
    }

    if (total_coolant > max_coolant)
    {
        for(int n = 0; n < SYS_COUNT; n++)
        {
            if (!hasSystem(ESystem(n))) continue;

            systems[n].coolant_request *= max_coolant / total_coolant;
        }
    } else {
        if (total_coolant > 0)
        {
            for(int n = 0; n < SYS_COUNT; n++)
            {
                if (!hasSystem(ESystem(n))) continue;
                systems[n].coolant_request = std::min(systems[n].coolant_request * max_coolant / total_coolant, (float) max_coolant_per_system);
            }
        }
    }
}

void PlayerSpaceship::setSystemCoolantRequest(ESystem system, float request)
{
    request = std::max(0.0f, std::min(request, std::min((float) max_coolant_per_system, max_coolant)));
    // Set coolant levels on a system.
    float total_coolant = 0;
    int cnt = 0;
    for(int n = 0; n < SYS_COUNT; n++)
    {
        if (!hasSystem(ESystem(n))) continue;
        if (n == system) continue;

        total_coolant += systems[n].coolant_request;
        cnt++;
    }
    if (total_coolant > max_coolant - request)
    {
        for(int n = 0; n < SYS_COUNT; n++)
        {
            if (!hasSystem(ESystem(n))) continue;
            if (n == system) continue;

            systems[n].coolant_request *= (max_coolant - request) / total_coolant;
        }
    }else{
        if (total_coolant > 0)
        {
            for(int n = 0; n < SYS_COUNT; n++)
            {
                if (!hasSystem(ESystem(n))) continue;
                if (n == system) continue;

                systems[n].coolant_request = std::min(systems[n].coolant_request * (max_coolant - request) / total_coolant, (float) max_coolant_per_system);
            }
        }
    }

    systems[system].coolant_request = request;
}

bool PlayerSpaceship::useEnergy(float amount)
{
    // Try to consume an amount of energy. If it works, return true.
    // If it doesn't, return false.
    if (energy_level >= amount)
    {
        energy_level -= amount;
        return true;
    }
    return false;
}

void PlayerSpaceship::addHeat(ESystem system, float amount)
{
    // Add heat to a subsystem if it's present.
    if (!hasSystem(system)) return;

    systems[system].heat_level += amount;

    if (systems[system].heat_level > 1.0)
    {
        float overheat = systems[system].heat_level - 1.0;
        systems[system].heat_level = 1.0;

        if (gameGlobalInfo->use_system_damage)
        {
            // Heat damage is specified as damage per second while overheating.
            // Calculate the amount of overheat back to a time, and use that to
            // calculate the actual damage taken.
            systems[system].health -= overheat / system_heatup_per_second * damage_per_second_on_overheat;

            if (systems[system].health < -1.0)
                systems[system].health = -1.0;
        }
    }

    if (systems[system].heat_level < 0.0)
        systems[system].heat_level = 0.0;
}

void PlayerSpaceship::playSoundOnMainScreen(string sound_name)
{
    sf::Packet packet;
    packet << CMD_PLAY_CLIENT_SOUND;
    packet << max_crew_positions;
    packet << sound_name;
    broadcastServerCommand(packet);
}

float PlayerSpaceship::getNetSystemEnergyUsage()
{
    // Get the net delta of energy draw for subsystems.
    float net_power = 0.0;

    // Determine each subsystem's energy draw.
    for(int n = 0; n < SYS_COUNT; n++)
    {
        if (!hasSystem(ESystem(n))) continue;
        // Factor the subsystem's health into energy generation.
        if (system_power_user_factor[n] < 0)
        {
            float f = getSystemEffectiveness(ESystem(n));
            if (f > 1.0f)
                f = (1.0f + f) / 2.0f;
            net_power -= system_power_user_factor[n] * f;
        }
        else
        {
            float shipTypeFactor = 1.f;
            if(ship_template && ship_template->getType() == ShipTemplate::TemplateType::Drone){
                shipTypeFactor = drones_energy_factor;
            }
            net_power -= system_power_user_factor[n] * systems[n].power_level * shipTypeFactor;
        }
    }

    // Return the net subsystem energy draw.
    return net_power;
}

int PlayerSpaceship::getRepairCrewCount()
{
    // Count and return the number of repair crews on this ship.
    return getRepairCrewFor(this).size();
}

void PlayerSpaceship::setRepairCrewCount(int amount)
{
    // This is a server-only function, and we only care about repair crews when
    // we care about subsystem damage.
    if (!game_server || !gameGlobalInfo->use_system_damage)
        return;

    // Prevent negative values.
    amount = std::max(0, amount);

    // Get the number of repair crews for this ship.
    PVector<RepairCrew> crew = getRepairCrewFor(this);

    // Remove excess crews by shifting them out of the array.
    while(int(crew.size()) > amount)
    {
        crew[0]->destroy();
        crew.update();
    }

    // Add crews until we reach the provided amount.
    for(int create_amount = amount - crew.size(); create_amount > 0; create_amount--)
    {
        P<RepairCrew> rc = new RepairCrew();
        rc->ship_id = getMultiplayerId();
    }
}

void PlayerSpaceship::transferPlayersToShip(P<PlayerSpaceship> other_ship)
{
    // Don't do anything without a valid target. The target must be a
    // PlayerSpaceship.
    if (!other_ship)
        return;

    // For each player, move them to the same station on the target.
    foreach(PlayerInfo, i, player_info_list)
    {
        if (i->ship_id == getMultiplayerId())
        {
            i->ship_id = other_ship->getMultiplayerId();
        }
    }
}

void PlayerSpaceship::transferPlayersAtPositionToShip(ECrewPosition position, P<PlayerSpaceship> other_ship)
{
    // Don't do anything without a valid target. The target must be a
    // PlayerSpaceship.
    if (!other_ship)
        return;

    // For each player, check which position they fill. If the position matches
    // the requested position, move that player. Otherwise, ignore them.
    foreach(PlayerInfo, i, player_info_list)
    {
        if (i->ship_id == getMultiplayerId() && i->crew_position[position])
        {
            i->ship_id = other_ship->getMultiplayerId();
        }
    }
}

bool PlayerSpaceship::hasPlayerAtPosition(ECrewPosition position)
{
    // If a position is occupied by a player, return true.
    // Otherwise, return false.
    foreach(PlayerInfo, i, player_info_list)
    {
        if (i->ship_id == getMultiplayerId() && i->crew_position[position])
        {
            return true;
        }
    }
    return false;
}

void PlayerSpaceship::addCustomButton(ECrewPosition position, string name, string caption, ScriptSimpleCallback callback)
{
    removeCustom(name);
    custom_functions.emplace_back();
    CustomShipFunction& csf = custom_functions.back();
    csf.type = CustomShipFunction::Type::Button;
    csf.name = name;
    csf.crew_position = position;
    csf.caption = caption;
    csf.callback = callback;
}

void PlayerSpaceship::addCustomInfo(ECrewPosition position, string name, string caption)
{
    removeCustom(name);
    custom_functions.emplace_back();
    CustomShipFunction& csf = custom_functions.back();
    csf.type = CustomShipFunction::Type::Info;
    csf.name = name;
    csf.crew_position = position;
    csf.caption = caption;
}

void PlayerSpaceship::addCustomMessage(ECrewPosition position, string name, string caption)
{
    removeCustom(name);
    custom_functions.emplace_back();
    CustomShipFunction& csf = custom_functions.back();
    csf.type = CustomShipFunction::Type::Message;
    csf.name = name;
    csf.crew_position = position;
    csf.caption = caption;
}

void PlayerSpaceship::addCustomMessageWithCallback(ECrewPosition position, string name, string caption, ScriptSimpleCallback callback)
{
    removeCustom(name);
    custom_functions.emplace_back();
    CustomShipFunction& csf = custom_functions.back();
    csf.type = CustomShipFunction::Type::Message;
    csf.name = name;
    csf.crew_position = position;
    csf.caption = caption;
    csf.callback = callback;
}

void PlayerSpaceship::removeCustom(string name)
{
    for(auto it = custom_functions.begin(); it != custom_functions.end();)
    {
        if (it->name == name)
            it = custom_functions.erase(it);
        else
            it++;
    }
}

void PlayerSpaceship::setCommsMessage(string message)
{
    // Record a new comms message to the ship's log.
    for(string line : message.split("\n"))
        addToShipLog(line, sf::Color(192, 192, 255));
    // Display the message in the messaging window.
    comms_incomming_message = message;
}

void PlayerSpaceship::addCommsIncommingMessage(string message)
{
    // Record incoming comms messages to the ship's log.
    for(string line : message.split("\n"))
        addToShipLog(line, sf::Color(192, 192, 255));
    // Add the message to the messaging window.
    comms_incomming_message = comms_incomming_message + "\n> " + message;
}

void PlayerSpaceship::addCommsOutgoingMessage(string message)
{
    // Record outgoing comms messages to the ship's log.
    for(string line : message.split("\n"))
        addToShipLog(line, colorConfig.log_send);
    // Add the message to the messaging window.
    comms_incomming_message = comms_incomming_message + "\n< " + message;
}

void PlayerSpaceship::addCommsReply(int32_t id, string message)
{
    if (comms_reply_id.size() >= 200)
        return;
    comms_reply_id.push_back(id);
    comms_reply_message.push_back(message);
}

bool PlayerSpaceship::hailCommsByGM(string target_name)
{
    // If a ship's comms aren't engaged, receive the GM's hail.
    // Otherwise, return false.
    if (!isCommsInactive() && !isCommsFailed() && !isCommsBroken() && !isCommsClosed())
        return false;

    // Log the hail.
    addToShipLog("Hailed by " + target_name, colorConfig.log_generic);

    // Set comms to the hail state and notify Relay/comms.
    comms_state = CS_BeingHailedByGM;
    comms_target_name = target_name;
    comms_target = nullptr;
    return true;
}

bool PlayerSpaceship::hailByObject(P<SpaceObject> object, string opening_message)
{
    // If trying to open comms with a non-object, return false.
    if (isCommsOpening() || isCommsBeingHailed())
    {
        if (comms_target != object)
        {
            return false;
        }
    }

    // If comms are engaged, return false.
    if (isCommsBeingHailedByGM())
    {
        return false;
    }
    if (isCommsChatOpen() || isCommsScriptOpen())
    {
        return false;
    }

    // Receive a hail from the object.
    comms_target = object;
    comms_target_name = object->getCallSign();
    comms_state = CS_BeingHailed;
    comms_incomming_message = opening_message;
    return true;
}

void PlayerSpaceship::closeComms()
{
    // If comms are closed, state it and log it to the ship's log.
    if (comms_state != CS_Inactive)
    {
        if (comms_state == CS_ChannelOpenPlayer && comms_target)
        {
            P<PlayerSpaceship> player_ship = comms_target;
            player_ship->comms_state = CS_ChannelClosed;
            player_ship->addToShipLog("Communication channel closed by other side", colorConfig.log_generic);
        }
        if (comms_state == CS_OpeningChannel && comms_target)
        {
            P<PlayerSpaceship> player_ship = comms_target;
            if (player_ship)
            {
                if (player_ship->comms_state == CS_BeingHailed && player_ship->comms_target == this)
                {
                    player_ship->comms_state = CS_Inactive;
                    player_ship->addToShipLog("Hailing from " + getCallSign() + " stopped", colorConfig.log_generic);
                }
            }
        }
        addToShipLog("Communication channel closed", colorConfig.log_generic);
        if (comms_state == CS_ChannelOpenGM)
            comms_state = CS_ChannelClosed;
        else
            comms_state = CS_Inactive;
    }
}

void PlayerSpaceship::acceptedMidRangeCall(string response){
    if (mid_range_comms_state == CS_OpeningChannel){
        mid_range_comms_state = CS_Inactive;
        addToShipLog("Opened voice channel to " + mid_range_comms_target_name, colorConfig.log_generic);
    }
}

void PlayerSpaceship::closeMidRangeComms(string response){
    if (mid_range_comms_state == CS_OpeningChannel){
        mid_range_comms_state = CS_Inactive;
        addToShipLog("Voice communication to "+ mid_range_comms_target_name+ " failed: " + response, colorConfig.log_generic); 
    }
}

void PlayerSpaceship::handleClientCommand(int32_t client_id, int16_t command, sf::Packet& packet)
{
    switch(command)
    {
    case CMD_WARP:
        if (warp_calibration_delay <= 0.0){
            packet >> warp_request;
        }
        break;
    case CMD_SET_SHIELDS:
        {
            bool active;
            packet >> active;

            if (shield_calibration_delay <= 0.0 && active != shields_active)
            {
                shields_active = active;
                if (active)
                {
                    soundManager->playSound("shield_up.wav");
                    addToShipLog("Shields on",sf::Color::Green,"intern");
                }
                else
                {
                    soundManager->playSound("shield_down.wav");
                    addToShipLog("Shields off",sf::Color::Green,"intern");
                }
            }
        }
        break;
    case CMD_SET_MAIN_SCREEN_SETTING:
        packet >> main_screen_setting;
        break;
    case CMD_SET_MAIN_SCREEN_OVERLAY:
        packet >> main_screen_overlay;
        break;
    case CMD_SCAN_OBJECT:
        {
            int32_t id;
            packet >> id;

            P<SpaceObject> scanning_target = game_server->getObjectById(id);
            if (scanning_target && scanning_target->canBeScannedBy(this))
            {
                scanning_target_id = id;
                scanning_delay = max_scanning_delay;
            }
        }
        break;
    case CMD_SCAN_DONE:
        if (getScanTarget())
        {
            getScanTarget()->scannedBy(this);
            scanning_target_id = -1;
        }
        break;
    case CMD_SCAN_CANCEL:
            scanning_target_id = -1;
        break;
    case CMD_SET_SYSTEM_POWER_REQUEST:
        {
            ESystem system;
            float request;
            packet >> system >> request;
            if (system < SYS_COUNT && request >= 0.0 && request <= 3.0)
                systems[system].power_request = request;
        }
        break;
    case CMD_SET_SYSTEM_COOLANT_REQUEST:
        {
            ESystem system;
            float request;
            packet >> system >> request;
            if (system < SYS_COUNT && request >= 0.0 && request <= 10.0)
                setSystemCoolantRequest(system, request);
        }
        break;
    case CMD_SET_ALL_SYSTEMS_COOLANT_REQUESTS:
        {
            float values[SYS_COUNT];
            float totalValue = 0;

            for(int n=0; n<SYS_COUNT; n++){
                packet >> values[n];
                totalValue += values[n];
            }
            for(int n=0; n<SYS_COUNT; n++){
                systems[n].coolant_request = values[n] * max_coolant / totalValue;
            }
        }
        break;
    case CMD_OPEN_MID_RANGE_COMM:
        {
            int32_t id;
            packet >> id;
            if (mid_range_comms_state == CS_Inactive || mid_range_comms_state == CS_BeingHailed || mid_range_comms_state == CS_BeingHailedByGM || mid_range_comms_state == CS_ChannelClosed)
            {
                mid_range_comms_target = game_server->getObjectById(id);
                if (mid_range_comms_target && length(getPosition() - mid_range_comms_target->getPosition()) < gameGlobalInfo->long_range_radar_range)
                {
                    mid_range_comms_state = CS_OpeningChannel;
                    comms_open_delay = comms_channel_open_time;
                    mid_range_comms_target_name = mid_range_comms_target->getCallSign();
                    comms_incomming_message = "Opened mid-range comms with " + mid_range_comms_target_name;
                    addToShipLog("Hailing (mid range): " + mid_range_comms_target_name, colorConfig.log_generic);
                    new ActionItem(string("Calling: " + mid_range_comms_target_name), string("(type reason if declining)"), 
                        [this](string response){acceptedMidRangeCall(response);}, 
                        [this](string response){closeMidRangeComms(response);});
                } else {
                    mid_range_comms_state = CS_Inactive;
                }
            }
        }
        break;
    case CMD_OPEN_TEXT_COMM:
        if (comms_state == CS_Inactive || comms_state == CS_BeingHailed || comms_state == CS_BeingHailedByGM || comms_state == CS_ChannelClosed)
        {
            int32_t id;
            packet >> id;
            comms_target = game_server->getObjectById(id);
            if (comms_target)
            {
                P<PlayerSpaceship> player = comms_target;
                comms_state = CS_OpeningChannel;
                comms_open_delay = comms_channel_open_time;
                comms_target_name = comms_target->getCallSign();
                comms_incomming_message = "Opened comms with " + comms_target_name;
                addToShipLog("Hailing (long range): " + comms_target_name, colorConfig.log_generic);
            }else{
                comms_state = CS_Inactive;
            }
        }
        break;
    case CMD_CLOSE_TEXT_COMM:
        closeComms();
        break;
    case CMD_ANSWER_COMM_HAIL:
        if (comms_state == CS_BeingHailed)
        {
            bool anwser;
            packet >> anwser;
            P<PlayerSpaceship> playerShip = comms_target;

            if (playerShip)
            {
                if (anwser)
                {
                    comms_state = CS_ChannelOpenPlayer;
                    playerShip->comms_state = CS_ChannelOpenPlayer;

                    comms_incomming_message = "Opened comms to " + playerShip->getCallSign();
                    playerShip->comms_incomming_message = "Opened comms to " + getCallSign();
                    addToShipLog("Opened communication channel to " + playerShip->getCallSign(), colorConfig.log_generic);
                    playerShip->addToShipLog("Opened communication channel to " + getCallSign(), colorConfig.log_generic);
                }else{
                    addToShipLog("Refused communications from " + playerShip->getCallSign(), colorConfig.log_generic);
                    playerShip->addToShipLog("Refused communications to " + getCallSign(), colorConfig.log_generic);
                    comms_state = CS_Inactive;
                    playerShip->comms_state = CS_ChannelFailed;
                }
            }else{
                if (anwser)
                {
                    if (!comms_target)
                    {
                        addToShipLog("Hail suddenly went dead.", colorConfig.log_generic);
                        comms_state = CS_ChannelBroken;
                    }else{
                        addToShipLog("Accepted hail from " + comms_target->getCallSign(), colorConfig.log_generic);
                        comms_reply_id.clear();
                        comms_reply_message.clear();
                        if (comms_incomming_message == "")
                        {
                            if (comms_script_interface.openCommChannel(this, comms_target))
                                comms_state = CS_ChannelOpen;
                            else
                                comms_state = CS_ChannelFailed;
                        }else{
                            // Set the comms message again, so it ends up in
                            // the ship's log.
                            // comms_incomming_message was set by
                            // "hailByObject", without ending up in the log.
                            setCommsMessage(comms_incomming_message);
                            comms_state = CS_ChannelOpen;
                        }
                    }
                }else{
                    if (comms_target)
                        addToShipLog("Refused hail from " + comms_target->getCallSign(), colorConfig.log_generic);
                    comms_state = CS_Inactive;
                }
            }
        }
        if (comms_state == CS_BeingHailedByGM)
        {
            bool anwser;
            packet >> anwser;

            if (anwser)
            {
                comms_state = CS_ChannelOpenGM;

                addToShipLog("Opened communication channel to " + comms_target_name, colorConfig.log_generic);
                comms_incomming_message = "Opened comms with " + comms_target_name;
            }else{
                addToShipLog("Refused hail from " + comms_target_name, colorConfig.log_generic);
                comms_state = CS_Inactive;
            }
        }
        break;
    case CMD_SEND_TEXT_COMM:
        if (comms_state == CS_ChannelOpen && comms_target)
        {
            uint8_t index;
            packet >> index;
            if (index < comms_reply_id.size())
            {
                addToShipLog(comms_reply_message[index], colorConfig.log_send);

                comms_incomming_message = "?";
                int id = comms_reply_id[index];
                comms_reply_id.clear();
                comms_reply_message.clear();
                comms_script_interface.commChannelMessage(id);
            }
        }
        break;
    case CMD_SEND_TEXT_COMM_PLAYER:
        if (comms_state == CS_ChannelOpenPlayer || comms_state == CS_ChannelOpenGM)
        {
            string message;
            packet >> message;

            addCommsOutgoingMessage(message);
            P<PlayerSpaceship> playership = comms_target;
            if (comms_state == CS_ChannelOpenPlayer && playership)
                playership->addCommsIncommingMessage(message);
        }
        break;
    case CMD_SET_AUTO_REPAIR:
        {
            packet >> auto_repair_enabled;
            addToShipLog("Auto repair enabled",sf::Color::White,"intern");
        }
        break;
    case CMD_SET_SHIELD_FREQUENCY:
        if (shield_calibration_delay <= 0.0)
        {
            int32_t new_frequency;
            packet >> new_frequency;
            if (new_frequency != shield_frequency)
            {
                shield_frequency = new_frequency;
                shield_calibration_delay = shield_calibration_time;
                shields_active = false;
                if (shield_frequency < 0)
                    shield_frequency = 0;
                if (shield_frequency > SpaceShip::max_frequency)
                    shield_frequency = SpaceShip::max_frequency;
                addToShipLog("Shields frequency changed : " + frequencyToString(new_frequency),sf::Color::Green,"intern");
            }
        }
        break;
    case CMD_ADD_WAYPOINT:
        {
            sf::Vector2f position;
            packet >> position;
            for (int i = 0; i < max_waypoints; i++){
                if (waypoints[i] >= empty_waypoint){
                    waypoints[i] = position;
                    break;
                }
            }
        }
        break;
    case CMD_REMOVE_WAYPOINT:
        {
            int32_t index;
            packet >> index;
            if (index >= 0 && index < max_waypoints)
                waypoints[index] = empty_waypoint;
        }
        break;
    case CMD_MOVE_WAYPOINT:
        {
            int32_t index;
            sf::Vector2f position;
            packet >> index >> position;
            if (index >= 0 && index < max_waypoints)
                waypoints[index] = position;
        }
        break;
    case CMD_ADD_ROUTE_WAYPOINT:
        {
            int route;
            sf::Vector2f position;
            packet >> route >> position;
            for (int i = 0; i < max_waypoints_in_route; i++){
                if (routes[route][i] >= empty_waypoint){
                    routes[route][i] = position;
                    break;
                }
            }
        }
        break;
    case CMD_REMOVE_ROUTE_WAYPOINT:
        {
            int route;
            int index;
            packet >> route >> index;
            if (index >= 0 && index < max_waypoints_in_route)
                routes[route][index] = empty_waypoint;
        }
        break;
    case CMD_MOVE_ROUTE_WAYPOINT:
        {
            int route;
            int index;
            sf::Vector2f position;
            packet >> route >> index >> position;
            if (index >= 0 && index < max_waypoints_in_route)
                routes[route][index] = position;
        }
        break;
    case CMD_ACTIVATE_SELF_DESTRUCT:
        activate_self_destruct = true;
        addToShipLog("Auto destruction activated",sf::Color::Red,"intern");
        for(int n=0; n<max_self_destruct_codes; n++)
        {
            self_destruct_code[n] = irandom(0, 99999);
            self_destruct_code_confirmed[n] = false;
            self_destruct_code_entry_position[n] = max_crew_positions;
            while(self_destruct_code_entry_position[n] == max_crew_positions)
            {
                self_destruct_code_entry_position[n] = ECrewPosition(irandom(0, relayOfficer));
                for(int i=0; i<n; i++)
                    if (self_destruct_code_entry_position[n] == self_destruct_code_entry_position[i])
                        self_destruct_code_entry_position[n] = max_crew_positions;
            }
            self_destruct_code_show_position[n] = max_crew_positions;
            while(self_destruct_code_show_position[n] == max_crew_positions)
            {
                self_destruct_code_show_position[n] = ECrewPosition(irandom(0, relayOfficer));
                if (self_destruct_code_show_position[n] == self_destruct_code_entry_position[n])
                    self_destruct_code_show_position[n] = max_crew_positions;
                for(int i=0; i<n; i++)
                    if (self_destruct_code_show_position[n] == self_destruct_code_show_position[i])
                        self_destruct_code_show_position[n] = max_crew_positions;
            }
        }
        break;
    case CMD_CANCEL_SELF_DESTRUCT:
        if (self_destruct_countdown <= 0.0f)
        {
            activate_self_destruct = false;
            addToShipLog("Auto destruction canceled",sf::Color::Red,"intern");
        }
        break;
    case CMD_CONFIRM_SELF_DESTRUCT:
        {
            int8_t index;
            uint32_t code;
            packet >> index >> code;
            if (index >= 0 && index < max_self_destruct_codes && self_destruct_code[index] == code)
                self_destruct_code_confirmed[index] = true;
        }
        break;
    case CMD_LAUNCH_PROBE:
        if (scan_probe_stock > 0)
        {
            sf::Vector2f target;
            packet >> target;
            P<ScanProbe> p = new ScanProbe();
            p->setPosition(getPosition());
            p->setTarget(target);
            p->setOwner(this);
            scan_probe_stock--;
        }
        break;
    case CMD_SET_ALERT_LEVEL:
        {
            packet >> alert_level;
            if(alertLevelToString(alert_level) == "RED ALERT")
                addToShipLog("RED ALERT",sf::Color::Red,"intern");
            if(alertLevelToString(alert_level) == "YELLOW ALERT")
                addToShipLog("YELLOW ALERT",sf::Color::Yellow,"intern");
        }
        break;
    case CMD_SET_SCIENCE_LINK:
        {
            packet >> linked_science_probe_id;
        }
        break;
	case CMD_SET_PROBE_3D_LINK:
        {
            packet >> linked_probe_3D_id;
        }
        break;
    case CMD_CUSTOM_FUNCTION:
        {
            string name;
            packet >> name;
            for(CustomShipFunction& csf : custom_functions)
            {
                if (csf.name == name)
                {
                    if (csf.type == CustomShipFunction::Type::Button || csf.type == CustomShipFunction::Type::Message)
                    {
                        csf.callback.call();
                    }
                    if (csf.type == CustomShipFunction::Type::Message)
                    {
                        removeCustom(name);
                    }
                    break;
                }
            }
        }
        break;
    case CMD_SET_AUTO_REPAIR_SYSTEM_TARGET:
        {
            ESystem system;
            packet >> system;
            if (system < SYS_COUNT)
                auto_repairing_system = system;
        }
        break;
    case CMD_SET_ENGINEERING_CONTROL:
        {
            bool value;
            packet >> value;
            engineering_control_from_bridge = value;
        }
        break;
    case CMD_SET_WARP_FREQUENCY:
        if (warp_calibration_delay <= 0.0)
        {
            int32_t new_frequency;
            packet >> new_frequency;
            if (new_frequency != warp_frequency)
            {
                warp_frequency = new_frequency;
                warp_calibration_delay = warp_calibration_time;
                warp_request = 0;
                if (current_warp > 0){
                    // deal heat and/or damage
                    addHeat(SYS_Warp, current_warp * my_spaceship->warp_calibration_penalty_heat_factor);
                    current_warp = 0;
                }
                if (warp_frequency < 0)
                    warp_frequency = 0;
                if (warp_frequency > SpaceShip::max_frequency)
                    warp_frequency = SpaceShip::max_frequency;
                addToShipLog("Warp frequency changed : " + frequencyToString(new_frequency),sf::Color::Green,"intern");
            }
        }
        break;
    case CMD_SCIENCE_QUERY_TO_BRIDGE_DB:
        {
            packet >> science_query_to_bridge_db;
        }
        break;
    case CMD_HACK_TASK:
        {
            ESystem target_system;
            uint32_t target_id;
            packet >> target_id >> target_system;
            P<SpaceShip> target = game_server->getObjectById(target_id);
            if (target && target_system < SYS_COUNT && target->hasSystem(target_system) && target->canBeHackedBy(this))
                ScienceTask::addHackTask(scienceTasks, max_science_tasks, target_id, target_system);
        }
        break;
    case CMD_SCAN_TASK:
        {
            uint32_t target_id;
            packet >> target_id;
            P<SpaceShip> target = game_server->getObjectById(target_id);
            if (target && target->canBeScannedBy(this))
                ScienceTask::addScanTask(scienceTasks, max_science_tasks, target_id);
        }
        break;
    case CMD_TASK_COMPLETED:
        {
            int taskIndex;
            bool success;
            packet >> taskIndex >> success;
            if (taskIndex > 0 && taskIndex < max_science_tasks){
                ScienceTask &task = scienceTasks[taskIndex];
                if (success){
                    P<SpaceShip> target = getObjectById(task.target_id);
                    if (task.type == STT_Hack && target && target->canBeHackedBy(this)){
                        target->hackFinished(this, getSystemName(task.target_system));
                    } else if (task.type == STT_Scan && target && target->canBeScannedBy(this)){
                        target->scannedBy(this);
                    }
                }
                task.clear();
            }
        }
        break;
    case CMD_CLEAR_TASKS:
        {
            for(int n = 0; n < max_science_tasks; n++)
            {
                scienceTasks[n].clear();
            }
        }
        break;
    case CMD_SEND_QUERY:
        {
            string message, station;
            packet >> message >> station;
            if ((station == "intern" && intern_in_query) || (station == "excalibur" && excalibur_in_query)){
                addToShipLog("failed request: " + message, colorConfig.log_receive_enemy, station);
                addToShipLog("previous request in process. try again later.", colorConfig.log_receive_enemy, station);
            } else {
                if (station == "intern"){
                    intern_in_query = true;
                } else if (station == "excalibur"){
                    excalibur_in_query = true;
                } 
                addToShipLog("Request: " + message, colorConfig.log_send, station);
                new ActionItem(string("Request: " + station + "@"+ getCallSign() ), message, 
                    [this, station](string response){
                        if (station == "intern"){
                            intern_in_query = false;
                        } else if (station == "excalibur"){
                            excalibur_in_query = false;
                        }
                        addToShipLog("Request resolved: " + response, colorConfig.log_receive_friendly, station);
                    }, 
                    [this, station](string response){
                        if (station == "intern"){
                            intern_in_query = false;
                        } else if (station == "excalibur"){
                            excalibur_in_query = false;
                        }
                        addToShipLog("Request error: " + response, colorConfig.log_receive_enemy, station);
                    });
            }
            // TODO
        }
        break;
    default:
        SpaceShip::handleClientCommand(client_id, command, packet);
    }
}

// Client-side functions to send a command to the server.

void PlayerSpaceship::commandSetShields(bool enabled)
{
    sf::Packet packet;
    packet << CMD_SET_SHIELDS << enabled;
    sendClientCommand(packet);
}

void PlayerSpaceship::commandMainScreenSetting(EMainScreenSetting mainScreen)
{
    sf::Packet packet;
    packet << CMD_SET_MAIN_SCREEN_SETTING << mainScreen;
    sendClientCommand(packet);
}

void PlayerSpaceship::commandMainScreenOverlay(EMainScreenOverlay mainScreen)
{
    sf::Packet packet;
    packet << CMD_SET_MAIN_SCREEN_OVERLAY << mainScreen;
    sendClientCommand(packet);
}

void PlayerSpaceship::commandScan(P<SpaceObject> object)
{
    sf::Packet packet;
    packet << CMD_SCAN_OBJECT << object->getMultiplayerId();
    sendClientCommand(packet);
}

void PlayerSpaceship::commandSetSystemPowerRequest(ESystem system, float power_request)
{
    sf::Packet packet;
    systems[system].power_request = power_request;
    packet << CMD_SET_SYSTEM_POWER_REQUEST << system << power_request;
    sendClientCommand(packet);
}

void PlayerSpaceship::commandSetSystemCoolantRequest(ESystem system, float coolant_request)
{
    sf::Packet packet;
    systems[system].coolant_request = coolant_request;
    packet << CMD_SET_SYSTEM_COOLANT_REQUEST << system << coolant_request;
    sendClientCommand(packet);
}

void PlayerSpaceship::commandSetAllSystemsCoolantRequests(float coolant_requests[SYS_COUNT])
{
    sf::Packet packet;
    packet << CMD_SET_ALL_SYSTEMS_COOLANT_REQUESTS;
    for(int n=0; n<SYS_COUNT; n++){
        packet << coolant_requests[n];
    }
    sendClientCommand(packet);
}

void PlayerSpaceship::commandOpenVoiceComm(P<SpaceObject> obj)
{
    if (!obj || 
        length(getPosition() - obj->getPosition()) > gameGlobalInfo->long_range_radar_range)
        return;
    sf::Packet packet;
    packet << CMD_OPEN_MID_RANGE_COMM << obj->getMultiplayerId();
    sendClientCommand(packet);
}

void PlayerSpaceship::commandOpenTextComm(P<SpaceObject> obj)
{
    if (!obj) return;
    sf::Packet packet;
    packet << CMD_OPEN_TEXT_COMM << obj->getMultiplayerId();
    sendClientCommand(packet);
}

void PlayerSpaceship::commandCloseTextComm()
{
    sf::Packet packet;
    packet << CMD_CLOSE_TEXT_COMM;
    sendClientCommand(packet);
}

void PlayerSpaceship::commandAnswerCommHail(bool awnser)
{
    sf::Packet packet;
    packet << CMD_ANSWER_COMM_HAIL << awnser;
    sendClientCommand(packet);
}

void PlayerSpaceship::commandSendComm(uint8_t index)
{
    sf::Packet packet;
    packet << CMD_SEND_TEXT_COMM << index;
    sendClientCommand(packet);
}

void PlayerSpaceship::commandSendCommPlayer(string message)
{
    sf::Packet packet;
    packet << CMD_SEND_TEXT_COMM_PLAYER << message;
    sendClientCommand(packet);
}

void PlayerSpaceship::commandSetAutoRepair(bool enabled)
{
    sf::Packet packet;
    packet << CMD_SET_AUTO_REPAIR << enabled;
    sendClientCommand(packet);
}

void PlayerSpaceship::commandSetShieldFrequency(int32_t frequency)
{
    sf::Packet packet;
    packet << CMD_SET_SHIELD_FREQUENCY << frequency;
    sendClientCommand(packet);
}

void PlayerSpaceship::commandAddWaypoint(sf::Vector2f position)
{
    sf::Packet packet;
    packet << CMD_ADD_WAYPOINT << position;
    sendClientCommand(packet);
}

void PlayerSpaceship::commandRemoveWaypoint(int32_t index)
{
    sf::Packet packet;
    packet << CMD_REMOVE_WAYPOINT << index;
    sendClientCommand(packet);
}

void PlayerSpaceship::commandMoveWaypoint(int32_t index, sf::Vector2f position)
{
    sf::Packet packet;
    packet << CMD_MOVE_WAYPOINT << index << position;
    sendClientCommand(packet);
}

void PlayerSpaceship::commandAddRouteWaypoint(int route, sf::Vector2f position)
{
    sf::Packet packet;
    packet << CMD_ADD_ROUTE_WAYPOINT << route << position;
    sendClientCommand(packet);
}

void PlayerSpaceship::commandRemoveRouteWaypoint(int route, int index)
{
    sf::Packet packet;
    packet << CMD_REMOVE_ROUTE_WAYPOINT << route << index;
    sendClientCommand(packet);
}

void PlayerSpaceship::commandMoveRouteWaypoint(int route, int index, sf::Vector2f position)
{
    sf::Packet packet;
    packet << CMD_MOVE_ROUTE_WAYPOINT << route << index << position;
    sendClientCommand(packet);
}

void PlayerSpaceship::commandActivateSelfDestruct()
{
    sf::Packet packet;
    packet << CMD_ACTIVATE_SELF_DESTRUCT;
    sendClientCommand(packet);
}

void PlayerSpaceship::commandCancelSelfDestruct()
{
    sf::Packet packet;
    packet << CMD_CANCEL_SELF_DESTRUCT;
    sendClientCommand(packet);
}

void PlayerSpaceship::commandConfirmDestructCode(int8_t index, uint32_t code)
{
    sf::Packet packet;
    packet << CMD_CONFIRM_SELF_DESTRUCT << index << code;
    sendClientCommand(packet);
}

void PlayerSpaceship::commandLaunchProbe(sf::Vector2f target_position)
{
    sf::Packet packet;
    packet << CMD_LAUNCH_PROBE << target_position;
    sendClientCommand(packet);
}

void PlayerSpaceship::commandScanDone()
{
    sf::Packet packet;
    packet << CMD_SCAN_DONE;
    sendClientCommand(packet);
}

void PlayerSpaceship::commandScanCancel()
{
    sf::Packet packet;
    packet << CMD_SCAN_CANCEL;
    sendClientCommand(packet);
}

void PlayerSpaceship::commandSetAlertLevel(EAlertLevel level)
{
    sf::Packet packet;
    packet << CMD_SET_ALERT_LEVEL;
    packet << level;
    sendClientCommand(packet);
}

void PlayerSpaceship::commandCustomFunction(string name)
{
    sf::Packet packet;
    packet << CMD_CUSTOM_FUNCTION;
    packet << name;
    sendClientCommand(packet);
}

void PlayerSpaceship::commandSetScienceLink(int32_t id){
    sf::Packet packet;
    packet << CMD_SET_SCIENCE_LINK << id;
    sendClientCommand(packet);
}

void PlayerSpaceship::commandSetProbe3DLink(int32_t id){
    sf::Packet packet;
    packet << CMD_SET_PROBE_3D_LINK << id;
    sendClientCommand(packet);
}

void PlayerSpaceship::commandSetAutoRepairSystemTarget(ESystem system)
{
    sf::Packet packet;
    packet << CMD_SET_AUTO_REPAIR_SYSTEM_TARGET << system;
    sendClientCommand(packet);
}

void PlayerSpaceship::commandSetEngineeringControlToBridge()
{
    sf::Packet packet;
    packet << CMD_SET_ENGINEERING_CONTROL << true;
    sendClientCommand(packet);
}

void PlayerSpaceship::commandSetEngineeringControlToECR()
{
    sf::Packet packet;
    packet << CMD_SET_ENGINEERING_CONTROL << false;
    sendClientCommand(packet);
}

void PlayerSpaceship::onReceiveServerCommand(sf::Packet& packet)
{
    int16_t command;
    packet >> command;
    switch(command)
    {
    case CMD_PLAY_CLIENT_SOUND:
        if (my_spaceship == this && my_player_info)
        {
            ECrewPosition position;
            string sound_name;
            packet >> position >> sound_name;
            if ((position == max_crew_positions && my_player_info->isMainScreen()) || my_player_info->crew_position[position])
            {
                soundManager->playSound(sound_name);
            }
        }
        break;
    }
}

void PlayerSpaceship::commandSendScienceQueryToBridgeDB(string entryName){
    sf::Packet packet;
    packet << CMD_SCIENCE_QUERY_TO_BRIDGE_DB << entryName;
    sendClientCommand(packet);
}

void PlayerSpaceship::commandAddHackTask(P<SpaceShip> target, ESystem target_system){
    sf::Packet packet;
    packet << CMD_HACK_TASK << target->getMultiplayerId() << target_system;
    sendClientCommand(packet);
}

void PlayerSpaceship::commandAddScanTask(P<SpaceObject> object){
    sf::Packet packet;
    packet << CMD_SCAN_TASK << object->getMultiplayerId();
    sendClientCommand(packet);
}

void PlayerSpaceship::commandCompleteScienceTask(int taskIndex, bool success){
    sf::Packet packet;
    packet << CMD_TASK_COMPLETED << taskIndex << success;
    sendClientCommand(packet);
}

void PlayerSpaceship::commandClearAllTasks(){
    sf::Packet packet;
    packet << CMD_CLEAR_TASKS;
    sendClientCommand(packet);
}

void PlayerSpaceship::commandSendQuery(string message, string station){
    sf::Packet packet;
    packet << CMD_SEND_QUERY << message << station;
    sendClientCommand(packet);
}

string PlayerSpaceship::getExportLine()
{
    return std::string("PlayerSpaceship()") + 
        ":setFaction(\"" + getFaction() + "\")" +
        ":setTemplate(\"" + template_name + "\")" + 
        ":setCallSign(\"" + getCallSign() + "\")" + 
        ":setPosition(" + string(getPosition().x, 0) + ", " + string(getPosition().y, 0) + ")" + 
        getScriptExportModificationsOnTemplate();
}

#ifndef _MSC_VER
#include "playerSpaceship.hpp"
#endif /* _MSC_VER */
