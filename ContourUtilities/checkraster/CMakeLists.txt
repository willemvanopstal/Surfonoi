add_executable(checkraster main.cpp)
target_link_libraries(checkraster ${GDAL_LIBRARY})

install (TARGETS checkraster DESTINATION bin)