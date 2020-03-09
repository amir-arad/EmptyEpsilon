-- Name: Helios
-- Description: Basic Helios map. A few random stations, with random stuff around them, are under attack by enemies. Kill all enemies to win.
-- Type: Basic
-- Variation[Empty]: Places no enemies. Recommended for GM-controlled scenarios and rookie crew orientation. The scenario continues until the GM declares victory or all FSN craft are destroyed.
-- Variation[Easy]: Places fewer enemies. Recommended for inexperienced crews.
-- Variation[Hard]: Places more enemies. Recommended if you have multiple player-controlled ships.
-- Variation[Extreme]: Places many enemies. You're pretty surely overwhelmed.

require("utils.lua")
require("helios.lua")
-- For this scenario, utils.lua provides:
--   vectorFromAngle(angle, length)
--      Returns a relative vector (x, y coordinates)
--   setCirclePos(obj, x, y, angle, distance)
--      Returns the object with its position set to the resulting coordinates.

function init()
	initLayers()
	-- Spawn a player Gravitas.
	PlayerSpaceship():setFaction("FSN"):setTemplate("Gravitas"):setCanBeDestroyed(false)
	addGMFunction("add Mouse", function()
		getObjectByCallSign("PL4"):addDroneCargo("L3 Mouse")
	end)
	addGMFunction("add Cat", function()
		getObjectByCallSign("PL4"):addDroneCargo("L3 Cat")
	end)
	addGMFunction("add Sparrow", function()
		getObjectByCallSign("PL4"):addDroneCargo("M19 Sparrow")
	end)
end

function update(delta)
	updateHelios(delta)
end