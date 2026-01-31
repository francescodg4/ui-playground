cmake_minimum_required(VERSION 3.21)

project(WidgetGallery VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()

find_package(Qt6 6.4 REQUIRED COMPONENTS Widgets)
qt_standard_project_setup()

qt_add_executable(gallery
  src/main.cpp
  src/Icons.cpp src/Icons.hpp
  src/ThemeRegistry.cpp src/ThemeRegistry.hpp
  src/WidgetGallery.cpp src/WidgetGallery.hpp
  src/WidgetStyle.cpp src/WidgetStyle.hpp
  src/themes/claudecode/Entry.cpp
  src/themes/claudecode/Style.cpp src/themes/claudecode/Style.hpp
  src/themes/claudecode/Theme.cpp src/themes/claudecode/Theme.hpp
  src/themes/emerald/Entry.cpp
  src/themes/emerald/Style.cpp src/themes/emerald/Style.hpp
  src/themes/emerald/Theme.hpp
  src/themes/emerald/widgets/Pixel.cpp src/themes/emerald/widgets/Pixel.hpp
  src/themes/flat/Entry.cpp
  src/themes/flat/Style.cpp src/themes/flat/Style.hpp
  src/themes/flat/Theme.cpp src/themes/flat/Theme.hpp
  src/themes/glass/Entry.cpp
  src/themes/glass/Style.cpp src/themes/glass/Style.hpp
  src/themes/glass/Theme.cpp src/themes/glass/Theme.hpp
  src/themes/glass/widgets/Glass.cpp src/themes/glass/widgets/Glass.hpp
  src/themes/metro/Entry.cpp
  src/themes/metro/Style.cpp src/themes/metro/Style.hpp
  src/themes/metro/Theme.cpp src/themes/metro/Theme.hpp
  src/themes/proxmox/Entry.cpp
  src/themes/proxmox/Style.cpp src/themes/proxmox/Style.hpp
  src/themes/proxmox/Theme.cpp src/themes/proxmox/Theme.hpp
  src/themes/revolut/Entry.cpp
  src/themes/revolut/Style.cpp src/themes/revolut/Style.hpp
  src/themes/revolut/Theme.cpp src/themes/revolut/Theme.hpp
  src/themes/vscode/Entry.cpp
  src/themes/vscode/Style.cpp src/themes/vscode/Style.hpp
  src/themes/vscode/Theme.cpp src/themes/vscode/Theme.hpp
  src/themes/winamp/Entry.cpp
  src/themes/winamp/Style.cpp src/themes/winamp/Style.hpp
  src/themes/winamp/Theme.hpp
  src/themes/winamp/widgets/Metal.cpp src/themes/winamp/widgets/Metal.hpp
)

target_include_directories(gallery PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)
target_compile_definitions(gallery PRIVATE GALLERY_VERSION="${PROJECT_VERSION}")
target_link_libraries(gallery PRIVATE Qt6::Widgets)
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
  target_compile_options(gallery PRIVATE -Wall -Wextra)
endif()

set_target_properties(gallery PROPERTIES WIN32_EXECUTABLE ON MACOSX_BUNDLE ON)

# ---- installation and packaging ------------------------------------------------------------------
# `cmake --install` and `cpack` produce a self-contained application: the executable with the Qt
# libraries and plugins it loads and the libraries they need, so it runs where Qt is not installed.

if(WIN32 OR APPLE)
  # Qt's own deployment (windeployqt / macdeployqt)
  install(TARGETS gallery RUNTIME DESTINATION bin BUNDLE DESTINATION .)
  if(Qt6_VERSION VERSION_GREATER_EQUAL 6.5)
    qt_generate_deploy_app_script(TARGET gallery OUTPUT_SCRIPT deploy_script NO_UNSUPPORTED_PLATFORM_ERROR)
  else()
    qt_generate_deploy_app_script(TARGET gallery FILENAME_VARIABLE deploy_script NO_UNSUPPORTED_PLATFORM_ERROR)
  endif()
  install(SCRIPT ${deploy_script})
else()
  include(cmake/DeployLinux.cmake)
  gallery_deploy_linux(gallery PLUGINS
    Qt6::QXcbIntegrationPlugin             # X11
    Qt6::QXcbGlxIntegrationPlugin
    Qt6::QXcbEglIntegrationPlugin
    Qt6::QOffscreenIntegrationPlugin       # -platform offscreen (screenshots)
    Qt6::QMinimalIntegrationPlugin
    Qt6::QComposePlatformInputContextPlugin # dead keys and compose sequences
  )
endif()

set(CPACK_PACKAGE_NAME "widget-gallery")
set(CPACK_PACKAGE_VENDOR "Francesco de Gioia")
set(CPACK_PACKAGE_CONTACT "Francesco de Gioia <francescodg4@gmail.com>")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "The standard Qt widgets in nine interface designs")
set(CPACK_PACKAGE_INSTALL_DIRECTORY "Widget Gallery")
set(CPACK_PACKAGE_EXECUTABLES "gallery;Widget Gallery")
set(CPACK_RESOURCE_FILE_README "${CMAKE_CURRENT_SOURCE_DIR}/README.md")
set(CPACK_STRIP_FILES ON)
set(CPACK_PROJECT_CONFIG_FILE "${CMAKE_CURRENT_SOURCE_DIR}/cmake/CPackOptions.cmake")
if(WIN32)
  set(CPACK_GENERATOR "ZIP")
elseif(APPLE)
  set(CPACK_GENERATOR "DragNDrop")
else()
  set(CPACK_GENERATOR "TGZ;DEB")
  # what the bundle expects from the system (see cmake/DeployLinux.cmake)
  set(CPACK_DEBIAN_PACKAGE_DEPENDS "libc6, libstdc++6, libgl1, libegl1, libopengl0, libx11-6, libx11-xcb1, libxcb1, libfontconfig1, libfreetype6, zlib1g, libexpat1")
  set(CPACK_DEBIAN_PACKAGE_SECTION "graphics")
  set(CPACK_DEBIAN_FILE_NAME DEB-DEFAULT)
endif()
include(CPack)
