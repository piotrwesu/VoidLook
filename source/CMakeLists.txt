set(VoidLook_SRC_FILES main.c)

add_executable(${PROJECTNAME} ${VoidLook_SRC_FILES})

target_link_libraries(${PROJECTNAME} PRIVATE CURL::libcurl)

set_property(TARGET ${PROJECTNAME} PROPERTY OUTPUT_NAME "voidlook")
