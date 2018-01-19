TEMPLATE = subdirs

SUBDIRS += \
    modules \

!qnx:SUBDIRS += \
    screenmanager \
    util \

qnx:SUBDIRS += \
    windowmanager \

qnx:SUBDIRS += \
    kanzitest \
