# Gather test source files
file(GLOB_RECURSE TEST_SRC "*.cpp" "*.h" "*.hpp")

# Create the test executable
add_executable(test-surviving-sarntal ${TEST_SRC})

# Add the source files to the test executable for coverage
target_sources(test-surviving-sarntal PRIVATE ${SRC})

target_include_directories(test-surviving-sarntal PUBLIC ".")
target_link_libraries(test-surviving-sarntal PRIVATE GTest::gtest_main pthread surviving-sarntal-library)
target_link_libraries(test-surviving-sarntal PRIVATE GTest::gmock pthread surviving-sarntal-library)

# Set working directory for Visual Studio
set_target_properties(
        surviving-sarntal PROPERTIES
        VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/bin"
)

# Discover tests with GoogleTest
include(GoogleTest)
gtest_discover_tests(test-surviving-sarntal)

find_program(LCOV_PATH lcov)
find_program(GENHTML_PATH genhtml)

if (ENABLE_COVERAGE)
    if (NOT LCOV_PATH)
        message(FATAL_ERROR "lcov not found!")
    endif ()

    if (NOT GENHTML_PATH)
        message(FATAL_ERROR "genhtml not found!")
    endif ()

    # Get the current timestamp for default folder naming if custom name is not provided
    if (COVERAGE_DIR_NAME STREQUAL "")
        execute_process(
                COMMAND date "+%Y-%m-%d_%H-%M-%S"
                OUTPUT_VARIABLE current_time
                OUTPUT_STRIP_TRAILING_WHITESPACE
        )
        set(COVERAGE_DIR_NAME "coverage_${current_time}")
    endif()

    # Generate a timestamp for the coverage directory name
    execute_process(
            COMMAND date "+%Y-%m-%d_%H-%M-%S"
            OUTPUT_VARIABLE current_time
            OUTPUT_STRIP_TRAILING_WHITESPACE
    )

    # Create a custom target to generate coverage report
    add_custom_target(coverage
            COMMAND rm -rf ${CMAKE_BINARY_DIR}/../${COVERAGE_DIR_NAME}
            COMMAND mkdir -p ${CMAKE_BINARY_DIR}/../${COVERAGE_DIR_NAME}
            COMMAND ${LCOV_PATH} -c -i -d ${CMAKE_BINARY_DIR}/src -o ${CMAKE_BINARY_DIR}/../${COVERAGE_DIR_NAME}/base.info
            COMMAND ${LCOV_PATH} --rc geninfo_unexecuted_blocks=1 --capture --directory ${CMAKE_BINARY_DIR}/src --output-file ${CMAKE_BINARY_DIR}/../${COVERAGE_DIR_NAME}/test.info
            COMMAND ${LCOV_PATH} -a ${CMAKE_BINARY_DIR}/../${COVERAGE_DIR_NAME}/base.info -a ${CMAKE_BINARY_DIR}/../${COVERAGE_DIR_NAME}/test.info -o ${CMAKE_BINARY_DIR}/../${COVERAGE_DIR_NAME}/coverage.info
            COMMAND ${LCOV_PATH} --remove ${CMAKE_BINARY_DIR}/../${COVERAGE_DIR_NAME}/coverage.info '/usr/*' --output-file ${CMAKE_BINARY_DIR}/../${COVERAGE_DIR_NAME}/coverage.info  # Exclude system headers
            #COMMAND ${LCOV_PATH} --remove ${CMAKE_BINARY_DIR}/../${COVERAGE_DIR_NAME}/coverage.info '*/libs/*' --output-file ${CMAKE_BINARY_DIR}/../${COVERAGE_DIR_NAME}/coverage.info  # Exclude external libraries
            COMMAND ${LCOV_PATH} --remove ${CMAKE_BINARY_DIR}/../${COVERAGE_DIR_NAME}/coverage.info 'cmake-build-debug/*' --output-file ${CMAKE_BINARY_DIR}/../${COVERAGE_DIR_NAME}/coverage.info     # Exclude libs
            COMMAND ${GENHTML_PATH} ${CMAKE_BINARY_DIR}/../${COVERAGE_DIR_NAME}/coverage.info --output-directory ${CMAKE_BINARY_DIR}/../${COVERAGE_DIR_NAME}/report
            WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
            COMMENT "Generating code coverage report ${current_time}"
            USES_TERMINAL
    )

    # Apply coverage flags to the test target
    target_compile_options(test-surviving-sarntal PRIVATE -fprofile-arcs -ftest-coverage)
    target_link_options(test-surviving-sarntal PRIVATE --coverage)
endif()

