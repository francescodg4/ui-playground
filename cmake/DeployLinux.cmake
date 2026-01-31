# Installs a Qt application on Linux together with what it needs to run without Qt installed:
# the Qt plugins it loads at runtime, and every shared library that the executable and those
# plugins depend on, except the ones that belong to the system (see gallery_system_libraries).
#
# Layout under the install prefix:
#   bin/<target>          launcher: points the dynamic linker at lib/, then starts the executable
#   libexec/<target>      the executable, with a qt.conf that makes Qt use plugins/ only
#   lib/                  the bundled libraries
#   plugins/<type>/       the bundled Qt plugins

# Libraries every Linux desktop provides, and that must come from the system: the C runtime and
# the dynamic linker, the C++ runtime, the graphics drivers' entry points, the X11 client
# libraries and the font stack.
set(GALLERY_SYSTEM_LIBRARIES
    [[.*/ld-linux.*\.so.*]]
    [[.*/lib(c|m|dl|rt|pthread|resolv|util|anl)\.so.*]]
    [[.*/lib(gcc_s|stdc\+\+)\.so.*]]
    [[.*/lib(GL|GLX|GLdispatch|EGL|OpenGL|gbm|drm)\.so.*]]
    [[.*/lib(X11|X11-xcb|xcb)\.so\..*]]
    [[.*/lib(fontconfig|freetype|z|expat)\.so.*]]
)

function(gallery_deploy_linux target)
    cmake_parse_arguments(PARSE_ARGV 1 arg "" "" "PLUGINS")

    install(TARGETS ${target} RUNTIME DESTINATION libexec)
    install(FILES ${CMAKE_CURRENT_FUNCTION_LIST_DIR}/linux/qt.conf DESTINATION libexec)
    install(PROGRAMS ${CMAKE_CURRENT_FUNCTION_LIST_DIR}/linux/launcher.sh DESTINATION bin RENAME ${target})

    # the plugins, each in the folder of its type (platforms, xcbglintegrations, ...)
    set(modules "")
    foreach(plugin IN LISTS arg_PLUGINS)
        if(NOT TARGET ${plugin})
            # some distributions ship the plugins' packages without loading them with Qt6Gui
            string(REPLACE "Qt6::" "Qt6" package "${plugin}")
            find_package(${package} CONFIG QUIET PATHS "${Qt6Gui_DIR}" NO_DEFAULT_PATH)
        endif()
        if(NOT TARGET ${plugin})
            message(STATUS "Packaging: ${plugin} is not available, skipped")
            continue()
        endif()
        get_target_property(location ${plugin} LOCATION)
        get_filename_component(type_dir "${location}" DIRECTORY)
        get_filename_component(type "${type_dir}" NAME)
        install(FILES "${location}" DESTINATION plugins/${type})
        list(APPEND modules "${location}")
    endforeach()

    # the libraries they all need, resolved at install time
    install(CODE "set(gallery_executable \"$<TARGET_FILE:${target}>\")")
    install(CODE "set(gallery_modules \"${modules}\")")
    install(CODE "set(gallery_system_libraries [==[${GALLERY_SYSTEM_LIBRARIES}]==])")
    install(CODE [[
        file(GET_RUNTIME_DEPENDENCIES
            EXECUTABLES "${gallery_executable}"
            MODULES ${gallery_modules}
            RESOLVED_DEPENDENCIES_VAR resolved
            UNRESOLVED_DEPENDENCIES_VAR unresolved
            POST_EXCLUDE_REGEXES ${gallery_system_libraries}
        )
        if(unresolved)
            message(WARNING "Packaging: unresolved libraries: ${unresolved}")
        endif()
        list(LENGTH resolved count)
        message(STATUS "Packaging: bundling ${count} libraries")
        foreach(library IN LISTS resolved)
            file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE SHARED_LIBRARY FOLLOW_SYMLINK_CHAIN FILES "${library}")
        endforeach()
    ]])
endfunction()
