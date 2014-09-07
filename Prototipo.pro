#-------------------------------------------------
#
# Project created by QtCreator 2014-06-08T22:01:41
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
qtHaveModule(printsupport): QT += printsupport

TARGET = Prototipo
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    common/commonFilters.cpp \
    cuerpos/Cuerpo.cpp \
    cuerpos/cuerpos.cpp \
    limpieza/limpieza.cpp \
    reconstruction/lineReconstruction.cpp \
    thinning/thinning.cpp \
    support/Exception.cpp \
    vectorize/main2.cpp \
    vectorize/vect.cpp \
    vectorize/vect_catmull.cpp \
    vectorize/vect_RDP.cpp

HEADERS  += mainwindow.h \
    common/commonFilters.h \
    cuerpos/Cuerpo.h \
    cuerpos/cuerpos.h \
    limpieza/limpieza.h \
    reconstruction/lineReconstruction.h \
    thinning/thinning.h \
    support/Exception.h \
    support/Trie.h \
    support/Trie.hxx \
    support/TrieFactory.h \
    support/TrieFactory.hxx \
    support/TrieNode.h \
    support/TrieNode.hxx \
    vectorize/vect.h \
    vectorize/vect_catmull.h \
    vectorize/vect_RDP.h \
    vectorize/shapelib/shapefil.h

FORMS    += mainwindow.ui \
    dialogThinning.ui

wince*: {
   DEPLOYMENT_PLUGIN += qjpeg qgif
}

win32 {
    message("* Using settings for Windows.")

    INCLUDEPATH += "C:\\opencv\\build\\include" \
                   "C:\\opencv\\build\\include\\opencv" \
                   "C:\\opencv\\build\\include\\opencv2"

    LIBS += -L"C:\\opencv\\release\\lib" \
        -lopencv_core249 \
        -lopencv_highgui249 \
        -lopencv_imgproc249
}


unix|win32: LIBS += -L$$PWD/vectorize/shapelib/ -lshapelib

INCLUDEPATH += $$PWD/vectorize/shapelib
DEPENDPATH += $$PWD/vectorize/shapelib
