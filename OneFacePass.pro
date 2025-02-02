QT       += core widgets gui network multimedia multimediawidgets concurrent

TARGET          = OneFacePass
TEMPLATE        = app
MOC_DIR         = temp/moc
RCC_DIR         = temp/rcc
UI_DIR          = temp/ui
OBJECTS_DIR     = temp/obj
DESTDIR         = $$PWD/bin
#win32:RC_FILE   = other/main.rc
PRECOMPILED_HEADER  = head.h

DEFINES +=  DEBUG
#DEFINES +=  DEBUG_FACE
DEFINES +=  DEBUG_THREAD
#DEFINES += DEBUG_CONFIG


INCLUDEPATH     +=  $$PWD        \
                    $$PWD/form  \
                    $$(OPENCV)/include/    \
                    $$(ARCFACE)/inc              \
                    $$PWD/3rdparty/face_module/include   \
                    $$PWD/3rdparty/jsoncpp/include

#CONFIG          += qt warn_off

LIBS            +=  $$(OPENCV)/x64/vc15/lib/opencv_world346d.lib   \
                    $$PWD/3rdparty/face_module/lib/face_module.lib     \
#                    C:/Workspace/face_recognition/x64/Debug/face_module.lib     \
                    $$(ARCFACE)/lib/X64/libarcsoft_face_engine.lib      \
                    $$PWD/3rdparty/jsoncpp/lib/jsoncppd.lib



SOURCES += main.cpp\
    form/cameraviewfinder.cpp \
    form/capturethread.cpp \
    form/facethread.cpp \
    form/quiwidget.cpp \
    form/quicreator.cpp \
    form/appinit.cpp    \
    form/stuwidget.cpp \
    form/posethread.cpp \
    form/config.cpp



HEADERS  += head.h\
    form/cameraviewfinder.h \
    form/capturethread.h \
    form/facethread.h \
    form/quiwidget.h \
    form/quicreator.h \
    form/appinit.h  \
    form/stuwidget.h    \
    form/posethread.h   \
    form/config.h



FORMS    += \
    form/quicreator.ui  \
    form/stuwidget.ui


RESOURCES += \
    other/qss.qrc \
    other/main.qrc
