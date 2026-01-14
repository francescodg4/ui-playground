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
  src/themes/emerald/Entry.cpp
  src/themes/emerald/Style.cpp src/themes/emerald/Style.hpp
  src/themes/emerald/Theme.hpp
  src/themes/emerald/widgets/Pixel.cpp src/themes/emerald/widgets/Pixel.hpp
  src/themes/glass/Entry.cpp
  src/themes/glass/Style.cpp src/themes/glass/Style.hpp
  src/themes/glass/Theme.cpp src/themes/glass/Theme.hpp
  src/themes/glass/widgets/Glass.cpp src/themes/glass/widgets/Glass.hpp
  src/themes/metro/Entry.cpp
  src/themes/metro/Style.cpp src/themes/metro/Style.hpp
  src/themes/metro/Theme.cpp src/themes/metro/Theme.hpp
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
install(TARGETS gallery RUNTIME DESTINATION bin BUNDLE DESTINATION .)
