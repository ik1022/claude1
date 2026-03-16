# CMake module for Doxygen integration in libemb
#
# This module provides a function to setup Doxygen documentation generation
# with automatic GitHub Pages deployment
#
# Usage:
#   libemb_add_doxygen_docs(
#       PROJECT_NAME libemb
#       PROJECT_VERSION "1.0.0"
#       OUTPUT_DIR "docs/generated"
#   )

function(libemb_add_doxygen_docs)
    cmake_parse_arguments(DOXYGEN
        ""
        "PROJECT_NAME;PROJECT_VERSION;OUTPUT_DIR"
        ""
        ${ARGN}
    )

    # Set defaults
    if(NOT DOXYGEN_PROJECT_NAME)
        set(DOXYGEN_PROJECT_NAME "libemb")
    endif()

    if(NOT DOXYGEN_PROJECT_VERSION)
        set(DOXYGEN_PROJECT_VERSION ${PROJECT_VERSION})
    endif()

    if(NOT DOXYGEN_OUTPUT_DIR)
        set(DOXYGEN_OUTPUT_DIR "${CMAKE_CURRENT_BINARY_DIR}/docs")
    endif()

    # Check if Doxygen is installed
    find_package(Doxygen
        REQUIRED
        COMPONENTS dot
        OPTIONAL_COMPONENTS mscgen dia
    )

    # Configure the Doxyfile
    if(EXISTS "${CMAKE_SOURCE_DIR}/Doxyfile.in")
        message(STATUS "Configuring Doxygen documentation for ${DOXYGEN_PROJECT_NAME}")

        # Set variables for substitution
        set(PROJECT_VERSION ${DOXYGEN_PROJECT_VERSION})
        set(CMAKE_CURRENT_BINARY_DIR ${CMAKE_BINARY_DIR})
        set(CMAKE_CURRENT_SOURCE_DIR ${CMAKE_SOURCE_DIR})

        # Configure Doxyfile
        configure_file(
            "${CMAKE_SOURCE_DIR}/Doxyfile.in"
            "${CMAKE_BINARY_DIR}/Doxyfile"
            @ONLY
        )

        # Create docs target
        add_custom_target(docs
            COMMAND ${DOXYGEN_EXECUTABLE}
                ${CMAKE_BINARY_DIR}/Doxyfile
            WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
            COMMENT "Generating Doxygen documentation..."
            VERBATIM
        )

        # Create docs-view target for local viewing
        add_custom_target(docs-view
            COMMAND ${CMAKE_COMMAND} -E echo "Opening documentation in browser..."
            COMMAND xdg-open ${CMAKE_BINARY_DIR}/docs/html/index.html 2>/dev/null ||
                    open ${CMAKE_BINARY_DIR}/docs/html/index.html 2>/dev/null ||
                    start ${CMAKE_BINARY_DIR}/docs/html/index.html 2>/dev/null ||
                    echo "Documentation at: ${CMAKE_BINARY_DIR}/docs/html/index.html"
            DEPENDS docs
            COMMENT "Opening generated documentation..."
        )

        # Create docs-clean target
        add_custom_target(docs-clean
            COMMAND ${CMAKE_COMMAND} -E remove_directory ${CMAKE_BINARY_DIR}/docs
            COMMENT "Cleaning generated documentation..."
        )

        message(STATUS "Doxygen targets created:")
        message(STATUS "  - cmake --build . --target docs       # Generate documentation")
        message(STATUS "  - cmake --build . --target docs-view  # Generate and view")
        message(STATUS "  - cmake --build . --target docs-clean # Remove generated files")

    else()
        message(WARNING "Doxyfile.in not found at ${CMAKE_SOURCE_DIR}/Doxyfile.in")
    endif()

endfunction()


# Function to add Doxygen documentation to a target
# This is useful for embedding documentation tags in code
function(libemb_add_target_docs TARGET_NAME)
    cmake_parse_arguments(TARGET_DOCS
        ""
        "BRIEF;DETAILS"
        ""
        ${ARGN}
    )

    if(TARGET_DOCS_BRIEF)
        get_target_property(TARGET_TYPE ${TARGET_NAME} TYPE)
        message(STATUS "Documentation for ${TARGET_NAME}: ${TARGET_DOCS_BRIEF}")
    endif()

endfunction()


# Helper function to check documentation coverage
function(libemb_check_doc_coverage)
    cmake_parse_arguments(COVERAGE
        ""
        "THRESHOLD"
        ""
        ${ARGN}
    )

    if(NOT COVERAGE_THRESHOLD)
        set(COVERAGE_THRESHOLD 80)
    endif()

    message(STATUS "Documentation coverage check is not implemented yet")
    message(STATUS "Set THRESHOLD to ${COVERAGE_THRESHOLD}%")

endfunction()
