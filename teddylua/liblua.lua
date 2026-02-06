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
-- This module offers utility functions for lua
local liblua = {}

local function table_copy(source)
    local dest = {}
    for k, v in pairs(source) do dest[k] = v end
    return dest
end

local function table_search_key(tbl, value)
    for k, v in pairs(tbl) do if v == value then return k end end
end

local function table_get_keys_sorted_by_values(tbl)
    local count = 0
    for _, _ in pairs(tbl) do count = count + 1 end
    local function get_max(tbl2)
        local max = 0
        local max_key = nil
        for k, v in pairs(tbl2) do
            if v > max then
                max = v
                max_key = k
            end
        end
        return max_key
    end
    local sorted_keys = {}
    local base = table_copy(tbl)
    while count > 0 do
        local k = get_max(base)
        table.insert(sorted_keys, k)
        base[k] = nil
        count = count - 1
    end
    return sorted_keys
end

local function list_print(lst)
    for i, v in ipairs(lst) do
        print(i .. ".", v)
    end
end

local function table_print(t, keys)
    if keys == nil then
        local count = 1
        for k, v in pairs(t) do
            print(count .. ".", k, v)
            count = count + 1
        end
    else
        for i, k in ipairs(keys) do
            print(i .. ".", k, t[k])
        end
    end
end

local function table_includes_value(t, needle)
    for _, v in pairs(t) do
        if v == needle then
            return true
        end
    end
    return false
end

local function tests()
    print("Tests:")
    local test_tbl = {[103] = 15, [2] = 22, [59] = 17}
    local keys = liblua.table_get_keys_sorted_by_values(test_tbl)
    table_print(test_tbl, keys)
end

liblua["table_copy"] = table_copy
liblua["table_search_key"] = table_search_key
liblua["table_print"] = table_print
liblua["list_print"] = list_print
liblua["table_get_keys_sorted_by_values"] = table_get_keys_sorted_by_values
liblua["table_includes_value"] = table_includes_value
liblua["tests"] = tests

return liblua
