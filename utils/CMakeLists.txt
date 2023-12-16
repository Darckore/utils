cmake_minimum_required(VERSION 3.26)
include("${OPT_UTILS_DIR}/utils.cmake")

project(${TARGET_NAME} CXX)

set(DOCS_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../docs)
file(GLOB_RECURSE DOC_FILES
     LIST_DIRECTORIES false
     "${DOCS_DIR}/*.md"
     "${DOCS_DIR}/*/*.md" )

collect_sources(SOURCE_FILES HEADERS ADDITIONAL_FILES)
add_library(${TARGET_NAME} ${SOURCE_FILES} ${HEADERS} ${OPT_PCH_NAME} ${ADDITIONAL_FILES} ${DOC_FILES} utils.natvis utils.natstepfilter)
set_build_opts(${TARGET_NAME} "${ADDITIONAL_FILES}")
make_src_groups("${SOURCE_FILES}" "${HEADERS}" "${ADDITIONAL_FILES}")
source_group(TREE ${DOCS_DIR}/.. FILES ${DOC_FILES})