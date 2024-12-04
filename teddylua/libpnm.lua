-------------------------------------------------------------
-- MIT Licence (Expat License Wording)
-------------------------------------------------------------
-- Copyright © 2024, Damien Gouteux
--
-- Permission is hereby granted, free of charge, to any person obtaining a copy
-- of this software and associated documentation files (the "Software"), to deal
-- in the Software without restriction, including without limitation the rights
-- to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
-- copies of the Software, and to permit persons to whom the Software is
-- furnished to do so, subject to the following conditions:
--
-- The above copyright notice and this permission notice shall be included in all
-- copies or substantial portions of the Software.
--
-- THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
-- IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
-- FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
-- AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
-- LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
-- OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
-- SOFTWARE.
--
-- For more information about my projects see:
-- https://xitog.github.io/dgx (in French)

-- This module handles only portable bitmap file format (PBM) in ASCII at the moment
-- See https://fr.wikipedia.org/wiki/Portable_pixmap for more information

local libpnm = {}

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

function PBM:get(col, row) return self.data[(row - 1) * self.width + col] end

function PBM:set(x, y, c)
    -- print("setting " .. x .. "@" .. y .. "=" .. "{" .. r .. ", " .. g .. ", " .. b .. "}")
    self.data[x + (y - 1) * self.width] = c
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

libpnm["PBM"] = PBM

return libpnm
