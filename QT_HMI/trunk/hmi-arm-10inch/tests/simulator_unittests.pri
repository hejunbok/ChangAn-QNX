# qpps lib
INCLUDEPATH += $$PWD/../qpps
DEPENDPATH += $$PWD/../qpps
win32: LIBPATH += $$BUILD_ROOT/bin
else: LIBPATH += $$BUILD_ROOT/lib
LIBS += -lqpps
QMAKE_RPATHDIR += $$BUILD_ROOT/qpps/lib
