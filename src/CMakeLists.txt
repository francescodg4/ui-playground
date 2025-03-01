qt_add_executable(pda
  main.cpp
  Theme.cpp Theme.hpp
)

target_include_directories(pda PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(pda PRIVATE Qt6::Widgets)
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
  target_compile_options(pda PRIVATE -Wall -Wextra)
endif()

set_target_properties(pda PROPERTIES WIN32_EXECUTABLE ON MACOSX_BUNDLE ON)
install(TARGETS pda RUNTIME DESTINATION bin BUNDLE DESTINATION .)
