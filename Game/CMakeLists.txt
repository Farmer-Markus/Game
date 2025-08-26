set(SOURCES_SUBDIRS )
macro(addDirectory dir)
    file(GLOB SUB_FILES ${dir}/*.cpp ${dir}/*.h ${dir}/*.hpp)
    set(SOURCES_SUBDIRS ${SOURCES_SUBDIRS} ${SUB_FILES})
endmacro()

addDirectory(graphics)
addDirectory(utils)

add_executable(game main.cpp ${SOURCES_SUBDIRS})
target_include_directories(game PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(game glad SDL3::SDL3)
set_property(TARGET game PROPERTY CXX_STANDART 17)
set_property(TARGET game PROPERTY CXX_STANDART_REQUIRED ON)