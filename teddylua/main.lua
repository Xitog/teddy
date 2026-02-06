-------------------------------------------------------------------------------
-- Imports
-------------------------------------------------------------------------------
local libpnm = require("libpnm")
local liblua = require("liblua")

-------------------------------------------------------------------------------
-- Globales
-------------------------------------------------------------------------------

_G["WALLS"] = 1
_G["OBJECTS"] = 2

-------------------------------------------------------------------------------
-- Local definitions
-------------------------------------------------------------------------------

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

-- both are 1 based in Lua and 0 based in C
function LevelFile:extract_to_binary(ilevel, iplane)
    local level = self.levels[ilevel]
    local offset = nil
    local size = nil
    if iplane == 1 then
        offset = level.offset0
        size = level.size0
    elseif iplane == 2 then
        offset = level.offset1
        size = level.size1
    else
        error("iplane must be 1 or 2 in Lua")
    end
    local plane = string.sub(self.raw_data, 1 + offset, 1 + offset + size)
    local data = {}
    local decompressed_size = string.unpack("<I2", string.sub(plane, 1, 2))
    print("Decompressed size = " .. decompressed_size)
    if decompressed_size ~= 8192 then
        print("Wrong decompressed size detected: " .. decompressed_size ..
                  " != " .. 8192)
        os.exit()
    end
    local index = 3
    while index <= #plane - 2 do
        local value = string.unpack("<I2", string.sub(plane, index, index + 2))
        if value == 0xABCD then
            local next1 = string.unpack("<I2",
                                        string.sub(plane, index + 2, index + 4))
            local next2 = string.unpack("<I2",
                                        string.sub(plane, index + 4, index + 6))
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
    return data
end

function LevelFile:count_values(ilevel, iplane)
    local data = self:extract_to_binary(ilevel, iplane)
    local values = {}
    local count = 0
    for line = 1, 64, 1 do
        for row = 1, 64, 1 do
            local v = data[(line - 1) * 64 + row]
            if values[v] == nil then
                values[v] = 1
                count = count + 1
            else
                values[v] = values[v] + 1
            end
        end
    end
    return values
end

function LevelFile:extract_to_text(ilevel, iplane)
    local data = self:extract_to_binary(ilevel, iplane)
    local file = io.open("level" .. ilevel .. "plane" .. iplane .. ".txt", "w")
    for line = 1, 64, 1 do
        for row = 1, 64, 1 do
            file:write(string.format("%3d", data[(line - 1) * 64 + row]) .. " ")
        end
        file:write("\n")
    end
    file:close()
end

function LevelFile:extract_to_ppm(i, graph, format)
    local plane0_data = self:extract_to_binary(i, _G["WALLS"])
    local plane1_data = self:extract_to_binary(i, _G["OBJECTS"])
    local size = 64
    local width = size
    local height = size
    local megatexture = libpnm.PBM.new(width * 64, height * 64)
    for line = 1, height, 1 do
        for col = 1, width, 1 do
            -- Drawing wall
            local raw_num0 = plane0_data[(line - 1) * 64 + col]
            local num = raw_num0 * 2 - 1
            if raw_num0 <= 64 or raw_num0 == 90 or raw_num0 == 91 or raw_num0 == 92 or
                raw_num0 == 93 or raw_num0 == 100 then
                if raw_num0 == 90 or raw_num0 == 91 then
                    num = 57
                elseif raw_num0 == 92 or raw_num0 == 93 then
                    num = 63
                elseif raw_num0 == 100 then
                    num = 61
                end
                graph:draw_wall(num, megatexture, (col - 1) * 64, (line - 1) * 64)
                --[[
                local raw = graph:extract_to_binary(num)
                -- On le traduit grâce à la palette
                local image = {}
                for _, v in ipairs(raw) do
                    -- print("reading raw at :", v+1)
                    table.insert(image, palette:get_color(v + 1))
                end
                local x = 1
                local y = 1
                for _, v in ipairs(image) do
                    megatexture:set((col - 1) * 64 + x, (line - 1) * 64 + y, v)
                    y = y + 1
                    if y == 65 then
                        x = x + 1
                        y = 1
                    end
                end
                ]]
            elseif raw_num0 >= 106 and raw_num0 <= 143 then
                megatexture:rect((col - 1) * 64, (line - 1) * 64, 64, 64,
                                 {112, 112, 112}, true)
                megatexture:rect((col - 1) * 64, (line - 1) * 64, 64, 64,
                                 {224, 224, 224}, false)
            else
                print('Missing wall : ' .. raw_num0)
            end
            -- Drawing sprite
            local num1 = plane1_data[(line - 1) * 64 + col]
            if num1 == 19 or num1 == 20 or num1 == 21 or num1 == 22 then
                megatexture:rect((col - 1) * 64 + 24, (line - 1) * 64 + 24, 16, 16,
                                 {0, 255, 0}, true)
            elseif num1 >= 23 and num1 <= 62 then
                graph:draw_sprite(num1 + 43, megatexture, (col - 1) * 64, (line - 1) * 64)
            elseif num1 == 98 then
                megatexture:rect((col - 1) * 64, (line - 1) * 64, 64, 64,
                                 {0, 255, 0}, false)
            elseif num1 ~= 0 then
                print('Missing sprite : ' .. num1)
            end
        end
    end
    megatexture:save("E1M0" .. i .. ".ppm", format)
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
                                                   string.sub(self.data, 3, 4)) +
                                         1
    self.first_sound_offset_index = string.unpack("<I2",
                                                  string.sub(self.data, 5, 6)) +
                                        1
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
    self.palette = nil
    return self
