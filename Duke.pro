TEMPLATE = app

TARGET = Duke

QT += quick widgets opengl

SOURCES += \
    blobdetector.cpp \
    dahengcamera.cpp \
    focusassistant.cpp \
    glwidget.cpp \
    graycodes.cpp \
    imageviewer.cpp \
    main.cpp \
    mainwindow.cpp \
    meshcreator.cpp \
    pointcloudimage.cpp \
    projector.cpp \
    reconstruct.cpp \
    set.cpp \
    utilities.cpp \
    virtualcamera.cpp \
    plyloader.cpp \
    cameracalibration.cpp \
    multifrequency.cpp \
    dotmatch.cpp\
    manualmatch.cpp \
    imageviewer.cpp \
    stereorect.cpp \
    mfreconstruct.cpp \
    blobdetector.cpp \
    blobdetector.cpp \


RESOURCES += \
    Resource/res.qrc
# MSVCRT link option (static or dynamic, it must be the same with your Qt SDK link option)


INCLUDEPATH += E:\opencv\build\include\
D:\DA\VC\inc\
D:\mrpt1.2.2\libs\base\include\
D:\mrpt1.2.2\libs\scanmatching\include\
D:\mrpt1.2.2\include\mrpt\mrpt-config\
D:\glm\

LIBS+=-LD:\mrpt1.2.2\lib\
-LE:\opencv\build\x64\vc12\lib\
-LD:\DA\VC\lib\
-lopencv_core249d\
-lopencv_highgui249d\
-lopencv_imgproc249d\
-lopencv_features2d249d\
-lopencv_calib3d249d\
-lopencv_nonfree249d\
-lopencv_flann249d\
-lHVDAILT\
-lHVExtend\
-lHVUtil\
-lRaw2Rgb\
-llibmrpt-base122-dbg\
-llibmrpt-scanmatching122-dbg\


# Default rules for deployment.
include(deployment.pri)

HEADERS += \
    graycodes.h \
    mainwindow.h \
    meshcreator.h \
    pointcloudimage.h \
    projector.h \
    reconstruct.h \
    set.h \
    utilities.h \
    virtualcamera.h \
    plyloader.h \
    glwidget.h \
    cameracalibration.h \
    dotmatch.h \
    multifrequency.h \
    blobdetector.h \
    dahengcamera.h \
    focusassistant.h \
    manualmatch.h \
    imageviewer.h \
    stereorect.h \
    mfreconstruct.h \




FORMS += \
    mainwindow.ui \
    Set.ui \
    focusassistant.ui \
    manualmatch.ui \
    imageviewer.ui

TRANSLATIONS += en.ts zh.ts
