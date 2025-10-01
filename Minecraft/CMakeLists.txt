set(SOURCES_SUBDIRS )
macro(addDirectory dir)
    file(GLOB SUB_FILES ${dir}/*.cpp ${dir}/*.h ${dir}/*.hpp)
    set(SOURCES_SUBDIRS ${SOURCES_SUBDIRS} ${SUB_FILES})
endmacro()

addDirectory(graphics)
addDirectory(utils)
addDirectory(config)

add_executable(minecraft main.cpp ${SOURCES_SUBDIRS})
target_include_directories(minecraft PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(minecraft glm stb glad SDL3::SDL3)

set_property(TARGET minecraft PROPERTY CXX_STANDARD 17)
set_property(TARGET minecraft PROPERTY CXX_STANDART_REQUIRED ON)
target_compile_definitions(minecraft PRIVATE $<$<CONFIG:DEBUG>:DEBUG>)