end

function GraphicFile:set_palette(palette) self.palette = palette end

function GraphicFile:nb_walls() return self.first_sprite_offset_index - 1 end

function GraphicFile:exist(num)
    return self.offsets[num] ~= 0 and self.sizes[num] ~= 0
end

function GraphicFile:extract_to_binary(num, raw)
    if not self:exist(num) then return nil end
    local start = self.offsets[num] + 1
    local size = self.sizes[num]
    local to = start + size
    local raw_data = string.sub(self.data, start, to)
    if raw ~= nil and type(raw) == "boolean" and raw then return raw_data end
    local data = {}
    for i = 1, size, 1 do
        local v = string.unpack("<I1", string.sub(raw_data, i, i))
        table.insert(data, v)
    end
    return data
end

function GraphicFile:draw_wall(num, texture, x, y)
    texture = texture or libpnm.PBM.new(64, 64, {0, 0, 0})
    x = x or 0
    y = y or 0
    local raw = self:extract_to_binary(num)
    if raw == nil then return end
    -- On le traduit grâce à la palette
    local image = {}
    for _, v in ipairs(raw) do
        -- print("reading raw at :", v+1)
        table.insert(image, self.palette:get_color(v + 1))
    end
    -- On produit le PPM
    local i = 1
    local j = 1
    for _, v in ipairs(image) do
        texture:set(x + i, y + j, v)
        j = j + 1
        if j == 65 then
            i = i + 1
            j = 1
        end
    end
    return texture
end

function GraphicFile:sprite_info(num)
    local raw_data = self:extract_to_binary(num, true)
    if raw_data == nil then return end
    -- La colonne la plus à gauche = la première
    local left_most = string.unpack("<I2", string.sub(raw_data, 1, 2)) + 1 -- UInt16LE
    -- La colonne la plus à droite = la dernière
    local right_most = string.unpack("<I2", string.sub(raw_data, 3, 4)) + 1 -- UInt16LE
    -- Le nombre de colonne (par calcul)
    local nb_columns = right_most - left_most + 1
    -- L'offset du premier post de chaque colonne
    local column_first_post_offsets = {}
    for i = 1, nb_columns, 1 do
        local v = string.unpack("<I2", string.sub(raw_data, 5 + (i - 1) * 2,
                                                  6 + (i - 1) * 2))
        table.insert(column_first_post_offsets, v)
    end
    -- Le début du pool de pixels (par calcul)
    local pixel_pool_offset = 4 + 2 * nb_columns -- left & right + 2 / colonnes

    print('#', num, ' left=', left_most, ' right=', right_most, ' nb col=', nb_columns,
        ' pixel pool=', pixel_pool_offset, ' post offset=', column_first_post_offsets[1])

     -- sub prend le 2ème index mais on ne fait pas +1 => on est juste avant
    local pixel_pool = string.sub(raw_data, pixel_pool_offset + 1,
                                  column_first_post_offsets[1])
    print('size of pixel pool=', string.len(pixel_pool))

    local computed_size = 0
    pixel_pool_offset = 1
    -- Column by column
    for ic = 1, #column_first_post_offsets, 1 do
        local start_of_posts = column_first_post_offsets[ic]
        print("column",
              string.format("%02d", ic) .. " / " .. #column_first_post_offsets,
              "x = " .. (left_most + (ic - 1)),
              "post start offset = " .. start_of_posts)
        local post_count = 1
        -- Post by post
        while true do
            local post = string.sub(raw_data, start_of_posts + 1,
                                    start_of_posts + 6)
            local post_end = string.unpack("<I2", string.sub(post, 1, 2)) / 2 -
                                 1
            if post_end == -1 then break end
            local post_start = string.unpack("<I2", string.sub(post, 5, 6)) / 2
            print("    post " .. post_count .. " start = " .. post_start ..
                      " end = " .. post_end .. " size = " ..
                      (post_end - post_start + 1))
            for py = 1, post_end - post_start + 1, 1 do
                print("        pixel " .. pixel_pool_offset .. " / " ..
                          string.len(pixel_pool))
                -- get the pixel from the pixel pool
                local v = string.unpack("<I1", string.sub(pixel_pool,
                                                          pixel_pool_offset,
                                                          pixel_pool_offset + 1))
                --texture:set(x + left_most + (ic - 1), y + post_start + py,
                --            self.palette:get_color(v + 1))
                pixel_pool_offset = pixel_pool_offset + 1
            end
            start_of_posts = start_of_posts + 6
            post_count = post_count + 1
            computed_size = computed_size + post_end - post_start + 1
        end
    end
