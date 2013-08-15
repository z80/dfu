#include <io>

main()
{
    new o1, o2, o3, o4;
    o1 = 0;
    o2 = 0;
    o3 = 0;
    o4 = 0;
    while ( 1 )
    {
        setIo( 0, o1 );
        o1 = o1 + 1;
        if ( o1 > 255 )
        {
            o1 = 0;
            setIo( 1, o2 );
            o2 = o2 + 1;
            if ( o2 > 255 )
            {
                o2 = 0;
                setIo( 2, o3 );
                o3 = o3 + 1;
                if ( o3 > 255 )
                {
                    o3 = 0;
                    setIo( 3, o4 );
                    o4 = o4 + 1;
                    if ( o4 > 255 )
                        o4 = 0;
                }
            }
        }
    }
}
