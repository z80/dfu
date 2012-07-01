
require( "unclasslib" )
require( "luactrboardio" )

local CMD_DATA = 0
local CMD_FUNC = 1

local FUNC_VERSION     = 1
local FUNC_GPIO_EN     = 2
local FUNC_GPIO_CONFIG = 3
local FUNC_GPIO_SET    = 4
local FUNC_GPIO        = 5

CtrlBoard = class()

function CtrlBoard:__init()
    self.dev = luactrlboardio.create()
end

function CtrlBoard:open( stri )
    local res = self.dev:open( stri )
    return res
end

function CtrlBoard:close()
    self.dev:close()
end

function CtrlBoard:isOpen()
    local res = self.dev:isOpen()
    return res
end

function CtrlBoard:putUInt8( val )
    local res = self.dev:putUInt8( val )
    return res
end

function CtrlBoard:putUInt16( val )
    local res = self.dev:putUInt16( val )
    return res
end

function CtrlBoard:execFunc( index )
    local res = self.dev:execFunc( index )
    return res
end

function CtrlBoard:readQueue( maxSize )
    local t = self.dev:readQueue( maxSize )
    return t
end

function CtrlBoard:version()
    self:execFunc( FUNC_VERSION )
    local t = self:readQueue( 2 )
    if ( #t < 2 ) then
        return nil, "Failed to read version"
    end
    local res = t[1] + t[2] * 256
end

function CtrlBoard:gpioEn( index, en )
    self:putUInt8( index )
    self:putUInt8( en and 1 or 0 )
    self:execFunc( FUNC_GPIO_EN )
    return true
end

function CtrlBoard:gpioConfig( index, pins, mode )
    self:putUInt8( index )
    self:putUInt16( pins )
    self:putUInt8( mode )
    self:execFunc( FUNC_GPIO_CONFIG )
    return true
end

function CtrlBoard:gpioSet( index, pins, vals )
    self:putUInt8( index )
    self:putUInt16( pins )
    self:putUInt16( vals )
    self:execFunc( FUNC_GPIO_SET )
    return true
end

function CtrlBoard:gpio( index )
    self:putUInt8( index )
    local t = self:readQueue( 2 )
    if ( #t < 2 ) then
        return nil, "ERROR: Failed to get GPIO state"
    end
    local res = t[1] + t[2] * 256
end




