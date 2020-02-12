"use strict";

const {
	lua: {
		LUA_OK,
		lua_pcall,
		lua_tojsstring
	},
	lauxlib: {
		luaL_loadstring,
		luaL_newstate,
		luaL_requiref
	},
	lualib: {
		luaL_openlibs
	},
	to_luastring
} = require("fengari");

const {
    luaopen_js,
    push,
    tojs
} = require("fengari-interop");

const {ShipTemplate, getShips} = require("./ship-template");
const fs = require('fs');
const path = require('path');

function new_state() {
    const L = luaL_newstate();
    luaL_openlibs(L);
    luaL_requiref(L, to_luastring("js"), luaopen_js, 0);
    return L;
};
 
const shipTemplatesScript = fs.readFileSync(path.join(__dirname, '..', 'scripts-helios', 'shipTemplates.lua'), 'utf8');

const script = `
local ShipTemplate = ...
${shipTemplatesScript}
`;

const L = new_state();
if (luaL_loadstring(L, to_luastring(script)) !== LUA_OK) {
    throw lua_tojsstring(L, -1);
}
push(L, ShipTemplate);
if (lua_pcall(L, 1, 0, 0) !== LUA_OK) {
    throw tojs(L, -1);
}
const shipsByClass = getShips();
for (let [clazz, ships] of Object.entries(shipsByClass)) {
        const text = `
== ${clazz} ==
${ships.map(ship => `
=== ${ship.name} ===
${ship.description}
{| class="wikitable"
|+ Statistics
|-
${ship.rows.map(
    ([k, v]) => `| '''${k}'''\n| ${v}\n`).join('|-\n')}|}`
).join('\n')}
`;
    const filePath = path.join(__dirname, `shipTemplates-${clazz}.md`);
    try {
        fs.unlinkSync(filePath);
    } catch(err){ }
    fs.writeFileSync(filePath, text);
}