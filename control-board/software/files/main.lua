
require( "debugger" )
require( "ctrl_board" )

ui = qt.load_ui( "form.ui" )
ui:show()

t = { stri = "Hi!", ui = ui, t = 0 }

function t:func()
    print( tostring( self.stri ) )
    local o = ui.plot
    o:clear()
    o:setCurvesCnt( 1 )
    o:setPointsCnt( 100 )
    for i=1, 99 do
        o:addData( 0, math.sin( i / 30 + self.t ) )
    end
    o:replot()
end

function t:timeout()
    self.t = self.t + 0.1
    self:func()
end

function t:onButton02()
--pause()
    print( "button02" )
    local dev = self.dev or CtrlBoard()
    if ( not dev:isOpen() ) then
        local res = dev:open()
        print( "open() invoked, result: " .. tostring( res ) )
    end
    self.dev = dev
end

function t:onButton03()
    print( "button03" )
    [[local dev = self.dev
    if ( not dev ) then
        print( "error: no device" )
        return
    end
    if ( not dev:isOpen() ) then
        print( "error: device is not open" )
        return
    end
    local res = dev:version()
    print( "version is: " .. tostring( res ) )]]
end

qt.connect( ui.button01, "clicked", t, t.func )
qt.connect( ui.button02, "clicked", t, t.onButton02 )
qt.connect( ui.button03, "clicked", t, t.onButton03 )
qt.connect( ui.plot,     "timeout", t, t.timeout )
--ui.plot:setInterval( 2 )
--ui.plot:start()
