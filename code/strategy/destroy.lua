
local IGNORED_WRITES = {}
local IGNORED_READS = {
    qt=true,
    _PROMPT=true,
    _PROMPT2=true,
    writeObjects=true,
    arg=true,
}

local function long_func() 
    local a = 1
    for i = 1, 1000, 1 do
        a = a +1
    end
    print("end of first loop")
    sleep(0.01)
    for i = 1, 10000, 1 do
        a = a +1
        if a % 1000 == 0 then
            print("a :  " .. tostring(a))
        end
    end
    print("end of 2ND loop")
    sleep(0.01)
    for i = 1, 100000, 1 do
        a = a +1
        if a % 10000 == 0 then
            print("b :  " .. tostring(a))
        end
    end
    print("end of 3RD loop")
    sleep(0.01)
    for i = 1, 1000000, 1 do
        a = a +1
        if a % 10000 == 0 then
            print("C :  " .. tostring(a))
        end
    end
    print("end of 4 loop")
    sleep(0.01)
    for i = 1, 10000000, 1 do
        a = a +1
        if a % 100000 == 0 then
            print("D :  " .. tostring(a))
        end
    end
    print("end of 5 loop")
    sleep(0.01)
    for i = 1, 100000000, 1 do
        a = a +1
        if a % 1000000 == 0 then
            print("E :  " .. tostring(a))
        end
    end
    print("end of 6 loop")
end
local a = 0
local b = 1
local c = 2
local a1 = 0
local b1 = 1
local c1 = 2
local a2 = 0
local b2 = 1
local c2 = 2
local a3 = 0
local b3 = 1
local c3 = 2
local a4 = 0
local b4 = 1
local c4 = 2
local a5 = 0
local b5 = 1
local c5 = 2
local a6 = 0
local b6 = 1
local c6 = 2
local a7 = 0
local b7 = 1
local c7 = 2
local a1 = 0
local b1 = 1
local c1 = 2
local a2 = 0
local b2 = 1
local c2 = 2
local a3 = 0
local b3 = 1
local c3 = 2
local a4 = 0
local b4 = 1
local c4 = 2
local a5 = 0
local b5 = 1
local c5 = 2
local a6 = 0
local b6 = 1
local c6 = 2
local a7 = 0
local b7 = 1
local c7 = 2
local a11 = 0
local b11 = 1
local c11 = 2
local a21 = 0
local b21 = 1
local c21 = 2
local a31 = 0
local b31 = 1
local c31 = 2
local a41 = 0
local b41 = 1
local c41 = 2
local a51 = 0
local b51 = 1
local c51 = 2
local a61 = 0
local b61 = 1
local c61 = 2
local a71 = 0
local b71 = 1
local c71 = 2
local a12 = 0
local b12 = 1
local c12 = 2
local a22 = 0
local b22 = 1
local c22 = 2
local a32 = 0
local b32 = 1
local c32 = 2
local a42 = 0
local b42 = 1
local c42 = 2
local a52 = 0
local b52 = 1
local c52 = 2
local a62 = 0
local b62 = 1
local c62 = 2
local a72 = 0
local b72 = 1
local c72 = 2
local a12 = 0
local b12 = 1
local c12 = 2
local a22 = 0
local b22 = 1
local c23 = 2
local a33 = 0
local b33 = 1
local c33 = 2
local a43 = 0
local b43 = 1
local c43 = 2
local a53 = 0
local b53 = 1
local c53 = 2
local a63 = 0
local b63 = 1
local c63 = 2
local a74 = 0
local b74 = 1
local c74 = 2
local a14 = 0
local b14 = 1
local c14 = 2
local a24 = 0
local b24 = 1
local c24 = 2
local a34 = 0
local b34 = 1
local c34 = 2
local a44 = 0
local b44 = 1
local c44 = 2
local a55 = 0
local b55 = 1
local c55 = 2
local a65 = 0
local b65 = 1
local c65 = 2
local a75 = 0
local b75 = 1
local c75 = 2
-- Raises an error when an undeclared variable is read.
local function guardGlobals()
    assert(getmetatable(_G) == nil, "another global metatable exists")

    -- The detecting of undeclared vars is discussed on:
    -- http://www.lua.org/pil/14.2.html
    -- http://lua-users.org/wiki/DetectingUndefinedVariables
    setmetatable(_G, {
        __newindex = function (table, key, value)
            if not IGNORED_WRITES[key] then
                local info = debug.getinfo(2, "Sl")
                io.stderr:write(string.format(
                    "strict: %s:%s: write to undeclared variable: %s\n",
                    tostring(info.short_src), tostring(info.currentline), key))
            end
            rawset(table, key, value)
        end,
        __index = function (table, key)
            if IGNORED_READS[key] then
                return
            end
            error("attempt to read undeclared variable "..key, 2)
        end,
    })

    local origRequire = require
    function require(modname)
        IGNORED_WRITES[modname] = true
        return origRequire(modname)
    end
