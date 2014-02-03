#include "stdafx.h"
#include "graphics_view.h"
#include "opengl_scene.h"
#include "ui_browser.h"

#include "../clientlogic/clientlogic.h"
void debug_line_output_handler(const char* line);
void chat_output_handler(int id, const char* line);
void game_event_handler(int event_type);

int main(int argc, char **argv)
{
    QApplication app(argc, argv);

    QCoreApplication::setOrganizationName("poolc");
    QCoreApplication::setOrganizationDomain("poolc.org");
    QCoreApplication::setApplicationName("mmo");

	int ret_code = 0;

    mmocl_context ctx;
    cl_initialize_params params;
    params.chat_output_handler = chat_output_handler;
    params.debug_line_output_handler = debug_line_output_handler;
    params.game_event_handler = game_event_handler;
    cl_initialize_context(&ctx, &params);
    cl_bind_context(ctx);

	opengl_scene* scene = new opengl_scene(nullptr);

    graphics_view view(scene);
    view.setViewport(new QGLWidget(QGLFormat(QGL::SampleBuffers)));
    view.setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
    view.setRenderHint(QPainter::Antialiasing);
    //view.setScene(new opengl_scene(&view));
    view.resize(640, 480);
    view.show();

	ret_code = app.exec();
    
	view.setScene(nullptr);
	delete scene;
    cl_destroy_context();
	
    return ret_code;
}
