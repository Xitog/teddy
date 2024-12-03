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
    local data = f:read("*all")
    self.total_number_of_chunks = string.unpack("<I2", string.sub(data, 1, 2))
    self.first_sprite_offset_index =
        string.unpack("<I2", string.sub(data, 3, 4))
    self.first_sound_offset_index = string.unpack("<I2", string.sub(data, 5, 6))
    self.offsets = {}
    self.sizes = {}
    local start = 7
    for i = 1, self.total_number_of_chunks, 1 do
        local from = start + (i - 1) * 4 -- i=1 then 7
        local to = start + i * 4 - 1 -- i=1 then 10, we read 7, 8, 9, 10 = 4 bytes
        local address = string.unpack("<I4", string.sub(data, from, to))
        table.insert(self.offsets, address)
    end
    start = start + self.total_number_of_chunks * 4
    for i = 1, self.total_number_of_chunks, 1 do
        local from = start + (i - 1) * 2
        local to = start + i * 2
        local address = string.unpack("<I2", string.sub(data, from, to))
        table.insert(self.sizes, address)
    end
    f:close()
    return self
end

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
-- Handling of portable pixmap (portable bitmap file format : pbm)
-------------------------------------------------------------------------------

local PBM = {}
PBM.__index = PBM
function PBM.new(width, height)
    local self = setmetatable({}, PBM)
    self.classname = 'PBM'
    self.width = width
    self.height = height
    self.size = self.width * self.height
    self.data = {}
    for _ = 1, self.width * self.height, 1 do
        table.insert(self.data, {0, 0, 0})
    end
    return self
end

function PBM:set(x, y, r, g, b)
    -- print("setting " .. x .. "@" .. y .. "=" .. "{" .. r .. ", " .. g .. ", " .. b .. "}")
    self.data[x + y * self.width] = {r, g, b}
end

function PBM:info()
    print("------------------------------------")
    print("[PBM File Information]")
    print("------------------------------------")
    print("Width = " .. self.width)
    print("Height = " .. self.height)
    print("File size = " .. self.size)
end

function PBM:rect(x, y, w, h, color, fill)
    for row = y, y + h - 1, 1 do
        for col = x, x + w - 1, 1 do
            if fill or
                ((row == y or row == y + h) and (col == x or col == x + w)) then
                self.data[(row - 1) * self.width + col] = color
            end
        end
    end
end

function PBM:randomize()
    math.randomseed(os.time())
    self.data = {}
    for _ = 1, self.width * self.height, 1 do
        local r = math.random(255)
        local g = math.random(255)
        local b = math.random(255)
        table.insert(self.data, {r, g, b})
    end
end

function PBM:save(filename)
    local file = io.open(filename, "w")
    file:write("P3\n");
    file:write("# ASCII PPM file\n")
    file:write(self.width .. " " .. self.height .. "\n")
    file:write("255\n\n")
    for row = 1, self.height, 1 do
        for col = 1, self.width, 1 do
            local data = self.data[(row - 1) * self.width + col]
            if data == nil then
                print("Error at col=" .. col .. " row=" .. row)
                os.exit()
            end
            local r = data[1]
            local g = data[2]
            local b = data[3]
            file:write(r .. " " .. g .. " " .. b .. "\n")
        end
        file:write("\n")
    end
    file:close()
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
        -- print(i, r .. "@" .. hex(start + (i-1)*3), g .. "@" .. hex(start + (i-1)*3 + 1), b .. "@" .. hex(start + (i-1)*3 + 2))
    end
    return self
end

function PaletteFile:get_color(i)
    print(self.colors[i][1], self.colors[i][2], self.colors[i][3])
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
    local pbm = PBM.new(160, 160)
    local row = 1
    local col = 1
    for i = 1, 256, 1 do
        pbm:rect((col - 1) * 10 + 1, (row - 1) * 10 + 1, 10, 10, self.colors[i],
                 true)
        col = col + 1
        if col == 17 then
            col = 1
            row = row + 1
        end
    end
    pbm:save(filename)
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
palette:ppm("palette.ppm")

for i=17, 20, 1 do
    palette:get_color(i)
end

local pbm = PBM.new(10, 10)
pbm:info()
pbm:rect(1, 6, 10, 5, {255, 0, 0}, true)
pbm:save("test.ppm")
-- pbm:save("pipo2.ppm")
-- pbm:randomize()
-- pbm:save("pipo.ppm")

-- 0x8 1er niveau

--[[
RLEW tag indicate a repeated value (the first word in MAPHEAD.WL6, which is 0xABCD)

While the decoded data length is less than the total length (or while there are bytes to decode):
    read a word w

--]]
