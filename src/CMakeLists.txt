set(SOURCES arfcn_freq.cc c0_detect.cc circular_buffer.cc fcch_detector.cc kal.cc offset.cc usrp_source.cc util.cc)
add_executable(kal ${SOURCES})
target_link_libraries(kal ${UHD_LIBRARIES} ${FFTW3_LIBRARIES})
