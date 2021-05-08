QT     += core gui sql widgets concurrent network multimedia

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    admform.cpp \
    chatbase.cpp \
    log.cpp \
    main.cpp \
    mainform.cpp \
    mngform.cpp \
    notes.cpp \
    officesystem.cpp \
    popnotify.cpp \
    safety.cpp \
    scrform.cpp \
    settingfile.cpp \
    userprofile.cpp

HEADERS += \
    admform.h \
    chatbase.h \
    log.h \
    mainform.h \
    mngform.h \
    notes.h \
    officesystem.h \
    popnotify.h \
    safety.h \
    scrform.h \
    settingfile.h \
    userprofile.h

FORMS += \
    admform.ui \
    mainform.ui \
    mngform.ui \
    scrform.ui

LIBS += -lws2_32
LIBS += -lwsock32

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

DISTFILES += \
    asd.ico \
    icon.rc \

win32:RC_FILE += icon.rc
RESOURCES += \
  images.qrc \
  sound.qrc
