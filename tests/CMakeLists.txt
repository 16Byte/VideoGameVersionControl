cmake_minimum_required(VERSION 3.20)

# Find Qt Test module
find_package(Qt6 REQUIRED COMPONENTS Test)

# Enable testing
enable_testing()

# Helper function to create a test
function(add_vgvc_test TEST_NAME TEST_SOURCE)
    add_executable(${TEST_NAME} ${TEST_SOURCE})
    
    target_link_libraries(${TEST_NAME}
        Qt6::Core
        Qt6::Test
        Qt6::Concurrent
    )
    
    # Add the src directory for includes
    target_include_directories(${TEST_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/src)
    
    add_test(NAME ${TEST_NAME} COMMAND ${TEST_NAME})
endfunction()

# Create test executables
add_vgvc_test(test_gitservice test_gitservice.cpp)
add_vgvc_test(test_snapshotmanager test_snapshotmanager.cpp)
add_vgvc_test(test_presetmanager test_presetmanager.cpp)

# Note: These tests need to link against the actual implementation files
# For now, they're just stubs