end

function GraphicFile:draw_sprite_debug(num, texture, x, y)
    texture = texture or libpnm.PBM.new(64, 64, {255, 0, 255})
    x = x or 0
    y = y or 0
    local raw_data = self:extract_to_binary(num, true)
    if raw_data == nil then return end
    -- La colonne la plus à gauche = la première
    local left_most = string.unpack("<I2", string.sub(raw_data, 1, 2)) + 1 -- UInt16LE
    -- La colonne la plus à droite = la dernière
    local right_most = string.unpack("<I2", string.sub(raw_data, 3, 4)) + 1 -- UInt16LE
    -- Le nombre de colonne (par calcul)
    local nb_columns = right_most - left_most + 1
    -- L'offset du premier post de chaque colonne
    local column_first_post_offsets = {}
    for i = 1, nb_columns, 1 do
        local v = string.unpack("<I2", string.sub(raw_data, 5 + (i - 1) * 2,
                                                  6 + (i - 1) * 2))
        table.insert(column_first_post_offsets, v)
    end
    -- Le début du pool de pixels (par calcul)
    local pixel_pool_offset = 4 + 2 * nb_columns -- left & right + 2 / colonnes

    -- Affichage début
    print('left=              ', left_most)
    print('right=             ', right_most)
    print('nb columns=        ', nb_columns)
    print('pixel pool offset= ', pixel_pool_offset)
    print('first post offset= ', column_first_post_offsets[1])

    -- sub prend le 2ème index mais on ne fait pas +1 => on est juste avant
    local pixel_pool = string.sub(raw_data, pixel_pool_offset + 1,
                                  column_first_post_offsets[1])
    print('size of pixel pool=', string.len(pixel_pool))

    local computed_size = 0
    pixel_pool_offset = 1
    -- Column by column
    for ic = 1, #column_first_post_offsets, 1 do
        local start_of_posts = column_first_post_offsets[ic]
        print("column",
              string.format("%02d", ic) .. " / " .. #column_first_post_offsets,
              "x = " .. (left_most + (ic - 1)),
              "post start offset = " .. start_of_posts)
        local post_count = 1
        -- Post by post
        while true do
            local post = string.sub(raw_data, start_of_posts + 1,
                                    start_of_posts + 6)
            local post_end = string.unpack("<I2", string.sub(post, 1, 2)) / 2 -
                                 1
            if post_end == -1 then break end
            local post_start = string.unpack("<I2", string.sub(post, 5, 6)) / 2
            print("    post " .. post_count .. " start = " .. post_start ..
                      " end = " .. post_end .. " size = " ..
                      (post_end - post_start + 1))
            for py = 1, post_end - post_start + 1, 1 do
                print("        pixel " .. pixel_pool_offset .. " / " ..
                          string.len(pixel_pool))
                -- get the pixel from the pixel pool
                local v = string.unpack("<I1", string.sub(pixel_pool,
                                                          pixel_pool_offset,
                                                          pixel_pool_offset + 1))
                texture:set(x + left_most + (ic - 1), y + post_start + py,
                            self.palette:get_color(v + 1))
                pixel_pool_offset = pixel_pool_offset + 1
            end
            start_of_posts = start_of_posts + 6
            post_count = post_count + 1
            computed_size = computed_size + post_end - post_start + 1
        end
    end

    -- Affichage fin
    print('left=              ', left_most)
    print('right=             ', right_most)
    print('nb columns=        ', nb_columns)
    print('pixel pool offset= ', pixel_pool_offset)
    print('first post offset= ', column_first_post_offsets[1] + 1)
    print('size of pixel pool=', string.len(pixel_pool))
    print('computed size=     ', computed_size)

    return texture
