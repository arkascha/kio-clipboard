find_package(KDE4 "4.5" REQUIRED)
include(KDE4Defaults)
include_directories(${KDE4_INCLUDES} ${QT_INCLUDES})
add_definitions(${QT_DEFINITIONS} ${KDE4_DEFINITIONS})

set(klipper_SRCS       clipboard/klipper/klipper_frontend.cpp
                       clipboard/klipper/klipper_backend.cpp)
set(shared_SRCS        protocol/kio_protocol.cpp
                       clipboard/clipboard_frontend.cpp
                       clipboard/clipboard_backend.cpp
                       node/node_wrapper.cpp
                       node/node_list.cpp
                       client/dbus/dbus_client.cpp)
set(kio_klipper_SRCS   kio_klipper.cpp
                       protocol/kio_klipper_protocol.cpp)
set(kio_clipboard_SRCS kio_clipboard.cpp
                       protocol/kio_clipboard_protocol.cpp)

set(CMAKE_CXX_FLAGS "-fexceptions")

kde4_add_plugin(kio_clipboard ${kio_clipboard_SRCS} ${shared_SRCS} ${klipper_SRCS})
kde4_add_plugin(kio_klipper   ${kio_klipper_SRCS}   ${shared_SRCS} ${klipper_SRCS})

target_link_libraries(kio_clipboard ${KDE4_KIO_LIBS} qjson)
target_link_libraries(kio_klipper   ${KDE4_KIO_LIBS} qjson)

install(TARGETS kio_clipboard DESTINATION ${PLUGIN_INSTALL_DIR})
install(TARGETS kio_klipper DESTINATION   ${PLUGIN_INSTALL_DIR})
install(FILES clipboard.protocol DESTINATION ${SERVICES_INSTALL_DIR})
install(FILES klipper.protocol DESTINATION   ${SERVICES_INSTALL_DIR})

add_subdirectory(utility)
add_subdirectory(node)
add_subdirectory(client)
add_subdirectory(clipboard)
add_subdirectory(protocol)