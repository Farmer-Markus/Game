set(SOURCES_SUBDIRS )
macro(addDirectory dir)
    file(GLOB SUB_FILES ${dir}/*.cpp ${dir}/*.h ${dir}/*.hpp)
    set(SOURCES_SUBDIRS ${SOURCES_SUBDIRS} ${SUB_FILES})
endmacro()

addDirectory(graphics)

add_executable(game main.cpp ${SOURCES_SUBDIRS})
target_link_libraries(game glad SDL3::SDL3)