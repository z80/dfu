
require( "debugger" )
require( "ctrl_board" )
require( "bit" )

I2c = class()

function I2c:__init()
    self.wnd = qt.load_ui( "./i2c/i2c.ui" )
    self.wnd:show()
    
    self.dev = CtrlBoard()
    --self.slDev = CtrlBoard()
    
    qt.connect( self.wnd.open,    'clicked', self, self.open )
    qt.connect( self.wnd.close,   'clicked', self, self.close )
    qt.connect( self.wnd.enable,  'clicked', self, self.enable )
    qt.connect( self.wnd.disable, 'clicked', self, self.disable )
    qt.connect( self.wnd.config,  'clicked', self, self.config )
    qt.connect( self.wnd.status,  'clicked', self, self.status )
    qt.connect( self.wnd.send01,  'clicked', self, self.send01 )
    qt.connect( self.wnd.send02,  'clicked', self, self.send02 )
    qt.connect( self.wnd.send03,  'clicked', self, self.send03 )
    
    
    qt.connect( self.wnd.low,    'clicked', self, self.low )
    qt.connect( self.wnd.high,   'clicked', self, self.high )
    qt.connect( self.wnd.pulses, 'clicked', self, self.pulses )
    qt.connect( self.wnd.stop,   'clicked', self, self.stop )
    
    --qt.connect( self.wnd.slaveStatus, 'clicked', self, self.slaveStatus )
    
    self.addr = 0
end

function I2c:open()
    self.dev:open()
    --self.slDev:open()
    print( "result: "  ..tostring( self.dev:isOpen() ) )
    -- .. ", " .. tostring( self.slDev:isOpen() ) )
end

function I2c:close()
    self.dev:close()
    self.slDev:close()
    print( "closed" )
end

function I2c:enable()
    self.dev:i2cEn( 0, true )
    --self.slDev:i2cEn( 0, true )
    print( "enabled" )
end

function I2c:disable()
    self.dev:i2cEnable( 0, false )
    --self.slDev:i2cEnable( 0, false )
    print( "disabled" )
end

function I2c:config()
    self.dev:i2cConfig( 0, true, 0, 10000 )
    --self.slDev:i2cConfig( 0, false, 123, 10000 )
    print( "configured" )
end

function I2c:status()
    local st = self.dev:i2cStatus( 0 )
    --local slSt = self.slDev:i2cStatus( 0 )
    print( "status: " .. tostring( st ) .. ", " .. tostring( slSt ) )
end

function I2c:send01()
    print( "send01()" )
    local addr = self.addr or 0
    local st = self.dev:i2cStatus( 0 )
    print( "status = " .. tostring( st ) )
    self.dev:i2cIo( 0, 123, {1, 2}, 1 )
    for i=1, 16 do
        local st, slSt = self.dev:i2cStatus( 0 ), 0 
                          --self.slDev:i2cStatus( 0 )
        print( "status = " .. tostring( st ) .. ", " .. tostring( slSt ) )
        if ( st == 0 ) then
            break
        end
    end
    local t = self.dev:i2cResult( 0, 10 )
    print( "length: " .. tostring( #t ) )
    for i=1, #t do
        print( "t[" .. tostring( i ) .. "] = " .. tostring( t[i] ) )
    end
end

function I2c:send02()
    --[[print( "send02()" )
    local addr = self.addr or 0
    local st = self.dev:i2cStatus( 0 )
    print( "status = " .. tostring( st ) )
    self.dev:i2cIo( 0, 0xA0 + addr, {0}, 10 )
    for i=1, 16 do
        st = self.dev:i2cStatus( 0 )
        print( "status = " .. tostring( st ) )
        if ( st == 0 ) then
            break
        end
    end
    local t = self.dev:i2cResult( 0, 10 )
    print( "length: " .. tostring( #t ) )
    for i=1, #t do
        print( "t[" .. tostring( i ) .. "] = " .. tostring( t[i] ) )
    end]]
end

function I2c:send03()
    --[[print( "send03()" )
    local addr = self.addr or 0
    local st = self.dev:i2cStatus( 0 )
    print( "status = " .. tostring( st ) )
    local wr = self.dev:i2cBytesWritten( 0 )
    print( "written: " .. tostring( wr ) )
    local rd = self.dev:i2cBytesRead( 0 )
    print( "read: " .. tostring( rd ) )
    local t = self.dev:i2cWriteQueue( 0, 5 )
    print( "writeQueue len: " .. tostring( #t ) )
    for i=1, #t do
        print( "t[" .. tostring( i ) .. "] = " .. tostring( t[i] ) )
    end]]
end

function I2c:low()
    self.dev:dbgSetLow()
end

function I2c:high()
    self.dev:dbgSetHigh()
end

function I2c:pulses()
    self.dev:dbgSetPulses( 2, 1, 128 )
end

function I2c:stop()
    self.dev:dbgStop()
end

function I2c:slaveStatus()

end

if ( i2c ) then
    i2c = nil
    collectgarbage()
end
i2c = I2c()




