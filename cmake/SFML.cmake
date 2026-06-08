# set SFML_TARGETS to the list of SFML:: components found by find_package
# This is necessary to set the MAP_IMPORTED_CONFIG_RELWITHDEBINFO property for all SFML targets
# While at it, it simplifies the target_link_libraries call in CMakeLists.txt
set (SFML_TARGETS "")
foreach (COMPONENT ${FIND_SFML_COMPONENTS_SORTED})
    list (APPEND SFML_TARGETS "SFML::${COMPONENT}")
endforeach()

# This is required so CMake will link with SFML release DLLs instead of debug ones when building in RelWithDebInfo configuration
set_target_properties (${SFML_TARGETS} PROPERTIES MAP_IMPORTED_CONFIG_RELWITHDEBINFO RELEASE)

# Copy SFML-related DLLs to the target folder, to make the executable runnable outside of VS too
add_custom_command(TARGET ${CMAKE_PROJECT_NAME} POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_RUNTIME_DLLS:${CMAKE_PROJECT_NAME}> $<TARGET_FILE_DIR:${CMAKE_PROJECT_NAME}>
  COMMAND_EXPAND_LISTS
)

target_link_libraries (${CMAKE_PROJECT_NAME} ${SFML_TARGETS})
