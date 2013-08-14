
require( "bit" )


local g_mcu

function init()
    local f = luamcuctrl.create()
    local res = f:open()
    print( "opened: " .. tostring( res ) )
    g_mcu = f;
    if ( res ) then
        reset()
    end
end

function write( t )
    g_mcu:write( t )
end

function read()
    local t = g_mcu:read()
    local stri = ""
    for i=1, #(t) do
        stri = string.format( "%s, %i", stri, t[i] )
    end
    print( stri )
    return t
end

function reopen()
    g_mcu:close()
    local res = g_mcu:open()
    print( "opened: " .. tostring( res ) )
    return res
end

init()
print( "default.lua loaded!!!" )



