
require( "debugger" )
require( "ctrl_board" )
require( "bit" )

I2c = class()

function I2c:__init()
    self.wnd = qt.load_ui( "./i2c/i2c.ui" )
    self.wnd:show()

    self.dev = CtrlBoard()
    self.dev:open()
    print( "Device open attempt " .. ( self.dev:isOpen() and "<b>succeeded</b>" or "<b>failed</b>" ) )
    self.dev:gpioEn( 1, true )
    self.dev:gpioConfig( 1, 0xffff, self.dev.GPIO_IPD )
    --self.dev:gpioSet( 1, 65535, 65535 )
    qt.connect( self.wnd.open,    'clicked', self, self.open )
    qt.connect( self.wnd.close,   'clicked', self, self.close )
    qt.connect( self.wnd.enable,  'clicked', self, self.enable )
    qt.connect( self.wnd.disable, 'clicked', self, self.disable )
    qt.connect( self.wnd.config,  'clicked', self, self.config )
    qt.connect( self.wnd.send01,  'clicked', self, self.send01 )
    qt.connect( self.wnd.send02,  'clicked', self, self.send02 )
    qt.connect( self.wnd.send03,  'clicked', self, self.send03 )
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
    self.dev:i2cConfig( 0, true, 0, 100 )
end

function I2c:send01()
    local addr = 0
    local st = self.dev:i2cStatus( 0 )
    print( "status = " .. tostring( st ) )
    self.dev:i2cSend( 0, 0xA0 + addr, {0}, 1 )
    for i=1, 10 do
        st = self.dev:i2cStatus( 0 )
        print( "status = " .. tostring( st ) )
    end
    local res = self.dev:i2cResult( 0, 1 )
    print( "length: " .. tostring( #res ) )
    print( "result[1]: " .. tostring( res[1] ) )
end

function I2c:send02()
    print( "send02()" )
end

function I2c:send03()
    print( "send03()" )
end

if ( i2c ) then
    i2c = nil
    collectgarbage()
end
i2c = I2c()




