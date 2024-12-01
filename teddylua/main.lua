-- run with lua.exe main.lua
local function hex(data) return string.format("0x%04X", data) end

local HeaderFile = {}
HeaderFile.__index = HeaderFile
function HeaderFile.new(filename)
    local self = setmetatable({}, HeaderFile)
    self.classname = 'HeaderFile'
    self.filename = filename
    self.compression_mark = nil
    self.level_offsets = {}
    self.end_of_data = nil
    local f = io.open(self.filename, "rb")
    self.size = f:seek("end")
    f:seek("set")
    local data = f:read("*all")
    self.compression_mark = string.unpack("<I2", string.sub(data, 1, 2)) -- Unsigned int 16 little endian
    for i = 0, 99, 1 do
        -- 1 car on indice depuis 1, 2 car on a lu un UINT16LE, *4 car on va de UINT32LE en UINT32LE
        local from = 1 + 2 + (i * 4)
        local to = 1 + 2 + (i + 1) * 4
        local raw_string = string.sub(data, from, to)
        local n = string.unpack("<I4", raw_string)
        if n == 0xFFFFFFFF then
            self.end_of_data = from - 1
            break
        end
        -- print(i + 1, "from", from, "to", to, hex(n), n)
        table.insert(self.level_offsets, n)
    end
    f:close()
    return self
end

function HeaderFile:info()
    print("------------------------------------")
    print("[Header File Information]")
    print("------------------------------------")
    print("File name = " .. self.filename)
    print("File size = " .. self.size)
    print("Compression mark : " .. hex(self.compression_mark) .. " (dec = " ..
              self.compression_mark .. ")")
    print("Number of levels : " .. #self.level_offsets)
    print("Number of bytes read = " .. self.end_of_data .. " / " .. self.size)
    print("List of level offsets = ")
    for i, v in ipairs(self.level_offsets) do
        print("    ", i, hex(v), string.format("%05d", v))
    end
    print("------------------------------------")
end

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
function LevelFile.new(filename, header)
    local self = setmetatable({}, LevelFile)
    self.classname = 'Level'
    self.filename = filename
    self.header = header
    self.levels = {}
    self.raw_data = nil
    local f = io.open(self.filename, "rb")
    self.size = f:seek("end")
    f:seek("set")
    self.raw_data = f:read("*all")
    f:close()
    local data = self.raw_data
    self.tag = string.sub(data, 1, 8)
    self.ending = "!ID!"
    self.counter = 8
    for i = 1, #self.header.level_offsets, 1 do
        local start = self.header.level_offsets[i]; --           8
        if self.counter ~= start then
            local diff = string.unpack("c4", string.sub(data, self.counter + 1, start))
            if diff ~= self.ending then
                print("Error while decoding counter = " .. self.counter ..
                          " and start = " .. start .. " and diff = " .. diff)
                error("Error")
            else
                self.counter = self.counter + 4
            end
        end
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
        self.counter = self.counter + 38 + plane0_size + plane1_size +
                           plane2_size
    end
    return self
end

function LevelFile:info()
    print("------------------------------------")
    print("[Level File Information]")
    print("------------------------------------")
    print("Tag = " .. self.tag)
    -- 64x64 = 4096 donc aucun niveau ne devrait avoir une taille supérieur
    print("n#|Name            (w x h)|start 0      |size|start 1     |size|")
    for i, v in ipairs(self.levels) do
        print(string.format("%2d", i) .. "|" .. string.format("%15s", v.name) ..
                  " (" .. v.width .. "x" .. v.height .. ")| " ..
                  string.format("%04x", v.offset0) .. " (" ..
                  string.format("%05d", v.offset0) .. ")|" ..
                  string.format("%04d", v.size0) .. "|" ..
                  string.format("%04x", v.offset1) .. " (" ..
                  string.format("%05d", v.offset1) .. ")|" ..
                  string.format("%04d", v.size1) .. "|")
    end
    print("Ending = " .. self.ending)
end
function LevelFile:decode(i)
    local level = self.levels[i]
    local plane0 = string.sub(self.raw_data, 1 + level.offset0,
                              1 + level.offset0 + level.size0)
    local data = {}
    for index = 1, 64, 2 do
        local first = string.unpack("<I2", string.sub(plane0, index, index + 2))
        print("num", index, "offset", hex(level.offset0 + index), "value", hex(first)) -- 8192
    end
end

local h =
    HeaderFile.new("../data/Wolfenstein 3D/Shareware maps/1.0/MAPHEAD.WL1")
h:info()
print()
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
