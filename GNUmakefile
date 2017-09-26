#
ifeq ($(GNUSTEP_MAKEFILES),)
 GNUSTEP_MAKEFILES := $(shell gnustep-config --variable=GNUSTEP_MAKEFILES 2>/dev/null)
endif
ifeq ($(GNUSTEP_MAKEFILES),)
 $(error You need to set GNUSTEP_MAKEFILES before compiling!)
endif


include $(GNUSTEP_MAKEFILES)/common.make

ADDITIONAL_OBJCFLAGS += -DUNITKIT_VERSION=\"1.5\"
export ADDITIONAL_OBJCFLAGS

SUBPROJECTS = Source/FrameworkSource Source/ToolSource

include $(GNUSTEP_MAKEFILES)/aggregate.make
