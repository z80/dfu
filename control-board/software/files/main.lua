
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

qt.connect( ui.button01, "clicked", t, t.func )
qt.connect( ui.plot,         "timeout", t, t.timeout )
ui.plot:setInterval( 0.5 )
ui.plot:start()
