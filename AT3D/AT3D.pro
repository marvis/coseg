# #####################################################################
# Automatically generated by qmake (2.01a) Sat Jun 26 22:25:52 2010
# #####################################################################
TEMPLATE = app
TARGET = 
DEPENDPATH += . \
    images
INCLUDEPATH += .
QT += opengl
CONFIG += qt \
    debug
LIBS += -ltiff \
    -lGLEW \
    -llpsolve55

# Input
HEADERS += widgets/glwidget.h \
    widgets/cellwidget.h \
    ../myalgorithms.h \
    at3d_view.h \
    ../component_tree.h \
    ../CT3D/cell_track_controller.h \
    ../CT3D/cell_track.h \
    ../CT3D/palette.h \
    dialogs/createdialog.h \
    dialogs/filterdialog.h \
    extends/cell_track_ex.h \
    dialogs/finetuningdialog.h
FORMS += widgets/at3d_view.ui \
    dialogs/createdialog.ui \
    dialogs/finetuningdialog.ui \
    dialogs/filterdialog.ui 
SOURCES += widgets/glwidget.cpp \
    widgets/cellwidget.cpp \
    ../myalgorithms.cpp \
    at3d_view.cpp \
    main.cpp \
    ../component_tree.cpp \
    ../CT3D/cell_track_controller.cpp \
    ../CT3D/cell_track.cpp \
    ../CT3D/palette.cpp \
    dialogs/createdialog.cpp \
    dialogs/filterdialog.cpp \
    extends/cell_track_ex.cpp \
    dialogs/finetuningdialog.cpp
