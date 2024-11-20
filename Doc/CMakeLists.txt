find_package(Doxygen)

if(DOXYGEN_FOUND)
    message(STATUS "Doxygen found")
else()
    message(STATUS "Doxygen not found")
endif()

set(CMAKE_DOXYGEN_MAINPAGE "${CMAKE_CURRENT_SOURCE_DIR}/mainpage.md")

set(CMAKE_DOXYGEN_OUTPUT_PATH
    "${CMAKE_CURRENT_SOURCE_DIR}/doxygen"
)

set(CMAKE_DOXYGEN_INPUT_LIST
    "${CMAKE_SOURCE_DIR}/Source"
    "${CMAKE_DOXYGEN_MAINPAGE}"
)

set(CMAKE_DOXYGEN_IMAGE_PATH
    "${CMAKE_SOURCE_DIR}/Doc/assets"
)

string(REPLACE ";" " \\\n                         " CMAKE_DOXYGEN_INPUT_LIST "${CMAKE_DOXYGEN_INPUT_LIST}")
string(REPLACE ";" " \\\n                         " CMAKE_DOXYGEN_OUTPUT_PATH "${CMAKE_DOXYGEN_OUTPUT_PATH}")

configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/CMake/Doxyfile.in"
    ${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile
)

doxygen_add_docs(
    docs
    CONFIG_FILE ${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile
)