end

function GraphicFile:draw_sprite(num, texture, x, y)
    texture = texture or libpnm.PBM.new(64, 64, {255, 0, 255})
    x = x or 0
    y = y or 0
    local raw_data = self:extract_to_binary(num, true)
    if raw_data == nil then return end
    -- La colonne la plus à gauche = la première
    local left_most = string.unpack("<I2", string.sub(raw_data, 1, 2)) + 1 -- UInt16LE
    -- La colonne la plus à droite = la dernière
    local right_most = string.unpack("<I2", string.sub(raw_data, 3, 4)) + 1 -- UInt16LE
    -- Le nombre de colonne (par calcul)
    local nb_columns = right_most - left_most + 1
    -- L'offset du premier post de chaque colonne
    local column_first_post_offsets = {}
    for i = 1, nb_columns, 1 do
        local v = string.unpack("<I2", string.sub(raw_data, 5 + (i - 1) * 2,
                                                  6 + (i - 1) * 2))
        table.insert(column_first_post_offsets, v)
    end
    -- Le début du pool de pixels (par calcul)
    local pixel_pool_offset = 4 + 2 * nb_columns -- left & right + 2 / colonnes

    -- sub prend le 2ème index mais on ne fait pas +1 => on est juste avant
    local pixel_pool = string.sub(raw_data, pixel_pool_offset + 1,
                                  column_first_post_offsets[1])

    local computed_size = 0
    pixel_pool_offset = 1
    -- Column by column
    for ic = 1, #column_first_post_offsets, 1 do
        local start_of_posts = column_first_post_offsets[ic]
        local post_count = 1
        -- Post by post
        while true do
            local post = string.sub(raw_data, start_of_posts + 1,
                                    start_of_posts + 6)
            local post_end = string.unpack("<I2", string.sub(post, 1, 2)) / 2 -
                                 1
            if post_end == -1 then break end
            local post_start = string.unpack("<I2", string.sub(post, 5, 6)) / 2
            for py = 1, post_end - post_start + 1, 1 do
                -- get the pixel from the pixel pool
                local v = string.unpack("<I1", string.sub(pixel_pool,
                                                          pixel_pool_offset,
                                                          pixel_pool_offset + 1))
                texture:set(x + left_most + (ic - 1), y + post_start + py,
                            self.palette:get_color(v + 1))
                pixel_pool_offset = pixel_pool_offset + 1
            end
            start_of_posts = start_of_posts + 6
            post_count = post_count + 1
            computed_size = computed_size + post_end - post_start + 1
        end
    end
    return texture
end

-- alternate ou juste strictement scénario nominal dans diag de sésq
-- obtenir le diag de classe participante : à partir d'une feuille blanche ? On met les liens de nav ?
-- ou à partir du diag de classe métier et on surligne en jaune ce qu'on ajoute

function GraphicFile:info(index, to_file)
    local display_everything = false
    if index ~= nil and type(index) == "number" then
        local v = self.offsets[index]
        print("    ", index, "offset=", hex(v), string.format("%05d", v),
              "size=", self.sizes[index])
        return
    elseif index ~= nil and type(index) == "boolean" then
        display_everything = index
    end
    local s = "------------------------------------\n"
    s = s .. "[Graphic File Information]\n"
    s = s .. "------------------------------------\n"
    s = s .. "(offset are 0-based like in C)\n"
    s = s .. "File name = " .. self.filename .. "\n"
    s = s .. "File size = " .. self.size .. "\n"
    s = s .. "Number of chunks = " .. self.total_number_of_chunks .. "\n"
    s = s .. "First sprite offset index = " .. self.first_sprite_offset_index ..
            "\n"
    s = s .. "First sound offset index = " .. self.first_sound_offset_index ..
            "\n"
    s = s .. "List of chunk offsets = " .. "\n"
    if display_everything then
        s = s .. "--- START OF WALLS ---\n"
        for i, v in ipairs(self.offsets) do
            if i == self.first_sprite_offset_index then
                s = s .. "--- START OF SPRITES ---\n"
            elseif i == self.first_sound_offset_index then
                s = s .. "--- START OF SOUNDS ---\n"
            end
            s = s .. "    " .. string.format("%03d", i) .. " offset = " ..
                    " hex: " .. string.format("%5x", v) .. " dec: " .. string.format("%06d", v) ..
                    " size = " .. self.sizes[i] .. "\n"
        end
    end
    s = s .. "------------------------------------\n"
    if to_file == nil then
        print(s)
    else
        local f = io.open(to_file, "w")
        f:write(s)
        f:close()
    end
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
    end
    file:close()
