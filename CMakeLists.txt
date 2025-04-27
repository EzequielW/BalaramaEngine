set(CMAKE_LINKER "C:/clang/bin/lld-link.exe")

cmake_minimum_required(VERSION 3.15)
project(BalaramaEngine)


set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(SDL2 REQUIRED) 
find_package(SDL2_image REQUIRED) 
find_package(SDL2_ttf REQUIRED) 
find_package(BZip2 REQUIRED) 
find_package(ZLIB REQUIRED) 
find_package(PNG REQUIRED) 
find_package(Freetype REQUIRED) 

file(GLOB_RECURSE SOURCES "src/*.cpp")
file(GLOB_RECURSE HEADERS "src/*.h")
file(GLOB_RECURSE SPRITES "src/GUI/assets/*.png")

add_executable(BalaramaEngine
  ${SOURCES}
  ${HEADERS}
)

file(COPY
  src/Sans.ttf 
  DESTINATION
  # ${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_BUILD_TYPE}/
  ${CMAKE_CURRENT_BINARY_DIR}/
)

file(COPY
  ${SPRITES}
  DESTINATION
  ${CMAKE_CURRENT_BINARY_DIR}/GUI/assets/
)

# Link the dependencies
target_link_libraries(BalaramaEngine
    PRIVATE
    SDL2::SDL2
    SDL2_image::SDL2_image
    SDL2_ttf::SDL2_ttf
    ZLIB::ZLIB
    PNG::PNG
    BZip2::BZip2
    Freetype::Freetype
)