# Root CMakeLists.txt for this repo when used as a Zephyr module.
# Pulls in the vendored SoftDevice Controller integration and the
# nrfxlib modules (softdevice_controller binary lib + MPSL) it needs.

if(CONFIG_BT_LL_SOFTDEVICE)
    # nrfxlib uses cmake-ext: True in its module.yml, so we have to
    # explicitly add its top-level CMakeLists from a dependent module.
    # That brings in softdevice_controller and mpsl based on Kconfig.
    if(DEFINED ZEPHYR_NRFXLIB_MODULE_DIR)
        add_subdirectory(${ZEPHYR_NRFXLIB_MODULE_DIR} ${CMAKE_BINARY_DIR}/nrfxlib)
    else()
        message(FATAL_ERROR "BT_LL_SOFTDEVICE=y but nrfxlib module not found. Make sure config/west.yml has the nrfxlib project.")
    endif()

    add_subdirectory(modules/sdc/controller)
endif()

if(CONFIG_MULTITHREADING_LOCK)
    add_subdirectory(modules/sdc/lib/multithreading_lock)
endif()
