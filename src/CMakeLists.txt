qt_add_executable(pda
  main.cpp
  Icons.cpp Icons.hpp
  PdaWindow.cpp PdaWindow.hpp
  Theme.cpp Theme.hpp
  pages/BlueprintsPage.cpp pages/BlueprintsPage.hpp
  pages/InventoryPage.cpp pages/InventoryPage.hpp
  pages/PingPage.cpp pages/PingPage.hpp
  widgets/Holo.cpp widgets/Holo.hpp
  widgets/TabBar.cpp widgets/TabBar.hpp
)

target_include_directories(pda PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(pda PRIVATE Qt6::Widgets)
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
  target_compile_options(pda PRIVATE -Wall -Wextra)
endif()

set_target_properties(pda PROPERTIES WIN32_EXECUTABLE ON MACOSX_BUNDLE ON)
install(TARGETS pda RUNTIME DESTINATION bin BUNDLE DESTINATION .)