end




x_initial, y_initial, theta_initial = 0, 0, 0

function on_init()
    print("on init")
    --overwrite_pose(x_initial, y_initial, theta_initial)
end

function on_run()
    print("Ready to rock") 
    sleep(0.001)
    a = 1
    a = 1 + 1
    local nest = { 
        [1] = { 1, 2, 3, 4, 5, { 10,11,12,13, [100] = {1001,1002}},
    1,2,3,4,5,
    1,2,3,4,5
    },
    [2] = {1, 2, 3, 4, 5},
    [3] = {1, 2, 3, 4, 5},
    [4] = {1, 2, 3, 4, 5},
    [5] = {1, 2, 3, 4, 5},
    [6] = {1, 2, 3, 4, 5},
    [7] = {1, 2, 3, 4, 5},
    [8] = {1, 2, 3, 4, 5},
    [9] = {1, 2, 3, 4, 5},
    [10] = {1, 2, 3, 4, 5},
    [11] = {1, 2, 3, 4, 5},
    [12] = {1, 2, 3, 4, 5},
    [22] = 12,
    [23] = 13,


    }

    local nest_string = { a = 5, b = 10, c = 15, e = {}, f = "btrvtr", g = {} }
    nest_string.g = {3,2,1}

    print(tostring(nest_string.g[1]))

    print("nest : ")
    print(tostring(nest[2]))
    print(tostring(nest[2][3]))
    print(tostring(nest[1][6][100][1]))

    long_func()


    print("end of sl eep")
    print("aaa")
    a = 1 + 1
    a = 1 + 1
    a = 1 + 1
    a = 1 + 1
    a = 1 + 1
    a = 1 + 1
    a = 1 + 1
    a = 1 + 1
    a = 1 + 1
    a = 1 + 1
    a = 1 + 1
    a = 1 + 1
    a = 1 + 1
    a = 1 + 1
    a = 1 + 1
    a = 1 + 1
    a = 1 + 1
    a = 1 + 1
    a = 1 + 1
    a = 1 + 1
    a = 1 + 1
    a = 1 + 1
    a = 1 + 1
    a = 1 + 1
    a = 1 + 1
    a = 1 + 1
    a = 1 + 1
    a = 1 + 1
    a = 1 + 1
    a = 1 + 1
    a = 1 + 1
    a = 1 + 1
    a = 1 + 1
    a = 1 + 1
    a = 1 + 1
    a = 1 + 1
    a = 1 + 1
    a = 1 + 1
    a = 1 + 1
    a = 1 + 1
    a = 1 + 1
    a = 1 + 1
    a = 1 + 1
    a = 1 + 1
    a = 1 + 1
    a = 1 + 1
    a = 1 + 1
    a = 1 + 1
    a = 1 + 1
    a = 1 + 1
    a = 1 + 1
    a = 1 + 1
    a = 1 + 1
    a = 1 + 1
    a = 1 + 1
    a = 1 + 1
    a = 1 + 1
    a = 1 + 1
    a = 1 + 1
    a = 1 + 1
    a = 1 + 1
    a = 1 + 1
    a = 1 + 1
    a = 1 + 1
    a = 1 + 1
    a = 1 + 1
    a = 1 + 1
    a = 1 + 1
    a = 1 + 1
    a = 1 + 1
    a = 1 + 1
    a = 1 + 1
    a = 1 + 1
    a = 1 + 1
    a = 1 + 1
    a = 1 + 1
    a = 1 + 1
    a = 1 + 1
    a = 1 + 1
    a = 1 + 1
    a = 1 + 1
    a = 1 + 1
    a = 1 + 1
    a = 1 + 1
    a = 1 + 1
    a = 1 + 1
    a = 1 + 1
    a = 1 + 1
    a = 1 + 1
    a = 1 + 1
    a = 1 + 1
    a = 1 + 1
    a = 1 + 1
    a = 1 + 1
    a = 1 + 1
    a = 1 + 1
    a = 1 + 1
    a = 1 + 1
    a = 1 + 1
    a = 1 + 1
    a = 1 + 1
    a = 1 + 1
    a = 1 + 1

    print("aaa")
    print("aaa")
    sleep(0.1)
    a = 1 + 1
    a = 1 + 1
    a = 1 + 1
    a = 1 + 1
    a = 1 + 1
    a = 1 + 1
    a = 1 + 1
    a = 1 + 1
    a = 1 + 1
    a = 1 + 1
    a = 1 + 1
    a = 1 + 1
    a = 1 + 1
    a = 1 + 1
    a = 1 + 1
    a = 1 + 1
    a = 1 + 1
    a = 1 + 1
    a = 1 + 1
    a = 1 + 1
    a = 1 + 1
    a = 1 + 1
    a = 1 + 1
    a = 1 + 1
    a = 1 + 1
    a = 1 + 1
    a = 1 + 1
    a = 1 + 1
    a = 1 + 1
    a = 1 + 1
    a = 1 + 1
    a = 1 + 1
    a = 1 + 1
    a = 1 + 1
    a = 1 + 1
    a = 1 + 1
    a = 1 + 1
    a = 1 + 1
    a = 1 + 1
    a = 1 + 1
    a = 1 + 1
    a = 1 + 1
    a = 1 + 1
    a = 1 + 1
    a = 1 + 1
    a = 1 + 1
    a = 1 + 1
    a = 1 + 1
    a = 1 + 1
    a = 1 + 1
    a = 1 + 1
    a = 1 + 1
    a = 1 + 1
    a = 1 + 1
    a = 1 + 1
    a = 1 + 1
    a = 1 + 1
    a = 1 + 1
    a = 1 + 1
    a = 1 + 1
    a = 1 + 1
    a = 1 + 1
    a = 1 + 1
    a = 1 + 1
    a = 1 + 1
    a = 1 + 1
    a = 1 + 1
    a = 1 + 1
    a = 1 + 1
    a = 1 + 1
    a = 1 + 1
    a = 1 + 1
    a = 1 + 1
    a = 1 + 1
    a = 1 + 1
    a = 1 + 1
    a = 1 + 1
    a = 1 + 1
    a = 1 + 1
    a = 1 + 1
    a = 1 + 1
    a = 1 + 1
    a = 1 + 1
    a = 1 + 1
    a = 1 + 1
    a = 1 + 1
    a = 1 + 1
    a = 1 + 1
    a = 1 + 1
    a = 1 + 1
    a = 1 + 1
    a = 1 + 1
    a = 1 + 1
    a = 1 + 1
    a = 1 + 1
    a = 1 + 1
    a = 1 + 1
    a = 1 + 1
    a = 1 + 1
    a = 1 + 1
    a = 1 + 1
    a = 1 + 1
    a = 1 + 1
    a = 1 + 1
    a = 1 + 1
    a = 1 + 1
    a = 1 + 1
    a = 1 + 1
    a = 1 + 1
    a = 1 + 1
    a = 1 + 1
    a = 1 + 1
    a = 1 + 1
    a = 1 + 1
    a = 1 + 1
    a = 1 + 1
    a = 1 + 1
    a = 1 + 1
    a = 1 + 1
    a = 1 + 1
    a = 1 + 1
    a = 1 + 1
    a = 1 + 1
    a = 1 + 1
    a = 1 + 1
    a = 1 + 1
    a = 1 + 1
    a = 1 + 1
    a = 1 + 1
    a = 1 + 1
    a = 1 + 1
    a = 1 + 1
    a = 1 + 1
    a = 1 + 1
    a = 1 + 1
    a = 1 + 1
    a = 1 + 1
    a = 1 + 1
    a = 1 + 1
    a = 1 + 1
    a = 1 + 1
    a = 1 + 1
    a = 1 + 1
    a = 1 + 1
    a = 1 + 1
    a = 1 + 1
    a = 1 + 1
    a = 1 + 1
    a = 1 + 1
    a = 1 + 1
    a = 1 + 1
    a = 1 + 1
    a = 1 + 1
    a = 1 + 1
    a = 1 + 1
    a = 1 + 1
    a = 1 + 1
    a = 1 + 1
    a = 1 + 1
    a = 1 + 1
    a = 1 + 1
    a = 1 + 1
    a = 1 + 1
    a = 1 + 1
    a = 1 + 1
    a = 1 + 1
    a = 1 + 1
    a = 1 + 1
    a = 1 + 1
    a = 1 + 1
    a = 1 + 1
    a = 1 + 1
    a = 1 + 1
    a = 1 + 1
    a = 1 + 1
    a = 1 + 1
    a = 1 + 1
    a = 1 + 1
    a = 1 + 1
    a = 1 + 1
    a = 1 + 1
    a = 1 + 1
    a = 1 + 1
    a = 1 + 1
    a = 1 + 1
    a = 1 + 1
    a = 1 + 1
    a = 1 + 1
    a = 1 + 1
    a = 1 + 1
    a = 1 + 1
    a = 1 + 1
    a = 1 + 1
    a = 1 + 1
    a = 1 + 1
    a = 1 + 1
    a = 1 + 1
    a = 1 + 1
    a = 1 + 1
    a = 1 + 1
    a = 1 + 1
    a = 1 + 1
    a = 1 + 1
    a = 1 + 1
    a = 1 + 1
    a = 1 + 1
    a = 1 + 1
    a = 1 + 1
    a = 1 + 1
    a = 1 + 1
    a = 1 + 1
    a = 1 + 1
    a = 1 + 1
    a = 1 + 1
    a = 1 + 1
    a = 1 + 1
    a = 1 + 1
    a = 1 + 1
    a = 1 + 1
    a = 1 + 1
    a = 1 + 1
    a = 1 + 1
    a = 1 + 1
    a = 1 + 1
    a = 1 + 1
    a = 1 + 1
    a = 1 + 1
    a = 1 + 1
    a = 1 + 1
    a = 1 + 1
    a = 1 + 1
    a = 1 + 1
    a = 1 + 1
    a = 1 + 1
    a = 1 + 1
    a = 1 + 1
    a = 1 + 1
    a = 1 + 1
    a = 1 + 1
    a = 1 + 1
    a = 1 + 1
    a = 1 + 1
    a = 1 + 1
    a = 1 + 1
    a = 1 + 1
    a = 1 + 1
    a = 1 + 1
    a = 1 + 1
    a = 1 + 1
    a = 1 + 1
    a = 1 + 1
    a = 1 + 1
    b = b + 1
    b = b + 1
    b = b + 1
    b = b + 1
    b = b + 1
    b = b + 1
    b = b + 1
    b = b + 1
    b = b + 1
    b = b + 1
    b = b + 1
    b = b + 1
    b = b + 1
    b = b + 1
    b = b + 1
    sleep(0.1)
    b = b + 1
    b = b + 1
    b = b + 1
    b = b + 1
    b = b + 1
    b = b + 1
    b = b + 1
    b = b + 1
    b = b + 1
    b = b + 1
    b = b + 1
    b = b + 1
    b = b + 1
    b = b + 1
    b = b + 1
    b = b + 1
    b = b + 1
    b = b + 1
    b = b + 1
    b = b + 1
    b = b + 1
    b = b + 1
    b = b + 1
    b = b + 1
    b = b + 1
    b = b + 1
    b = b + 1
    b = b + 1
    b = b + 1
    b = b + 1
    b = b + 1
    b = b + 1
    b = b + 1
    b = b + 1
    b = b + 1
    b = b + 1
    b = b + 1
    b = b + 1
    b = b + 1
    b = b + 1
    b = b + 1
    b = b + 1
    b = b + 1
    b = b + 1
    b = b + 1
    b = b + 1
    b = b + 1
    b = b + 1
    b = b + 1
    b = b + 1
    b = b + 1
    b = b + 1
    b = b + 1
    b = b + 1
    b = b + 1
    b = b + 1
    b = b + 1
    b = b + 1
    b = b + 1
    b = b + 1
    b = b + 1
    b = b + 1
    b = b + 1
    b = b + 1
    b = b + 1
    b = b + 1
    b = b + 1
    b = b + 1
    b = b + 1
    b = b + 1
    b = b + 1
    b = b + 1
    b = b + 1
    b = b + 1
    b = b + 1
    b = b + 1
    b = b + 1
    b = b + 1
    b = b + 1
    b = b + 1
    b = b + 1
    b = b + 1
    b = b + 1
    b = b + 1
    b = b + 1
    b = b + 1
    b = b + 1
    b = b + 1
    b = b + 1
    b = b + 1
    b = b + 1
    b = b + 1
    b = b + 1
    b = b + 1
    b = b + 1
    b = b + 1
    b = b + 1
    b = b + 1
    b = b + 1
    b = b + 1
    b = b + 1
    b = b + 1
    b = b + 1
    b = b + 1
    b = b + 1
    b = b + 1
    b = b + 1
    b = b + 1
    b = b + 1
    b = b + 1
    b = b + 1
    b = b + 1
    b = b + 1
    b = b + 1
    b = b + 1
    b = b + 1
    b = b + 1
    b = b + 1
    b = b + 1
    b = b + 1
    sleep(0.1)
    b = b + 1
    b = b + 1
    b = b + 1
    b = b + 1
    b = b + 1
    b = b + 1
    b = b + 1
    b = b + 1
    b = b + 1
    b = b + 1
    b = b + 1
    b = b + 1
    b = b + 1
    b = b + 1
    b = b + 1
    b = b + 1
    b = b + 1
    b = b + 1
    b = b + 1
    b = b + 1
    b = b + 1
    b = b + 1
    b = b + 1
    b = b + 1
    b = b + 1
    b = b + 1
    b = b + 1
    b = b + 1
    b = b + 1
    b = b + 1
    b = b + 1
    b = b + 1
    b = b + 1
    b = b + 1
    b = b + 1
    b = b + 1
    b = b + 1
    b = b + 1
    b = b + 1
    b = b + 1
    b = b + 1
    b = b + 1
    b = b + 1
    b = b + 1
    b = b + 1
    b = b + 1
    b = b + 1
    b = b + 1
    b = b + 1
    b = b + 1
    b = b + 1
    b = b + 1
    b = b + 1
    b = b + 1
    b = b + 1
    b = b + 1
    b = b + 1
    b = b + 1
    b = b + 1
    b = b + 1
    b = b + 1
    b = b + 1
    b = b + 1
    b = b + 1
    b = b + 1
    b = b + 1
    b = b + 1
    b = b + 1
    b = b + 1
    b = b + 1
    b = b + 1
    b = b + 1
    b = b + 1
    b = b + 1
    b = b + 1
    b = b + 1
    b = b + 1
    b = b + 1
    b = b + 1
    b = b + 1
    b = b + 1
    b = b + 1
    b = b + 1
    b = b + 1
    b = b + 1
    b = b + 1
    b = b + 1
    b = b + 1
    b = b + 1
    b = b + 1
    b = b + 1
    b = b + 1
    b = b + 1
    b = b + 1
    b = b + 1
    b = b + 1
    b = b + 1
    b = b + 1
    b = b + 1
    b = b + 1
    b = b + 1
    b = b + 1
    b = b + 1
    b = b + 1
    b = b + 1
    b = b + 1
    b = b + 1
    b = b + 1
    b = b + 1
    b = b + 1
    b = b + 1
    b = b + 1
    b = b + 1
    b = b + 1
    b = b + 1
    b = b + 1
    b = b + 1
    b = b + 1
    b = b + 1
    b = b + 1
    b = b + 1
    b = b + 1
    b = b + 1
    b = b + 1
    b = b + 1
    b = b + 1
    b = b + 1
    b = b + 1
    b = b + 1
    b = b + 1
    b = b + 1
    b = b + 1
    b = b + 1
    b = b + 1
    b = b + 1
    b = b + 1
    b = b + 1
    b = b + 1
    b = b + 1
    b = b + 1
    b = b + 1
    b = b + 1
    b = b + 1
    b = b + 1
    sleep(0.1)
    if true and true and true and true and true and true then
        if not false then
        b = 0.1
        end
    end
    if true and true and true and true and true and true then
        if not false then
        b = 0.1
        end
    end
    if true and true and true and true and true and true then
        if not false then
        b = 0.1
        end
    end
    if true and true and true and true and true and true then
        if not false then
        b = 0.1
        end
    end
    if true and true and true and true and true and true then
        if not false then
        b = 0.1
        end
    end
    if true and true and true and true and true and true then
        if not false then
        b = 0.1
        end
    end
    if true and true and true and true and true and true then
        if not false then
        b = 0.1
        end
    end
    if true and true and true and true and true and true then
        if not false then
        b = 0.1
        end
    end
    if true and true and true and true and true and true then
        if not false then
        b = 0.1
        end
    end
    if true and true and true and true and true and true then
        if not false then
        b = 0.1
        end
    end
    if true and true and true and true and true and true then
        if not false then
        b = 0.1
        end
    end
    if true and true and true and true and true and true then
        if not false then
        b = 0.1
        end
    end
    if true and true and true and true and true and true then
        if not false then
        b = 0.1
        end
    end
    if true and true and true and true and true and true then
        if not false then
        b = 0.1
        end
    end
    if true and true and true and true and true and true then
        if not false then
        b = 0.1
        end
    end
    if true and true and true and true and true and true then
        if not false then
        b = 0.1
        end
    end
    if true and true and true and true and true and true then
        if not false then
        b = 0.1
        end
    end
    if true and true and true and true and true and true then
        if not false then
        b = 0.1
        end
    end
    if true and true and true and true and true and true then
        if not false then
        b = 0.1
        end
    end
    if true and true and true and true and true and true then
        if not false then
        b = 0.1
        end
    end
    if true and true and true and true and true and true then
        if not false then
        b = 0.1
        end
    end
    if true and true and true and true and true and true then
        if not false then
        b = 0.1
        end
    end
    print("before tabletop !")
    local tabletop ={ 1, 2, 3, 4, 5, 5, 6, 6,7,7,8,9,
    1, 2, 3, 4, 5, 5, 6, 6,7,7,8,9,
    1, 2, 3, 4, 5, 5, 6, 6,7,7,8,9,
    1, 2, 3, 4, 5, 5, 6, 6,7,7,8,9,
    1, 2, 3, 4, 5, 5, 6, 6,7,7,8,9,
    1, 2, 3, 4, 5, 5, 6, 6,7,7,8,9,
    1, 2, 3, 4, 5, 5, 6, 6,7,7,8,9,
    1, 2, 3, 4, 5, 5, 6, 6,7,7,8,9,
    1, 2, 3, 4, 5, 5, 6, 6,7,7,8,9,
    1, 2, 3, 4, 5, 5, 6, 6,7,7,8,9,
    1, 2, 3, 4, 5, 5, 6, 6,7,7,8,9,
    1, 2, 3, 4, 5, 5, 6, 6,7,7,8,9,
    1, 2, 3, 4, 5, 5, 6, 6,7,7,8,9,
    1, 2, 3, 4, 5, 5, 6, 6,7,7,8,9,
    1, 2, 3, 4, 5, 5, 6, 6,7,7,8,9,
    1, 2, 3, 4, 5, 5, 6, 6,7,7,8,9,
    1, 2, 3, 4, 5, 5, 6, 6,7,7,8,9,
    1, 2, 3, 4, 5, 5, 6, 6,7,7,8,9,
    1, 2, 3, 4, 5, 5, 6, 6,7,7,8,9,
    1, 2, 3, 4, 5, 5, 6, 6,7,7,8,9,
    1, 2, 3, 4, 5, 5, 6, 6,7,7,8,9,
    1, 2, 3, 4, 5, 5, 6, 6,7,7,8,9,
    1, 2, 3, 4, 5, 5, 6, 6,7,7,8,9,
    1, 2, 3, 4, 5, 5, 6, 6,7,7,8,9,
    1, 2, 3, 4, 5, 5, 6, 6,7,7,8,9,
    1, 2, 3, 4, 5, 5, 6, 6,7,7,8,9,
    1, 2, 3, 4, 5, 5, 6, 6,7,7,8,9,
    1, 2, 3, 4, 5, 5, 6, 6,7,7,8,9,
    1, 2, 3, 4, 5, 5, 6, 6,7,7,8,9,

    1, 2, 3, 4, 5, 5, 6, 6,7,7,8,9,
    1, 2, 3, 4, 5, 5, 6, 6,7,7,8,9,
    1, 2, 3, 4, 5, 5, 6, 6,7,7,8,9,
    1, 2, 3, 4, 5, 5, 6, 6,7,7,8,9,
    1, 2, 3, 4, 5, 5, 6, 6,7,7,8,9,
    1, 2, 3, 4, 5, 5, 6, 6,7,7,8,9,
    1, 2, 3, 4, 5, 5, 6, 6,7,7,8,9,
    1, 2, 3, 4, 5, 5, 6, 6,7,7,8,9,
    1, 2, 3, 4, 5, 5, 6, 6,7,7,8,9,
    1, 2, 3, 4, 5, 5, 6, 6,7,7,8,9,
    1, 2, 3, 4, 5, 5, 6, 6,7,7,8,9,
    1, 2, 3, 4, 5, 5, 6, 6,7,7,8,9,
    1, 2, 3, 4, 5, 5, 6, 6,7,7,8,9,
    1, 2, 3, 4, 5, 5, 6, 6,7,7,8,9,
    1, 2, 3, 4, 5, 5, 6, 6,7,7,8,9,
    1, 2, 3, 4, 5, 5, 6, 6,7,7,8,9,
    1, 2, 3, 4, 5, 5, 6, 6,7,7,8,9,
    1, 2, 3, 4, 5, 5, 6, 6,7,7,8,9,
    1, 2, 3, 4, 5, 5, 6, 6,7,7,8,9,
    1, 2, 3, 4, 5, 5, 6, 6,7,7,8,9,
    1, 2, 3, 4, 5, 5, 6, 6,7,7,8,9,
    1, 2, 3, 4, 5, 5, 6, 6,7,7,8,9,
    1, 2, 3, 4, 5, 5, 6, 6,7,7,8,9,
    1, 2, 3, 4, 5, 5, 6, 6,7,7,8,9,
    1, 2, 3, 4, 5, 5, 6, 6,7,7,8,9,
    1, 2, 3, 4, 5, 5, 6, 6,7,7,8,9,
    1, 2, 3, 4, 5, 5, 6, 6,7,7,8,9,
    1, 2, 3, 4, 5, 5, 6, 6,7,7,8,9,
    1, 2, 3, 4, 5, 5, 6, 6,7,7,8,9,
    1, 2, 3, 4, 5, 5, 6, 6,7,7,8,9,
    1, 2, 3, 4, 5, 5, 6, 6,7,7,8,9,
    1, 2, 3, 4, 5, 5, 6, 6,7,7,8,9,
    1, 2, 3, 4, 5, 5, 6, 6,7,7,8,9,
    1, 2, 3, 4, 5, 5, 6, 6,7,7,8,9,
    1, 2, 3, 4, 5, 5, 6, 6,7,7,8,9,
    1, 2, 3, 4, 5, 5, 6, 6,7,7,8,9,
    1, 2, 3, 4, 5, 5, 6, 6,7,7,8,9,
    1, 2, 3, 4, 5, 5, 6, 6,7,7,8,9,
    1, 2, 3, 4, 5, 5, 6, 6,7,7,8,9,
    1, 2, 3, 4, 5, 5, 6, 6,7,7,8,9,
    1, 2, 3, 4, 5, 5, 6, 6,7,7,8,9,
    1, 2, 3, 4, 5, 5, 6, 6,7,7,8,9,
    1, 2, 3, 4, 5, 5, 6, 6,7,7,8,9,
    1, 2, 3, 4, 5, 5, 6, 6,7,7,8,9,
    1, 2, 3, 4, 5, 5, 6, 6,7,7,8,9,
    1, 2, 3, 4, 5, 5, 6, 6,7,7,8,9,
    1, 2, 3, 4, 5, 5, 6, 6,7,7,8,9,
    1, 2, 3, 4, 5, 5, 6, 6,7,7,8,9,
    1, 2, 3, 4, 5, 5, 6, 6,7,7,8,9,
    1, 2, 3, 4, 5, 5, 6, 6,7,7,8,9,
    1, 2, 3, 4, 5, 5, 6, 6,7,7,8,9,
    1, 2, 3, 4, 5, 5, 6, 6,7,7,8,9,
    1, 2, 3, 4, 5, 5, 6, 6,7,7,8,9,
    1, 2, 3, 4, 5, 5, 6, 6,7,7,8,9,
    1, 2, 3, 4, 5, 5, 6, 6,7,7,8,9,
    1, 2, 3, 4, 5, 5, 6, 6,7,7,8,9,
    1, 2, 3, 4, 5, 5, 6, 6,7,7,8,9
}
tabletop[10] = 4299042
tabletop[12] = 452541200
tabletop[59] = 918429
local tabletop2 = {
    1, 2, 3, 4, 5, 5, 6, 6,7,7,8,9,
    1, 2, 3, 4, 5, 5, 6, 6,7,7,8,9,
    1, 2, 3, 4, 5, 5, 6, 6,7,7,8,9,
    1, 2, 3, 4, 5, 5, 6, 6,7,7,8,9,
    1, 2, 3, 4, 5, 5, 6, 6,7,7,8,9,
    1, 2, 3, 4, 5, 5, 6, 6,7,7,8,9,
    1, 2, 3, 4, 5, 5, 6, 6,7,7,8,9,
    1, 2, 3, 4, 5, 5, 6, 6,7,7,8,9,
    1, 2, 3, 4, 5, 5, 6, 6,7,7,8,9,
    1, 2, 3, 4, 5, 5, 6, 6,7,7,8,9,
    1, 2, 3, 4, 5, 5, 6, 6,7,7,8,9,
    1, 2, 3, 4, 5, 5, 6, 6,7,7,8,9,
    1, 2, 3, 4, 5, 5, 6, 6,7,7,8,9,
   
    1, 2, 3, 4, 5, 5, 6, 6,7,7,8,9,
    1, 2, 3, 4, 5, 5, 6, 6,7,7,8,9,
    1, 2, 3, 4, 5, 5, 6, 6,7,7,8,9,
    1, 2, 3, 4, 5, 5, 6, 6,7,7,8,9,
    1, 2, 3, 4, 5, 5, 6, 6,7,7,8,9,
    1, 2, 3, 4, 5, 5, 6, 6,7,7,8,9,
    1, 2, 3, 4, 5, 5, 6, 6,7,7,8,9,
    1, 2, 3, 4, 5, 5, 6, 6,7,7,8,9,
    1, 2, 3, 4, 5, 5, 6, 6,7,7,8,9,
    1, 2, 3, 4, 5, 5, 6, 6,7,7,8,9,
    1, 2, 3, 4, 5, 5, 6, 6,7,7,8,9,



}

