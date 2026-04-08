#####################################################################
# The module network is responsible for all raw network stuff and
# checks for TLS security.
#####################################################################

ADD_PLATFORM_LIBRARY(AusweisAppNetwork)

if(NOT TARGET llhttp::llhttp)
    if(TARGET llhttp::llhttp_shared)
        add_library(llhttp::llhttp ALIAS llhttp::llhttp_shared)
    elseif(TARGET llhttp::llhttp_static)
        add_library(llhttp::llhttp ALIAS llhttp::llhttp_static)
    else()
        message(FATAL_ERROR "TARGET of llhttp not found")
    endif()
endif()

target_link_libraries(AusweisAppNetwork ${Qt}::Core ${Qt}::Network OpenSSL::SSL llhttp::llhttp)
target_link_libraries(AusweisAppNetwork AusweisAppGlobal AusweisAppSecureStorage AusweisAppSettings)