end

function PaletteFile:ppm(filename, format)
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
    if format == nil then format = "ascii" end
    img:save(filename, format)
end

-------------------------------------------------------------------------------
-- Main
-------------------------------------------------------------------------------

local function main()

    local h = HeaderFile.new(
                  "../data/Wolfenstein 3D/Shareware maps/1.0/MAPHEAD.WL1")
    h:info()
    print()
    local lvl = LevelFile.new(
                    "../data/Wolfenstein 3D/Shareware maps/1.0/MAPTEMP.WL1", h)
    lvl:info()

    local graph = GraphicFile.new(
                      "../data/Wolfenstein 3D/Shareware maps/1.0/VSWAP.WL1")
    graph:info()

    local palette = PaletteFile.new(
                        "../data/Wolfenstein 3D/Shareware maps/1.0/GAMEPAL.OBJ")
    -- palette:save("palette.txt")
    -- palette:ppm("palette.ppm")

    graph:set_palette(palette)

    -- 54s
    local start_time = os.clock()
    -- lvl:extract_to_ppm(3, graph, palette, "binary") 54.85
    print(string.format("extract_to_ppm: %.2f\n", os.clock() - start_time))

    local wall = graph:draw_wall(1, libpnm.PBM.new(64, 64, {255, 0, 255}))
    if wall == nil then
        print("no wall")
    else
        print("wall")
        -- wall:save("wall.ppm", "binary")
    end

    local start_export = 666 --115
    local end_export = nil
    local exports = {
        -- [19] = "player_spawn_oriented_north",
        -- [20] = "player_spawn_oriented_east",
        -- [21] = "player_spawn_oriented_south",
        -- [22] = "player_spawn_oriented_west",
        [65] = "demo",
        [66] = "puddle",
        [67] = "green_barrel",
        [68] = "table_and_chairs",
        [69] = "floor_lamp",
        [70] = "chandelier",
        [71] = "hanged_skeleton",
        [72] = "dog_food",
        [73] = "while_pillar",
        [74] = "tree",
        [75] = "skeleton",
        [76] = "sink",
        [77] = "potted_plant",
        [78] = "urn",
        [79] = "bare_table",
        [80] = "ceiling_lamp",
        [81] = "pans",
        [82] = "suit_of_armor",
        [83] = "hanging_cage",
        [84] = "skeleton_in_cage",
        [85] = "bones",
        [86] = "silver_key",
        [87] = "gold_key",
        [88] = "bed",
        [89] = "bucket",
        [90] = "food",
        [91] = "medkit",
        [92] = "ammo_clip",
        [93] = "machine_gun",
        [94] = "chaingun",
        [95] = "cross_treasure",
        [96] = "chalice_treasure",
        [97] = "chest_treasure",
        [98] = "crown_treasure",
        [99] = "life_up",
        [100] = "bones_and_blood",
        [101] = "barrel",
        [102] = "well",
        [103] = "empty_well",
        [104] = "blood",
        [105] = "flag",
        [106] = "x_call_apogee",
        [107] = "x_m1",
        [108] = "x_m2",
        [109] = "x_m3",
        [110] = "x_m4",
        [111] = "x_stove",
        [112] = "x_spears",
        [113] = "x_m5",
        [114] = "x_m6" -- soldat
    }
    for k, v in pairs(exports) do
        print("Exporting at " .. k .. " : " .. v)
        if k >= start_export and (end_export == nil or k <= end_export) then
            local texture = graph:draw_sprite(k, libpnm.PBM
                                                  .new(64, 64, {255, 255, 255}))
            if texture == nil then
                print("Nothing at " .. k)
            else
                print("Saving")
                --texture:save(v .. "_" .. k .. ".ppm", "binary")
            end
        end
    end

    --for i=65, 72 do
    --    graph:sprite_info(i)
    --end
    graph:sprite_info(104) -- 68
     os.exit()

    --graph:info(true, "graphinfo.txt")

    -- 0s
    start_time = os.clock()
    --lvl:extract_to_text(1, 2) -- level 1 plane 2
    print(string.format("extract_to_text: %.2f\n", os.clock() - start_time))

    local values = lvl:count_values(1, 2)
    print("Values of level 1 plane 1:")
    local filtered = {}
    for k, v in pairs(values) do
        if (k < 23 or k > 62) and not liblua.table_includes_value({0}, k) then
            filtered[k] = v
        end
    end
    values = filtered
    liblua.table_print(values, liblua.table_get_keys_sorted_by_values(values))

    lvl:extract_to_ppm(1, graph, "binary")

end

main()

os.exit()
