#  
# configuration to find DB_CXX library  
#  
  
# Include dir  
#find_path(THREAD_DB_INCLUDE_DIR
#  NAMES thread_db.h
#  PATHS /usr/include/ /usr/local/include/
#)
  
# Finally the library itself  
find_library(JIN_DEPEND
            NAMES avutil swscale swresample
            PATHS src/main/jniLibs/armeabi-v7a/
              )