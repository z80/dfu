#include <io>

main()
{
    new o1, o2, o3;
    o1 = 0;
    while ( 1 )
    {
        setIo( o1 );
        o1 = o1 + 1;
    }
}
