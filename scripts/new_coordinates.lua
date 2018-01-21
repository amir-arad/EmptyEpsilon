setCoordinates([[
function (x, y)
    local ascii_offset = ('A'):byte()
    local float sector_size = 20000
    local sector_x = math.floor(x / sector_size)
    local sector_y = math.floor(y / sector_size)
    local quadrant = 0
    local row = ""
    if sector_y < 0 then
        quadrant = quadrant + 2
        sector_y = -1 - sector_y
    end
    if sector_x < 0 then
        quadrant = quadrant + 1
        sector_x = -1 - sector_x
    end
    while sector_y > -1 do
        row = string.char(ascii_offset + (sector_y % 26)) .. row
        sector_y = math.floor(sector_y / 26) - 1
    end
    return row .. sector_x .. string.char(ascii_offset + quadrant)
end
]], [[
function (sectorName)
    local ascii_offset = ('A'):byte()
    local float sector_size = 20000
    local quadrant = sectorName:sub(sectorName:len()):upper():byte() - ascii_offset;
    local sector_x = tonumber(sectorName:sub(sectorName:find('%d+')))
    local row = sectorName:sub(sectorName:find('%a+')):upper()
    local sector_y = 0;
    for i = #row, 1 do
        sector_y = sector_y * 26 + row:byte(i) - ascii_offset
    end
    if quadrant % 2 == 1 then
        sector_x = -1 - sector_x
    end
    if (math.floor(quadrant /2)) % 2 == 1 then
        sector_y = -1 - sector_y
    end
    return sector_x * sector_size, sector_y * sector_size
end
]], 20000)