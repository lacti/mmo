TEMPLATE		= app
FORMS			= signin.ui escmenu.ui inven2.ui dialog.ui browser.ui
RESOURCES		= client.qrc
DESTDIR			= ../bin/x86

CONFIG( debug, debug|release ) {
	MMO_CONFIGURATION = debug
} else {
	MMO_CONFIGURATION = release
}

#QMAKE_CXXFLAGS_DEBUG += /Od

unix {
	MMO_LIB_POSTFIX = 
} else {
	MMO_LIB_POSTFIX = $$join(MMO_CONFIGURATION,,_,.lib)
}

INCLUDEPATH		+= ../include

LIBS			+= -L../lib/x86
LIBS			+= -L../lib/x86/tbb
LIBS			+= rendering_service$$MMO_LIB_POSTFIX
LIBS			+= clientlogic$$MMO_LIB_POSTFIX
LIBS			+= mmobase$$MMO_LIB_POSTFIX
#LIBS			+= tbb$$MMO_LIB_POSTFIX

OBJECTS_DIR	= $$MMO_CONFIGURATION/.obj
MOC_DIR		= $$MMO_CONFIGURATION/.moc
#RCC_DIR		= $$MMO_CONFIGURATION/.rcc
#UI_DIR		= $$MMO_CONFIGURATION/.ui

build_pass {
	TARGET = $$join(TARGET,,,_$$MMO_CONFIGURATION)
	#TARGET = $$join(TARGET,,,_d)
}

#TARGET = $$join(TARGET,,,_d)

# Input
PRECOMPILED_HEADER = stdafx.h
HEADERS += opengl_scene.h point3d.h model.h graphics_view.h chat_input_box.h logger_box.h signin_widget.h escmenu_widget.h inven_widget.h clientlogic_thread.h browser_widget.h
SOURCES += main.cpp model.cpp opengl_scene.cpp graphics_view.cpp chat_input_box.cpp logger_box.cpp signin_widget.cpp escmenu_widget.cpp inven_widget.cpp clientlogic_thread.cpp browser_widget.cpp

QT += opengl webkit
