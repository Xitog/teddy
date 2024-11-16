-- run with lua.exe main.lua

local function hex(data)
    return string.format("0x%X", data)
end

local Header = {}
Header.__index = Header
function Header.new(filename, autoread)
    if autoread == nil then
        autoread = true
    end
    local self = setmetatable({}, Header)
    self.classname = 'Header'
    self.filename = filename
    self.compression_mark = nil
    self.levels = {}
    if autoread then
        self:read()
    end
    return self
end
function Header:addLevel(offset)
    table.insert(self.levels, offset)
end
function Header:read()
    local f = io.open(self.filename, "rb")
    local data = f:read("*all")
    self.compression_mark = string.unpack("<I2", string.sub(data, 1, 2)) -- Unsigned int 16 little endian
    for i=0, 99, 1 do
        -- 1 car on indice depuis 1, 2 car on a lu un UINT16LE, *4 car on va de UINT32LE en UINT32LE
        local from = 1+2+(i*4)
        local to = 1+2+(i+1)*4
        local raw_string = string.sub(data, from, to)
        local n = string.unpack("<I4", raw_string)
        print(i+1, "from", from, "to", to, hex(n), n)
        self:addLevel(n)
    end
    f:close()
end
function Header:info()
    print("Compression mark : " .. hex(self.compression_mark))
end

local Level = {}
Level.__index = Level
function Level.new(filename, autoread)
    if autoread == nil then
        autoread = true
    end
    local self = setmetatable({}, Level)
    self.classname = 'Level'
    self.filename = filename
    if autoread then
        self:read()
    end
end

function Level:read()
    local f = io.open(self.filename, "rb")
    local data = f:read("*all")
end

--[[
UInt32LE 	Offset in file of plane 0 compressed data
UInt32LE 	Offset in file of plane 1 compressed data
UInt32LE 	Offset in file of plane 2 compressed data (unused)
UInt16LE 	Size in bytes of plane 0 compressed data
UInt16LE 	Size in bytes of plane 1 compressed data
UInt16LE 	Size in bytes of plane 2 compressed data (unused)
UInt16LE 	Width of the level grid (always 64)
UInt16LE 	Height of the level grid (always 64)
Char[16] 	0-terminated name of the map (for instance “Wolf3 Map1”)
--]]

local h = Header.new("../data/Wolfenstein 3D/Shareware maps/1.0/MAPHEAD.WL1")
h:info()
local lvl = Level.new("../data/Wolfenstein 3D/Shareware maps/1.0/MAPTEMP.WL1")

-- 0x8 1er niveau
