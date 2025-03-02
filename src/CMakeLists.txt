qt_add_executable(pda
  main.cpp
  Icons.cpp Icons.hpp
  PdaWindow.cpp PdaWindow.hpp
  PhotoLibrary.cpp PhotoLibrary.hpp
  Theme.cpp Theme.hpp
  pages/BlueprintsPage.cpp pages/BlueprintsPage.hpp
  pages/EncyclopediaPage.cpp pages/EncyclopediaPage.hpp
  pages/InventoryPage.cpp pages/InventoryPage.hpp
  pages/LogPage.cpp pages/LogPage.hpp
  pages/PhotoPage.cpp pages/PhotoPage.hpp
  pages/PingPage.cpp pages/PingPage.hpp
  widgets/Holo.cpp widgets/Holo.hpp
  widgets/TabBar.cpp widgets/TabBar.hpp
)

target_include_directories(pda PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_compile_definitions(pda PRIVATE
  PDA_DEFAULT_PHOTO_DIR="${PROJECT_SOURCE_DIR}/photos"
)
target_link_libraries(pda PRIVATE Qt6::Widgets)
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
  target_compile_options(pda PRIVATE -Wall -Wextra)
endif()

set_target_properties(pda PROPERTIES WIN32_EXECUTABLE ON MACOSX_BUNDLE ON)
install(TARGETS pda RUNTIME DESTINATION bin BUNDLE DESTINATION .)
