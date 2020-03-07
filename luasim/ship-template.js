const shipsByClass = {};

exports.getShips = function getShips() {
  return shipsByClass;
};
function addShip(clazz, ship) {
  const ships = (shipsByClass[clazz] = shipsByClass[clazz] || []);
  ships.push(ship);
}

const noopMethods = [
  "setType",
  "setModel",
  "setDockClasses",
  "setRadarTrace",
  "setJumpDrive",
  "setTubeDirection",
  "setDocks",
  "addDrones",
  "setTractorBeam",
  "weaponTubeDisallowMissle",
  "setWeaponTubeExclusiveFor",
  "setHasReactor",
  "setEnergyStorage",
  "setDefaultAI",
  "setRepairCrewCount"
];

function getDirectionString(dir){
  if (Math.abs(dir) <=45)
    return "front";
  if (Math.abs(90 - dir) <=45)
    return "right";
  if (Math.abs(dir - 90) <=45)
    return "left";
  if (Math.abs(180 - dir) <=45)
    return "rear";
}

exports.ShipTemplate = function ShipTemplate() {
  const shipdata = {
    name: "",
    description: "",
    rows: []
  };
  const res = {
    setName(name) {
      shipdata.name = name;
      return this;
    },
    setClass(clazz, subclass) {
      shipdata.rows.push(["class", clazz]);
      shipdata.rows.push(["sub-class", subclass]);
      addShip(clazz, shipdata);
      return this;
    },
    setDescription(desc) {
      shipdata.description = desc;
      return this;
    },
    setHull(hull) {
      shipdata.rows.push(["hull", hull]);
      return this;
    },
    setShields(...args) {
      shipdata.rows.push(["shields", args.join(" / ")]);
      return this;
    },
    setSpeed(impulse, turn, acceleration) {
      shipdata.rows.push(["impulse speed", impulse]);
      shipdata.rows.push(["turn speed", turn]);
      shipdata.rows.push(["acceleration speed", acceleration]);
      return this;
    },
    setWarpSpeed(warp) {
      if (warp > 0){
        shipdata.rows.push(["warp speed", warp]);
      }
      return this;
    },
    setCombatManeuver(boost, strafe) {
      shipdata.rows.push(["combat maneuver boost speed", boost]);
      shipdata.rows.push(["combat maneuver strafe speed", strafe]);
      return this;
    },
    setBeam(idx, arc, dir, range, cycleTime, dmg) {
      shipdata.rows.push([getDirectionString(dir) + " beam weapon", (dmg / cycleTime).toFixed(2) + " DPS"]);
      return this;
    },
    setTubes(amount, load_time) {
      shipdata.rows.push(["missile tubes", amount]);
      shipdata.rows.push(["missile load time", load_time]);
      return this;
    },
    setWeaponStorage(weapon, amount) {
      shipdata.rows.push(["missile storage " + weapon, amount]);
      return this;
    }
  };
  res.setBeamWeapon = res.setBeam;
  for (let m of noopMethods) {
    res[m] = function(...args) {
      return this;
    };
  }
  return res;
};