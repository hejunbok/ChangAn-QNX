TEMPLATE = subdirs

SUBDIRS = \
    models \
    qnxcar \
    services \
    util

!qnx:SUBDIRS += \
    system
