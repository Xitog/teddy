-------------------------------------------------------------------------------
-- Imports
-------------------------------------------------------------------------------
local libpnm = require("libpnm")

-------------------------------------------------------------------------------
-- Local definitions
-------------------------------------------------------------------------------

-- run with lua.exe main.lua
local function hex(data) return string.format("0x%04X", data) end

local function p(t)
    if t == nil then return "nil" end
    local s = ""
    for _, v in ipairs(t) do s = s .. " " .. v end
    return s
end

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
    self.classname = 'Level'
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
    self.classname = 'LevelFile'
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
            local diff = string.unpack("c4", string.sub(data, self.counter + 1,
                                                        start))
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
    local decompressed_size = string.unpack("<I2", string.sub(plane0, 1, 2))
    print("Decompressed size = " .. decompressed_size)
    if decompressed_size ~= 8192 then
        print("Wrong decompressed size detected: " .. decompressed_size ..
                  " != " .. 8192)
        os.exit()
    end
    local index = 3
    while index <= #plane0 - 2 do
        local value = string.unpack("<I2", string.sub(plane0, index, index + 2))
        if value == 0xABCD then
            local next1 = string.unpack("<I2", string.sub(plane0, index + 2,
                                                          index + 4))
            local next2 = string.unpack("<I2", string.sub(plane0, index + 4,
                                                          index + 6))
            for _ = 1, next1, 1 do table.insert(data, next2) end
            index = index + 4
        else
            table.insert(data, value)
        end
        -- print("num", index, "offset", hex(level.offset0 + index), "value",
        --      hex(value), "size", #plane0, "decompressed size", #data,
        --      "/", decompressed_size)
        index = index + 2
    end
    print("Size of level = " .. #data)
    --[[
    local file = io.open("level1plane0.txt", "w")
    for line=1, 64, 1 do
        for row=1, 64, 1 do
            file:write(string.format("%3d", data[(line-1)*64 + row]) .. " ")
        end
        file:write("\n")
    end
    file:close()
    --]]
end

local GraphicFile = {}
GraphicFile.__index = GraphicFile
function GraphicFile.new(filename)
    local self = setmetatable({}, GraphicFile)
    self.classname = 'GraphicFile'
    self.filename = filename
    local f = io.open(self.filename, "rb")
    self.size = f:seek("end")
    f:seek("set")
    self.data = f:read("*all")
    self.total_number_of_chunks = string.unpack("<I2",
                                                string.sub(self.data, 1, 2))
    self.first_sprite_offset_index = string.unpack("<I2",
                                                   string.sub(self.data, 3, 4))
    self.first_sound_offset_index = string.unpack("<I2",
                                                  string.sub(self.data, 5, 6))
    self.offsets = {}
    self.sizes = {}
    local start = 7
    for i = 1, self.total_number_of_chunks, 1 do
        local from = start + (i - 1) * 4 -- i=1 then 7
        local to = start + i * 4 - 1 -- i=1 then 10, we read 7, 8, 9, 10 = 4 bytes
        local address = string.unpack("<I4", string.sub(self.data, from, to))
        table.insert(self.offsets, address)
    end
    start = start + self.total_number_of_chunks * 4
    for i = 1, self.total_number_of_chunks, 1 do
        local from = start + (i - 1) * 2
        local to = start + i * 2
        local address = string.unpack("<I2", string.sub(self.data, from, to))
        table.insert(self.sizes, address)
    end
    f:close()
    return self
end

function GraphicFile:extract(num)
    print("GraphicFile:extract")
    local start = self.offsets[num] + 1
    local size = self.sizes[num]
    local to = start + size
    local raw = string.sub(self.data, start, to)
    local data = {}
    print("from=", start, hex(start), "size=", size, "to=", to, hex(to),
          "length=", #raw)
    for i = 1, size, 1 do
        local v = string.unpack("<I1", string.sub(raw, i, i))
        table.insert(data, v)
    end
    return data
end

-- alternate ou juste strictement scénario nominal dans diag de sésq
-- obtenir le diag de classe participante : à partir d'une feuille blanche ? On met les liens de nav ?
-- ou à partir du diag de classe métier et on surligne en jaune ce qu'on ajoute

function GraphicFile:info()
    print("------------------------------------")
    print("[Graphic File Information]")
    print("------------------------------------")
    print("File name = " .. self.filename)
    print("File size = " .. self.size)
    print("Number of chunks = " .. self.total_number_of_chunks)
    print("First sprite offset index = " .. self.first_sprite_offset_index)
    print("First sound offset index = " .. self.first_sound_offset_index)
    print("List of chunk offsets = ")
    print("--- START OF WALLS ---")
    for i, v in ipairs(self.offsets) do
        if i == 10 then break end
        if i == self.first_sprite_offset_index then
            print("--- START OF SPRITES ---")
        elseif i == self.first_sound_offset_index then
            print("--- START OF SOUNDS ---")
        end
        print("    ", i, "offset=", hex(v), string.format("%05d", v), "size=",
              self.sizes[i])
    end
    print("------------------------------------")
end

-------------------------------------------------------------------------------
-- Handling of palette file
-------------------------------------------------------------------------------

local PaletteFile = {}
PaletteFile.__index = PaletteFile
function PaletteFile.new(filename)
    local self = setmetatable({}, PaletteFile)
    self.classname = 'PaletteFile'
    self.filename = filename
    local f = io.open(self.filename, "rb")
    self.size = f:seek("end")
    f:seek("set")
    local data = f:read("*all")
    local start = 0x77 + 1
    self.colors = {}
    for i = 1, 256, 1 do
        local r = string.unpack("<I1", string.sub(data, start + (i - 1) * 3,
                                                  start + (i - 1) * 3))
        local g = string.unpack("<I1", string.sub(data, start + (i - 1) * 3 + 1,
                                                  start + (i - 1) * 3 + 1))
        local b = string.unpack("<I1", string.sub(data, start + (i - 1) * 3 + 2,
                                                  start + (i - 1) * 3 + 2))
        r = r * 4
        g = g * 4
        b = b * 4
        table.insert(self.colors, {r, g, b})
    end
    return self
end

function PaletteFile:get_color(i)
    -- print(self.colors[i][1], self.colors[i][2], self.colors[i][3])
    return self.colors[i]
end

function PaletteFile:save(filename)
    local file = io.open(filename, "w")
    for i = 1, 256, 1 do
        local r = self.colors[i][1]
        local g = self.colors[i][2]
        local b = self.colors[i][3]
        file:write("\t" .. r .. "," .. g .. "," .. b .. ",\n")
        -- file:write("\t{" .. r .. ", " .. g .. ", " .. b .. "}\n");
    end
    file:close()
end

function PaletteFile:ppm(filename)
    local img = libpnm.PBM.new(160, 160)
    local row = 1
    local col = 1
    for i = 1, 256, 1 do
        img:rect((col - 1) * 10 + 1, (row - 1) * 10 + 1, 10, 10, self.colors[i],
                 true)
        col = col + 1
        if col == 17 then
            col = 1
            row = row + 1
        end
    end
    img:save(filename)
end

-------------------------------------------------------------------------------
-- Main
-------------------------------------------------------------------------------

local h =
    HeaderFile.new("../data/Wolfenstein 3D/Shareware maps/1.0/MAPHEAD.WL1")
h:info()
print()
local lvl = LevelFile.new(
                "../data/Wolfenstein 3D/Shareware maps/1.0/MAPTEMP.WL1", h)
lvl:info()
lvl:decode(1)

local graph = GraphicFile.new(
                  "../data/Wolfenstein 3D/Shareware maps/1.0/VSWAP.WL1")
graph:info()

local palette = PaletteFile.new(
                    "../data/Wolfenstein 3D/Shareware maps/1.0/GAMEPAL.OBJ")
palette:save("palette.txt")
-- palette:ppm("palette.ppm")

for i = 17, 20, 1 do palette:get_color(i) end

local img = libpnm.PBM.new(10, 10)
img:info()
img:rect(1, 6, 10, 5, {255, 0, 0}, true)
-- img:save("test.ppm")

-- On obtient les données raw du mur n°1
local raw = graph:extract(1)
print("Size of raw = ", #raw)
-- On le traduit grâce à la palette
local image = {}
for _, v in ipairs(raw) do
    -- print("reading raw at :", v+1)
    table.insert(image, palette:get_color(v + 1))
end
-- Verifie les 10 premiers
for i = 1, 10, 1 do print("img apres palette @" .. i .. " = " .. p(image[i])) end

-- On produit le PPM
local texture = libpnm.PBM.new(64, 64)
local x = 1
local y = 1
for _, v in ipairs(image) do
    texture:set(x, y, v)
    y = y + 1
    if y == 65 then
        x = x + 1
        y = 1
    end
end
texture:info()
-- texture:save("wall1.ppm")

-- 0x8 1er niveau
