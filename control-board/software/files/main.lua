
require( "debugger" )
require( "ctrl_board" )
require( "bit" )

I2c = class()

function I2c:__init()
    self.wnd = qt.load_ui( "./i2c/i2c.ui" )
    self.wnd:show()

    self.dev = CtrlBoard()
    --self.dev:open()
    --print( "Device open attempt " .. ( self.dev:isOpen() and "<b>succeeded</b>" or "<b>failed</b>" ) )
    --self.dev:gpioEn( 1, true )
    --self.dev:gpioConfig( 1, 0xffff, self.dev.GPIO_IPD )
    
    qt.connect( self.wnd.open,    'clicked', self, self.open )
    qt.connect( self.wnd.close,   'clicked', self, self.close )
    qt.connect( self.wnd.enable,  'clicked', self, self.enable )
    qt.connect( self.wnd.disable, 'clicked', self, self.disable )
    qt.connect( self.wnd.config,  'clicked', self, self.config )
    qt.connect( self.wnd.status,  'clicked', self, self.status )
    qt.connect( self.wnd.send01,  'clicked', self, self.send01 )
    qt.connect( self.wnd.send02,  'clicked', self, self.send02 )
    qt.connect( self.wnd.send03,  'clicked', self, self.send03 )
    
    self.addr = 0
end

function I2c:open()
    self.dev:open()
    print( "result: "  ..tostring( self.dev:isOpen() ) )
end

function I2c:close()
    self.dev:close()
    print( "closed" )
end

function I2c:enable()
    self.dev:i2cEn( 0, true )
    print( "enabled" )
end

function I2c:disable()
    self.dev:i2cEnable( 0, false )
end

function I2c:config()
    self.dev:i2cConfig( 0, true, 0, 10000 )
end

function I2c:status()
    local st = self.dev:i2cStatus( 0 )
    print( "status: " .. tostring( st ) )
end

function I2c:send01()
    print( "send01()" )
    local addr = self.addr or 0
    local st = self.dev:i2cStatus( 0 )
    print( "status = " .. tostring( st ) )
    self.dev:i2cIo( 0, 0xA0 + addr, {0, 2, 3, 4, 5, 6, 7, 8, 9, 10 }, 0 )
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
    end
end

function I2c:send02()
    print( "send02()" )
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
    end
end

function I2c:send03()
    print( "send03()" )
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
    end
end

if ( i2c ) then
    i2c = nil
    collectgarbage()
end
i2c = I2c()




