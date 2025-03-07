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
  src/PdaWindow.cpp src/PdaWindow.hpp
  src/PhotoLibrary.cpp src/PhotoLibrary.hpp
  src/Theme.cpp src/Theme.hpp
  src/pages/BlueprintsPage.cpp src/pages/BlueprintsPage.hpp
  src/pages/EncyclopediaPage.cpp src/pages/EncyclopediaPage.hpp
  src/pages/InventoryPage.cpp src/pages/InventoryPage.hpp
  src/pages/LogPage.cpp src/pages/LogPage.hpp
  src/pages/PhotoPage.cpp src/pages/PhotoPage.hpp
  src/pages/PingPage.cpp src/pages/PingPage.hpp
  src/widgets/Holo.cpp src/widgets/Holo.hpp
  src/widgets/TabBar.cpp src/widgets/TabBar.hpp
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
