set(VoidLook_SRC_FILES main.c
                        include/apikey.c
                        include/https_client.c
                        include/sjson.c
                        include/image.c)

add_executable(${PROJECTNAME} ${VoidLook_SRC_FILES})

target_link_libraries(${PROJECTNAME} PRIVATE CURL::libcurl
                                                stb
                                                sixel)

target_include_directories(${PROJECTNAME} PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/include")

set_property(TARGET ${PROJECTNAME} PROPERTY OUTPUT_NAME "voidlook")
