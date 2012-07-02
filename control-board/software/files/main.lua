
require( "debugger" )
require( "ctrl_board" )
require( "bit" )

Joystick = class()

function Joystick:__init()
    self.wnd = qt.load_ui( "joystick.ui" )
    self.wnd:show()

    self.dev = CtrlBoard()
    self.dev:open()
    print( "Device open attempt " .. ( self.dev:isOpen() and "<b>succeeded</b>" or "<b>failed</b>" ) )
    self.dev:gpioEn( 1, true )
    self.dev:gpioConfig( 1, 0xffff, self.dev.GPIO_IPD )
    --self.dev:gpioSet( 1, 65535, 65535 )
end

function Joystick:refresh()
    print( "Refresh" )
    if ( self.dev:isOpen() ) then
        local val = self.dev:gpio( 1 )
        val = bit.band( val, 1 + 2 + 1024 + 2048 )
        local stri = bit.tohex( val )
        self.wnd.hex:setText( stri )
        stri = ""
        if ( bit.band( val, 1 ) > 0 ) then
            stri = stri .. "Up"
        end
        if ( bit.band( val, 2 ) > 0 ) then
            stri = stri .. "Down"
        end
        if ( bit.band( val, 1024 ) > 0 ) then
            stri = stri .. "Left"
        end
        if ( bit.band( val, 2048 ) > 0 ) then
            stri = stri .. "Right"
        end
        self.wnd.text:setText( stri )
    end
end

function Joystick:closed()
    j = nil
end

if ( j ) then
    j = nil
    collectgarbage()
end
j = Joystick()
while ( true ) do
    j:refresh()
end



