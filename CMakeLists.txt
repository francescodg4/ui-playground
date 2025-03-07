cmake_minimum_required(VERSION 3.21)

project(PdaDemo VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()

find_package(Qt6 6.4 REQUIRED COMPONENTS Widgets)
qt_standard_project_setup()

qt_add_executable(pda
  src/main.cpp
  src/Icons.cpp src/Icons.hpp
  src/PdaShell.hpp
  src/PhotoLibrary.cpp src/PhotoLibrary.hpp
  src/ThemeController.cpp src/ThemeController.hpp
  src/ThemeDialog.cpp src/ThemeDialog.hpp
  src/ThemeRegistry.cpp src/ThemeRegistry.hpp
  src/themes/emerald/Entry.cpp
  src/themes/emerald/PdaWindow.cpp src/themes/emerald/PdaWindow.hpp
  src/themes/emerald/Theme.cpp src/themes/emerald/Theme.hpp
  src/themes/emerald/pages/BlueprintsPage.cpp src/themes/emerald/pages/BlueprintsPage.hpp
  src/themes/emerald/pages/EncyclopediaPage.cpp src/themes/emerald/pages/EncyclopediaPage.hpp
  src/themes/emerald/pages/InventoryPage.cpp src/themes/emerald/pages/InventoryPage.hpp
  src/themes/emerald/pages/LogPage.cpp src/themes/emerald/pages/LogPage.hpp
  src/themes/emerald/pages/PhotoPage.cpp src/themes/emerald/pages/PhotoPage.hpp
  src/themes/emerald/pages/PingPage.cpp src/themes/emerald/pages/PingPage.hpp
  src/themes/emerald/widgets/BoxHeader.cpp src/themes/emerald/widgets/BoxHeader.hpp
  src/themes/emerald/widgets/Pixel.cpp src/themes/emerald/widgets/Pixel.hpp
  src/themes/emerald/widgets/PixelWidgets.cpp src/themes/emerald/widgets/PixelWidgets.hpp
  src/themes/glass/Entry.cpp
  src/themes/glass/PdaWindow.cpp src/themes/glass/PdaWindow.hpp
  src/themes/glass/Theme.cpp src/themes/glass/Theme.hpp
  src/themes/glass/pages/BlueprintsPage.cpp src/themes/glass/pages/BlueprintsPage.hpp
  src/themes/glass/pages/EncyclopediaPage.cpp src/themes/glass/pages/EncyclopediaPage.hpp
  src/themes/glass/pages/InventoryPage.cpp src/themes/glass/pages/InventoryPage.hpp
  src/themes/glass/pages/LogPage.cpp src/themes/glass/pages/LogPage.hpp
  src/themes/glass/pages/PhotoPage.cpp src/themes/glass/pages/PhotoPage.hpp
  src/themes/glass/pages/PingPage.cpp src/themes/glass/pages/PingPage.hpp
  src/themes/glass/widgets/ClayButton.cpp src/themes/glass/widgets/ClayButton.hpp
  src/themes/glass/widgets/Glass.cpp src/themes/glass/widgets/Glass.hpp
  src/themes/glass/widgets/Holo.cpp src/themes/glass/widgets/Holo.hpp
  src/themes/glass/widgets/PageTransition.cpp src/themes/glass/widgets/PageTransition.hpp
  src/themes/glass/widgets/TabBar.cpp src/themes/glass/widgets/TabBar.hpp
)

target_include_directories(pda PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)
target_compile_definitions(pda PRIVATE
  PDA_VERSION="${PROJECT_VERSION}"
  PDA_DEFAULT_PHOTO_DIR="${PROJECT_SOURCE_DIR}/photos"
)
target_link_libraries(pda PRIVATE Qt6::Widgets)
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
  target_compile_options(pda PRIVATE -Wall -Wextra)
endif()

set_target_properties(pda PROPERTIES WIN32_EXECUTABLE ON MACOSX_BUNDLE ON)
install(TARGETS pda RUNTIME DESTINATION bin BUNDLE DESTINATION .)
