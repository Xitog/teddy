-- run with lua.exe main.lua
local function hex(data) return string.format("0x%X", data) end

local HeaderFile = {}
HeaderFile.__index = HeaderFile
function HeaderFile.new(filename, autoread)
    if autoread == nil then autoread = true end
    local self = setmetatable({}, HeaderFile)
    self.classname = 'HeaderFile'
    self.filename = filename
    self.compression_mark = nil
    self.level_offsets = {}
    if autoread then self:read() end
    return self
end
function HeaderFile:addLevel(offset) table.insert(self.level_offsets, offset) end
function HeaderFile:read()
    local f = io.open(self.filename, "rb")
    local data = f:read("*all")
    self.compression_mark = string.unpack("<I2", string.sub(data, 1, 2)) -- Unsigned int 16 little endian
    for i = 0, 99, 1 do
        -- 1 car on indice depuis 1, 2 car on a lu un UINT16LE, *4 car on va de UINT32LE en UINT32LE
        local from = 1 + 2 + (i * 4)
        local to = 1 + 2 + (i + 1) * 4
        local raw_string = string.sub(data, from, to)
        local n = string.unpack("<I4", raw_string)
        print(i + 1, "from", from, "to", to, hex(n), n)
        self:addLevel(n)
    end
    f:close()
end
function HeaderFile:info() print("Compression mark : " .. hex(self.compression_mark)) end

local Level = {}
Level.__index = Level
function Level.new(offset0, offset1, offset2, size0, size1, size2, name, width,
                   height)
    local self = setmetatable({}, Level)
    self.offset0 = offset0
    self.offset1 = offset1
    self.offset2 = offset2
    self.size0 = size0
    self.size1 = size1
    self.size2 = size2
    self.name = name
    self.width = width
    self.height = height
    return self
end

local LevelFile = {}
LevelFile.__index = LevelFile
function LevelFile.new(filename, header, autoread)
    if autoread == nil then autoread = true end
    local self = setmetatable({}, LevelFile)
    self.classname = 'Level'
    self.filename = filename
    self.header = header
    self.levels = {}
    self.raw_data = data
    if autoread then self:read() end
    return self
end

function LevelFile:read()
    local f = io.open(self.filename, "rb")
    self.raw_data = f:read("*all")
    local data = self.raw_data
    for i = 1, 100, 1 do
        local start = self.header.level_offsets[i]; --           8
        if start == 0xFFFFFFFF then break end
        local plane0_offset = string.unpack("<I4", string.sub(data, 1 + start,
                                                              1 + start + 4)) -- UInt32LE  8-12
        local plane1_offset = string.unpack("<I4", string.sub(data,
                                                              1 + start + 4,
                                                              1 + start + 8)) -- UInt32LE 12-16
        local plane2_offset = string.unpack("<I4", string.sub(data,
                                                              1 + start + 8,
                                                              1 + start + 12)) -- UInt32LE 16-20
        local plane0_size = string.unpack("<I2", string.sub(data,
                                                            1 + start + 12,
                                                            1 + start + 14)) -- UInt16LE 20-22
        local plane1_size = string.unpack("<I2", string.sub(data,
                                                            1 + start + 14,
                                                            1 + start + 16)) -- UInt16LE 22-24
        local plane2_size = string.unpack("<I2", string.sub(data,
                                                            1 + start + 16,
                                                            1 + start + 18)) -- UInt16LE 24-26
        local width = string.unpack("<I2", string.sub(data, 1 + start + 18,
                                                      1 + start + 20)) -- UInt16LE 26-28
        local height = string.unpack("<I2", string.sub(data, 1 + start + 20,
                                                       1 + start + 22)) -- UInt16LE 28-30
        local name = string.unpack("z", string.sub(data, 1 + start + 22,
                                                   1 + start + 38)) -- Char[16] 30-46
        table.insert(self.levels,
                     Level.new(plane0_offset, plane1_offset, plane2_offset,
                               plane0_size, plane1_size, plane2_size, name,
                               width, height))
    end
    f:close()
end
function LevelFile:info()
    for i, v in ipairs(self.levels) do
        print(
            string.format("%2d", i) .. "|" .. v.name .. " (" .. v.width .. "x" ..
                v.height .. ")|")
    end
end
function LevelFile:decode(i)
    local level = self.levels[i]
    local plane0 = string.sub(self.raw_data, 1 + level.offset0, 1 + level.offset0 + level.size0)
    local first = string.unpack("<I2", string.sub(plane0, 1, 1 + 2))
    print(first)
end

local h = HeaderFile.new("../data/Wolfenstein 3D/Shareware maps/1.0/MAPHEAD.WL1")
h:info()
local lvl = LevelFile.new(
                "../data/Wolfenstein 3D/Shareware maps/1.0/MAPTEMP.WL1", h)
lvl:info()
lvl:decode(1)

-- 0x8 1er niveau

--[[
RLEW tag indicate a repeated value (the first word in MAPHEAD.WL6, which is 0xABCD)
Read first 2 bytes of input file. This is the total length in bytes of the decoded data
While the decoded data length is less than the total length (or while there are bytes to decode):
    read a word w
    if the word is the RLEW tag, read two more words w1 and w2, repeat w1 times the word w2 in the output
    otherwise, write w in the output
--]]