local tabletop3 = { 


    1, 2, 3, 4, 5, 5, 6, 6,7,7,8,9,
    1, 2, 3, 4, 5, 5, 6, 6,7,7,8,9,
    1, 2, 3, 4, 5, 5, 6, 6,7,7,8,9,
    1, 2, 3, 4, 5, 5, 6, 6,7,7,8,9,
    1, 2, 3, 4, 5, 5, 6, 6,7,7,8,9,
    1, 2, 3, 4, 5, 5, 6, 6,7,7,8,9,
    1, 2, 3, 4, 5, 5, 6, 6,7,7,8,9,
    1, 2, 3, 4, 5, 5, 6, 6,7,7,8,9,

    1, 2, 3, 4, 5, 5, 6, 6,7,7,8,9,

    1, 2, 3, 4, 5, 5, 6, 6,7,7,8,9,
    1, 2, 3, 4, 5, 5, 6, 6,7,7,8,9,
    1, 2, 3, 4, 5, 5, 6, 6,7,7,8,9,
    1, 2, 3, 4, 5, 5, 6, 6,7,7,8,9,
    1, 2, 3, 4, 5, 5, 6, 6,7,7,8,9,
    1, 2, 3, 4, 5, 5, 6, 6,7,7,8,9,

    1, 2, 3, 4, 5, 5, 6, 6,7,7,8,9,
    1, 2, 3, 4, 5, 5, 6, 6,7,7,8,9,
    1, 2, 3, 4, 5, 5, 6, 6,7,7,8,9,
    1, 2, 3, 4, 5, 5, 6, 6,7,7,8,9,
    1, 2, 3, 4, 5, 5, 6, 6,7,7,8,9,
    1, 2, 3, 4, 5, 5, 6, 6,7,7,8,9,
    1, 2, 3, 4, 5, 5, 6, 6,7,7,8,9,
    1, 2, 3, 4, 5, 5, 6, 6,7,7,8,9,
    1, 2, 3, 4, 5, 5, 6, 6,7,7,8,9,
    1, 2, 3, 4, 5, 5, 6, 6,7,7,8,9,
    1, 2, 3, 4, 5, 5, 6, 6,7,7,8,9,
    1, 2, 3, 4, 5, 5, 6, 6,7,7,8,9,
    1, 2, 3, 4, 5, 5, 6, 6,7,7,8,9,
    1, 2, 3, 4, 5, 5, 6, 6,7,7,8,9,

    1, 2, 3, 4, 5, 5, 6, 6,7,7,8,9,
    1, 2, 3, 4, 5, 5, 6, 6,7,7,8,9,
    1, 2, 3, 4, 5, 5, 6, 6,7,7,8,9,
    1, 2, 3, 4, 5, 5, 6, 6,7,7,8,9,
    1, 2, 3, 4, 5, 5, 6, 6,7,7,8,9,
    1, 2, 3, 4, 5, 5, 6, 6,7,7,8,9,
    1, 2, 3, 4, 5, 5, 6, 6,7,7,8,9,
    1, 2, 3, 4, 5, 5, 6, 6,7,7,8,9,

    1, 2, 3, 4, 5, 5, 6, 6,7,7,8,9,
    1, 2, 3, 4, 5, 5, 6, 6,7,7,8,9,
    1, 2, 3, 4, 5, 5, 6, 6,7,7,8,9,
    1, 2, 3, 4, 5, 5, 6, 6,7,7,8,9,
    1, 2, 3, 4, 5, 5, 6, 6,7,7,8,9,
    1, 2, 3, 4, 5, 5, 6, 6,7,7,8,9,
    1, 2, 3, 4, 5, 5, 6, 6,7,7,8,9,
    1, 2, 3, 4, 5, 5, 6, 6,7,7,8,9,
    1, 2, 3, 4, 5, 5, 6, 6,7,7,8,9,

    1, 2, 3, 4, 5, 5, 6, 6,7,7,8,9,
    1, 2, 3, 4, 5, 5, 6, 6,7,7,8,9,
    1, 2, 3, 4, 5, 5, 6, 6,7,7,8,9,

    1, 2, 3, 4, 5, 5, 6, 6,7,7,8,9,
    1, 2, 3, 4, 5, 5, 6, 6,7,7,8,9,
    1, 2, 3, 4, 5, 5, 6, 6,7,7,8,9,
}

    --set_pose(1000, 0, 0, true)
end

function on_end()
    print("Score: 0")
end

--guardGlobals()
