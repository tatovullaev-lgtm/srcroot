set(INCLUDE_DIRECTORIES
 ${ROOT_INCLUDE_DIR}
 ${XML2_INCLUDE_DIR}
)

include_directories(${INCLUDE_DIRECTORIES})

set(LINK_DIRECTORIES
 ${ROOT_LIBRARY_DIR}
 ${XML2_LIBRARY_DIR}
)

link_directories(${LINK_DIRECTORIES})

#message("Copying file: merge.C to bin directory")
set(DST_FILE_TARGET ${EXECUTABLE_OUTPUT_PATH}/merge.C)
add_custom_command(
    OUTPUT ${DST_FILE_TARGET}
    COMMAND ${CMAKE_COMMAND}
    ARGS -E copy_if_different ${CMAKE_CURRENT_SOURCE_DIR}/src/merge.C ${DST_FILE_TARGET}
    MAIN_DEPENDENCY ${CMAKE_CURRENT_SOURCE_DIR}/src/merge.C
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/src/merge.C
)
add_custom_target(copy-merge-result ALL DEPENDS ${DST_FILE_TARGET})

#message("Copying file: merge.sh to bin directory")
set(DST_FILE_TARGET ${EXECUTABLE_OUTPUT_PATH}/merge.sh)
add_custom_command(
    OUTPUT ${DST_FILE_TARGET}
    COMMAND ${CMAKE_COMMAND}
    ARGS -E copy_if_different ${CMAKE_CURRENT_SOURCE_DIR}/src/merge.sh ${DST_FILE_TARGET}
    MAIN_DEPENDENCY ${CMAKE_CURRENT_SOURCE_DIR}/src/merge.sh
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/src/merge.sh
)
add_custom_target(copy-merge-job ALL DEPENDS ${DST_FILE_TARGET})

if (DEFINED LIBXML2_LIBRARIES) # if we have found XML2 library using find_package, then use this found libxml2
  SET(MY_XML2 ${LIBXML2_LIBRARIES})
else()
  SET(MY_XML2 xml2)
endif()

add_executable(nica-scheduler src/nica-scheduler.cpp)
target_link_libraries(nica-scheduler ${ROOT_LIBRARIES} ${MY_XML2})

INSTALL(FILES ${EXECUTABLE_OUTPUT_PATH}/merge.C DESTINATION ${CMAKE_INSTALL_PREFIX}/bin/)
INSTALL(FILES ${EXECUTABLE_OUTPUT_PATH}/merge.sh DESTINATION ${CMAKE_INSTALL_PREFIX}/bin/)
INSTALL(PROGRAMS ${EXECUTABLE_OUTPUT_PATH}/nica-scheduler DESTINATION ${CMAKE_INSTALL_PREFIX}/bin/)
INSTALL(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/examples/ DESTINATION ${CMAKE_INSTALL_PREFIX}/examples/nica_scheduler)
