# ����� � ������������� ���������� Qt.
find_package( Qt4 )
include( ${QT_USE_FILE} )

# ����������� ������������� ������������ ������.
include_directories( "src" )

# ����� ��������� ����.
list( APPEND com_src "./src/qextserialbase.cpp" )
list( APPEND com_src "./src/qextserialport.cpp" )
if( WIN32 )
    list( APPEND com_src "./src/win_qextserialport.cpp" )
    set( defs _TTY_WIN_ )
else( WIN32 )
    list( APPEND com_src "./src/posix_qextserialport.cpp" )
    set( defs _TTY_POSIX_ )
endif( WIN32 )
message( STATUS "SERIAL DEFS: ${defs}" )

# �����������, ��������� �������� �� ������ � ����� ����� ������������.
add_library( serial STATIC ${com_src} )
target_link_libraries( serial ${QT_LIBRARIES} )
# ������� ��� ������� ������������ ���������.
set_property( TARGET serial PROPERTY DEFINE_SYMBOL ${defs} )

set_property( TARGET serial PROPERTY COMPILE_DEFINITIONS ${defs} )

message( STATUS "configuring serial" )
