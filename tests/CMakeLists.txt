find_package(Qt6 REQUIRED COMPONENTS Test)

qt_add_executable(tst_pda
  tst_pda.cpp
  ../src/Icons.cpp
  ../src/PhotoLibrary.cpp
)
target_include_directories(tst_pda PRIVATE ../src)
target_link_libraries(tst_pda PRIVATE Qt6::Widgets Qt6::Test)
set_target_properties(tst_pda PROPERTIES WIN32_EXECUTABLE OFF MACOSX_BUNDLE OFF)

add_test(NAME tst_pda COMMAND tst_pda -platform offscreen)
