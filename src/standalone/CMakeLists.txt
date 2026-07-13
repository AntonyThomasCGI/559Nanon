
add_executable(nanon
    main.cpp
)

set_target_properties(nanon PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}"
)

target_link_libraries(nanon PRIVATE nanon_core)


