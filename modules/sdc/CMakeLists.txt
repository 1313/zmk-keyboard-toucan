# Top-level CMake for the vendored SoftDevice Controller integration.
# Pulls in the BLE controller subsystem and the multithreading lock library
# only when CONFIG_BT_LL_SOFTDEVICE is enabled.

if(CONFIG_BT_LL_SOFTDEVICE)
    add_subdirectory(controller)
endif()

if(CONFIG_MULTITHREADING_LOCK)
    add_subdirectory(lib/multithreading_lock)
endif()
