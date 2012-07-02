
require( "debugger" )
require( "ctrl_board" )
require( "bit" )

ui = qt.load_ui( "form.ui" )
ui:show()

t = { stri = "Hi!", ui = ui, t = 0 }

function t:func()
    print( "button02" )
    local dev = self.dev or CtrlBoard()
    if ( not dev:isOpen() ) then
        local res = dev:open()
        print( "open() invoked, result: " .. tostring( res ) )
    end

    self.dev = dev
end

function t:timeout()
    self.t = self.t + 0.1
    self:func()
end

function t:onButton02()
    print( "button03" )
    local dev = self.dev
    if ( not dev ) then
        print( "error: no device" )
        return
    end
    if ( not dev:isOpen() ) then
        print( "error: device is not open" )
        return
    end
    dev:putUInt8( 1 )
    --local res = dev:version()
    --print( "version is: " .. tostring( res ) )

    dev:gpioEn( 1, true )
    dev:gpioConfig( 1, 0xFFFF, 0x28 )
    local gpio = dev:gpio( 1 )
    print( "gpio = " .. tostring( gpio ) )
    --dev:gpioConfig( 1, 0xFFFF, 0x48 )
end

function t:onButton03()
    local dev = self.dev
    if ( not dev ) or ( not dev:isOpen() ) then
        print( "Access to a device failed" )
        return
    end

    local res = dev:gpio( 1 )
    print( "gpio = " .. tostring( res ) )
end

qt.connect( ui.button01, "clicked", t, t.func )
qt.connect( ui.button02, "clicked", t, t.onButton02 )
qt.connect( ui.button03, "clicked", t, t.onButton03 )
qt.connect( ui.plot,     "timeout", t, t.timeout )
--ui.plot:setInterval( 2 )
--ui.plot:start()
