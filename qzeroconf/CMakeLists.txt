set(SRC PWD/qzeroconfservice.h
        PWD/qzeroconfservice.cpp
        PWD/qzeroconfglobal.h
        PWD/qzeroconf.h
        PWD/bonjour_p.h
        PWD/bonjour.cpp
        PWD/bonjour-sdk/dnssd_clientlib.c
        PWD/bonjour-sdk/dnssd_clientstub.c
        PWD/bonjour-sdk/dnssd_ipc.c)

add_library(qzeroconf STATIC ${SRC})
set_target_properties(qzeroconf PROPERTIES CXX_STANDARD 11)
target_link_libraries(qzeroconf PUBLIC wsock32 ws2_32)