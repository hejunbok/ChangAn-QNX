TEMPLATE = subdirs

qnx:SUBDIRS += dirwatcher object
SUBDIRS += variant
!qnx: SUBDIRS += simulator
