find_package(KDE4 REQUIRED)
include(KDE4Defaults)
include_directories(${KDE4_INCLUDES} ${QT_INCLUDES})
add_definitions(${QT_DEFINITIONS} ${KDE4_DEFINITIONS})

set(klipper_SRCS       clipboards/klipper/klipper_frontend.cpp
                       clipboards/klipper/klipper_backend.cpp)
set(shared_SRCS        kio_protocol.cpp
                       wrapper/clipboard_wrapper.cpp
                       wrapper/node_wrapper.cpp
                       clients/dbus/dbus_client.cpp)
set(kio_klipper_SRCS   kio_klipper.cpp
                       kio_klipper_protocol.cpp)
set(kio_clipboard_SRCS kio_clipboard.cpp
                       kio_clipboard_protocol.cpp)

set(CMAKE_CXX_FLAGS "-fexceptions")

kde4_add_plugin(kio_clipboard ${kio_clipboard_SRCS} ${shared_SRCS} ${klipper_SRCS})
kde4_add_plugin(kio_klipper   ${kio_klipper_SRCS}   ${shared_SRCS} ${klipper_SRCS})

target_link_libraries(kio_clipboard ${KDE4_KIO_LIBS})
target_link_libraries(kio_klipper   ${KDE4_KIO_LIBS})

install(TARGETS kio_clipboard DESTINATION ${PLUGIN_INSTALL_DIR})
install(TARGETS kio_klipper DESTINATION   ${PLUGIN_INSTALL_DIR})
install(FILES clipboard.protocol DESTINATION ${SERVICES_INSTALL_DIR})
install(FILES klipper.protocol DESTINATION   ${SERVICES_INSTALL_DIR})

add_subdirectory(christian-reiner.info)
add_subdirectory(wrapper)
add_subdirectory(clients)
add_subdirectory(clipboards)