# Root CMakeLists.txt for this repo when used as a Zephyr module.
# Pulls in the vendored SoftDevice Controller integration.

if(CONFIG_BT_LL_SOFTDEVICE)
    add_subdirectory(modules/sdc/controller)
endif()

if(CONFIG_MULTITHREADING_LOCK)
    add_subdirectory(modules/sdc/lib/multithreading_lock)
endif()
