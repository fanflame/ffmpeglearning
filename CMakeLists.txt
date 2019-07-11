cmake_minimum_required(VERSION 3.12)
project(ffmpeg)

set(CMAKE_CXX_STANDARD 11)


set(FFMPEG_DIR /usr/local/Cellar/ffmpeg/4.1.4)
include_directories(${FFMPEG_DIR}/include/)
link_directories(${FFMPEG_DIR}/lib/)

add_executable(ffmpeg examples/encode_video.c)
#add_executable(ffmpeg main.cpp)

target_link_libraries(
        ffmpeg
        avcodec
        avdevice
        avfilter
        avformat
        avresample
        avutil
        postproc
        swresample
        swscale
)
