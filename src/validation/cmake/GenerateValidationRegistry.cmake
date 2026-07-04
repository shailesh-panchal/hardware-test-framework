#
# GenerateValidationRegistry.cmake
#

message(STATUS "Generating Validation Registry")

#
# Find every validation source
#
file(GLOB_RECURSE VALIDATION_SOURCES
    "${PROJECT_SOURCE_DIR}/src/validation/*.c"
)

set(VALIDATION_EXTERN_DECLARATIONS "")
set(VALIDATION_TABLE "")

foreach(SOURCE_FILE ${VALIDATION_SOURCES})

    file(READ "${SOURCE_FILE}" FILE_CONTENT)

    #
    # Search:
    #
    # VALIDATION_EXPORT(status_indication)
    #
    string(REGEX MATCH
        "VALIDATION_EXPORT[ \t\r\n]*\\(([A-Za-z0-9_]+)\\)"
        MATCH_RESULT
        "${FILE_CONTENT}"
    )

    if(MATCH_RESULT)

        set(TEST_NAME "${CMAKE_MATCH_1}")

        message(STATUS "  Validation : ${TEST_NAME}")

        string(APPEND VALIDATION_EXTERN_DECLARATIONS
"extern validation_descriptor_t *
${TEST_NAME}_get_descriptor(void);

")

    string(APPEND VALIDATION_TABLE
"    ${TEST_NAME}_get_descriptor,
")

    endif()

endforeach()

configure_file(

    "${PROJECT_SOURCE_DIR}/src/validation/cmake/validation_registry.c.in.txt"

    "${PROJECT_SOURCE_DIR}/src/validation/generated/validation_registry.c"

    @ONLY
)