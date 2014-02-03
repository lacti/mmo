#pragma once

#include <QtGui>
#include <QtOpenGL>
#include <QGLWidget>
#include <QFile>
#include <QTextStream>
#include <QVarLengthArray>
#include <QGraphicsScene>
#include <QLabel>
#include <QTime>
#include <QLineEdit>
#include <QtCore/QVariant>
#include <QtWebKit/QWebView>

#ifdef WIN32
#include <gl/GLU.h>
#endif

#ifndef GL_MULTISAMPLE
#define GL_MULTISAMPLE  0x809D
#endif

#include <boost/thread.hpp>
#include <boost/function.hpp>
#include <tbb/concurrent_queue.h>
#include "../mmobase/mmobase.h"

typedef std::function<int ()> clt_async_command_t;

#define CLIENTQT_MULTITHREADED 0
