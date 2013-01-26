LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := libdownload_static

LOCAL_MODULE_FILENAME := libdownload

define all-cpp-files-under
$(patsubst ./%,%, $(shell find $(LOCAL_PATH) -name "*.cpp" -and -not -name ".*"))
endef

define all-subdir-cpp-files
$(call all-cpp-files-under,.)
endef

define all-c-files-under
$(patsubst ./%,%, $(shell find $(LOCAL_PATH) -name "platform" -prune -o -name "*.c" -and -not -name ".*"))
endef
  
define all-subdir-c-files
$(call all-c-files-under,.)
endef

CPP_FILE_LIST := $(call all-subdir-cpp-files)
C_FILE_LIST := $(call all-subdir-c-files)

LOCAL_SRC_FILES := $(CPP_FILE_LIST:$(LOCAL_PATH)/%=%)
LOCAL_SRC_FILES += $(C_FILE_LIST:$(LOCAL_PATH)/%=%)
               
LOCAL_EXPORT_C_INCLUDES := $(LOCAL_PATH)/
                                                     
LOCAL_C_INCLUDES := $(LOCAL_PATH)         
                    
LOCAL_LDLIBS := -lpthread

LOCAL_WHOLE_STATIC_LIBRARIES := basecode_static

include $(BUILD_STATIC_LIBRARY)
$(call import-module, basecode)

