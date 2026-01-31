# Per-generator CPack settings, read when each package is made.

if(CPACK_GENERATOR STREQUAL "DEB")
    # the bundled Qt must not mix with the system's: the package lives in its own folder
    set(CPACK_PACKAGING_INSTALL_PREFIX "/opt/widget-gallery")
endif()
