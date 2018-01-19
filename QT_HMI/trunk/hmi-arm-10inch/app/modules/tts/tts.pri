DEPENDPATH += $$PWD
INCLUDEPATH += $$PWD

HEADERS += \
	#$$PWD/TTSPlayer.h \
	#$$PWD/PCMPlayer.h \
	modules/tts/TTSPlayer.h \
	modules/tts/PCMPlayer.h \
	modules/tts/include/iss_chipauth.h \
	modules/tts/include/iss_errors.h \
	modules/tts/include/iss_mvw.h \
	modules/tts/include/iss_sr.h \
	modules/tts/include/iss_tts.h \
	modules/tts/include/iss_types.h \
    modules/tts/iss_auth.h \
    modules/tts/iss_chipauth.h \
    modules/tts/iss_errors.h \
    modules/tts/iss_mvw.h \
    modules/tts/iss_sr.h \
    modules/tts/iss_tts.h \
    modules/tts/iss_types.h \
    modules/tts/PCMType.h

SOURCES += \
	$$PWD/TTSPlayer.cpp \
	$$PWD/PCMPlayer.cpp
