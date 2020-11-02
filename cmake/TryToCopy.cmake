cmake_policy(VERSION 3.16.1)
#
# Tries to copy file from TTC_SRC_FILE_PATH to TTC_TARGET_FOLDER_PATH if it isn't already there
# (If not success - nothing happens)
# 
# use in cases where error during copy should be treated as silent error 
# (otherwise the whole build would be treated as failed)
#
#
get_filename_component(TTC_TARGET_FIL "${TTC_SRC_FILE_PATH}" NAME)

if(NOT EXISTS "${TTC_SRC_FILE_PATH}" OR EXISTS "${TTC_TARGET_FOLDER_PATH}/${TTC_TARGET_FIL}")
   #nothing happens
else()
    message("Copying ${TTC_SRC_FILE_PATH} to ${TTC_TARGET_FOLDER_PATH}/")
    file(COPY "${TTC_SRC_FILE_PATH}" DESTINATION "${TTC_TARGET_FOLDER_PATH}/")
endif()


