project (MshInterface)

set (Dependencies )

include_directories (${CMAKE_SOURCE_DIR}/include)

file (GLOB_RECURSE ${PROJECT_NAME}_sources ${PROJECT_SOURCE_DIR}/source/*.cpp)

add_library (${PROJECT_NAME} ${${PROJECT_NAME}_sources})

foreach (Dependency ${Dependencies})
    target_link_libraries (${PROJECT_NAME} ${Dependency})
endforeach ()
