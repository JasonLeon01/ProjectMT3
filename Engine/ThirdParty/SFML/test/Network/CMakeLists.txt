set(NETWORK_SRC
    Dns.test.cpp
    Ftp.test.cpp
    Http.test.cpp
    IpAddress.test.cpp
    Packet.test.cpp
    Sftp.test.cpp
    Socket.test.cpp
    SocketSelector.test.cpp
    TcpListener.test.cpp
    TcpLoopback.test.cpp
    TcpSocket.test.cpp
    UdpSocket.test.cpp
)
if(SFML_OS_EMSCRIPTEN)
    # Listening for connections cannot exist in a browser, so those tests are native-only.
    list(REMOVE_ITEM NETWORK_SRC TcpListener.test.cpp TcpLoopback.test.cpp)
endif()
sfml_add_test(test-sfml-network "${NETWORK_SRC}" SFML::Network)

if(SFML_OS_EMSCRIPTEN)
    set_target_properties(test-sfml-network PROPERTIES SUFFIX ".js")
    target_link_options(test-sfml-network PRIVATE -sEXIT_RUNTIME=1 -sENVIRONMENT=node -sNODE_HOST_ENV=1)
endif()

sfml_set_option(SFML_NETWORK_TESTS_MAX_FDS 256 STRING "The maximum number of open file descriptors a test should use")
target_compile_definitions(test-sfml-network PRIVATE SFML_NETWORK_TESTS_MAX_FDS=${SFML_NETWORK_TESTS_MAX_FDS})

if(SFML_RUN_IP_DISCOVERY_TESTS)
    target_compile_definitions(test-sfml-network PRIVATE SFML_RUN_IP_DISCOVERY_TESTS)
endif()
