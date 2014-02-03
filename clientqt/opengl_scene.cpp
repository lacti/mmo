#include "stdafx.h"
#include "opengl_scene.h"
#include "model.h"
#include "chat_input_box.h"
#include "logger_box.h"
#include "../rendering_service/rendering_service.h"
#include "../clientlogic/clientlogic.h"
#include "signin_widget.h"
#include "escmenu_widget.h"
#include "inven_widget.h"
#include "ui_dialog.h"

typedef boost::function<void (opengl_scene*)> opengl_scene_command;

tbb::concurrent_queue<logger_line> g_logger_line_queue;
tbb::concurrent_queue<opengl_scene_command> g_scene_command_queue;

void debug_line_output_handler(const char* line)
{
    g_logger_line_queue.push(logger_line(0, QString::fromUtf8(line).trimmed()));
}

void chat_output_handler(int id, const char* line)
{
    g_logger_line_queue.push(logger_line(id, QString::fromUtf8(line)));
}


void game_event_handler(int event_type)
{
    switch (event_type)
    {
    case CGE_PLAYER_SPAWNED:
        g_scene_command_queue.push( boost::bind(&opengl_scene::show_inven, _1, true) );
        break;
    default:
        break;
    }
}

opengl_scene::opengl_scene(QWidget* view)
    : m_wireframeEnabled(false)
    , m_normalsEnabled(false)
    , m_modelColor(153, 255, 0)
    , m_backgroundColor(0, 170, 255)
    , m_model(0)
    , m_lastTime(0)
    , m_distance(1.4f)
    , m_angularMomentum(0, 40, 0)
    , m_logger_win_proxy(nullptr)
    , m_signin_win_proxy(nullptr)
    , m_escmenu_win_proxy(nullptr)
    , m_logger_win(nullptr)
    , m_signin_win(nullptr)
    , m_escmenu_win(nullptr)
    , m_inven_win_proxy(nullptr)
    , m_view(view)
    , m_frame_time(0)
    , m_accum_frame_time(0)
    , m_accum_count(0)
{
    m_logger_win = create_dialog(tr("Logger"));
    m_logger_win->layout()->setMargin(0);
    m_logger_win->layout()->setSpacing(0);

    m_logger_win->layout()->addItem(new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding));
    
    m_logger = new logger_box();
    m_logger_win->layout()->addWidget(m_logger);
    
    m_inputEdit = new chat_input_box();
    connect(m_inputEdit, SIGNAL(send_chat(const QString&)), this, SLOT(display_player_chat(const QString&)));
    m_logger_win->layout()->addWidget(m_inputEdit);

    {
        auto widget = new signin_widget(nullptr);
        
        connect(widget, SIGNAL(sign_in(const QString&, const QString&)),
            this, SLOT(sign_in(const QString&, const QString&)));

        m_signin_win = widget;
    }

    {
        m_escmenu_win = new escmenu_widget(nullptr);
        
        connect(m_escmenu_win, SIGNAL(exit_requested()),
            this, SLOT(prepare_exit()));
    }

    {
        m_signin_win_proxy = addWidget(m_signin_win, 0 /*Qt::Dialog*/);
    }

    {
        m_logger_win_proxy = addWidget(m_logger_win);

        m_logger_win_proxy->setVisible(false);
    }

    {
        m_escmenu_win_proxy = addWidget(m_escmenu_win, 0 /*Qt::Dialog*/);

        m_escmenu_win_proxy->setVisible(false);

        auto signout_button = m_escmenu_win->findChild<QPushButton*>("signout_button");
        signout_button->setFocus();
    }

    {
        auto win = new inven_widget(nullptr);

        m_inven_win_proxy = addWidget(win, 0);
        m_inven_win_proxy->setPos(0, 0);
        m_inven_win_proxy->setVisible(false);
    }

    {
        auto dialog_win = create_dialog(tr("Dialog"));
        auto dialog = new QDialog();
        auto dialog_scaffold = new Ui::Dialog;
        
        dialog_scaffold->setupUi(dialog);
        dialog_win->layout()->addWidget(dialog);
        
        auto w = addWidget(dialog_win);
        w->setPos(0, 200);
        w->resize(500, 90);
        w->setVisible(false);
    }

    {
        auto debug_win = create_dialog(tr("Debugger"));
        m_frame_time_label = new QLabel("Frame Rate Shown Here");
        debug_win->layout()->addWidget(m_frame_time_label);
        auto w = addWidget(debug_win);
        w->setPos(0, 100);
    }

    setActiveWindow(m_signin_win_proxy);
    setFocusItem(m_signin_win_proxy);
    
    QPointF pos(0, 0);
    foreach (QGraphicsItem *item, items()) {
        item->setFlag(QGraphicsItem::ItemIsMovable);
        item->setCacheMode(QGraphicsItem::DeviceCoordinateCache);
    }

    m_time.start();

    connect(this, SIGNAL(sceneRectChanged(const QRectF&)), this, SLOT(resize_ui_layer(const QRectF&)));
}

opengl_scene::~opengl_scene()
{
    if (rs_is_initialized())
    {
        rs_destroy();
    }
}

void opengl_scene::create_chat_bubble(const logger_line& line, const QPointF& pos, double lifetime)
{
    auto bubble = create_dialog(tr("Chat_bubble"));
    bubble->layout()->setMargin(0);
    bubble->setFont(QFont("Malgun Gothic", 10));
    bubble->setStyleSheet("background-color: qlineargradient(spread:pad, x1:1, y1:0, x2:1, y2:1, stop:0 rgba(100, 77, 210, 255), stop:1 rgba(53, 41, 59, 255));color: rgb(255, 255, 255); border-radius:4px");
    auto chat_box_label = new QLabel(line.second);
    bubble->layout()->addWidget(chat_box_label);
    
    auto bubble_widget = addWidget(bubble);
    bubble_widget->setPos(pos);
    //chat_box_widget->setOpacity(0.8);
    //chat_box_widget->setVisible(false);
    //chat_box_widget->close();

    m_chat_bubbles.push_back(chat_bubble(bubble_widget, lifetime, line.first));
}

void opengl_scene::prepare_exit()
{
    QApplication::exit();
}

void opengl_scene::sign_in(const QString& server, const QString& name)
{
    m_signin_win_proxy->setVisible(false);
    m_signin_win_proxy->setEnabled(false);

    m_logger_win_proxy->setVisible(true);

    cl_connect_to_server(server.toUtf8());

    cl_enter_world(name.toUtf8());

    cl_start_network_thread();
}

void opengl_scene::resize_ui_layer(const QRectF& rect)
{
    if (m_logger_win)
    {
        //m_logger_win->resize(static_cast<int>(rect.width()), static_cast<int>(rect.height()));

        auto p = rect.center() - m_logger_win->rect().center();
        m_logger_win->move( static_cast<int>(p.x()), static_cast<int>(p.y() + rect.height()/2 - m_logger_win->height()/2) );
    }

    if (m_signin_win)
    {
        //m_signin_win->resize(static_cast<int>(rect.width()), static_cast<int>(rect.height()));

        auto p = rect.center() - m_signin_win->rect().center();

        int rh = rect.height();
        int h = m_signin_win->height();
        
        m_signin_win->move( static_cast<int>(p.x()), static_cast<int>(p.y() + rh/2 - h/2) );
    }

    if (m_escmenu_win)
    {
        //m_escmenu_win->resize(static_cast<int>(rect.width()), static_cast<int>(rect.height()));

        auto p = rect.center() - m_escmenu_win->rect().center();
        m_escmenu_win->move( static_cast<int>(p.x()), static_cast<int>(p.y()) );
    }
}

QWidget *opengl_scene::create_dialog(const QString& windowTitle) const
{
    auto dialog = new QWidget(nullptr, Qt::FramelessWindowHint);

    dialog->setWindowOpacity(1.0);
    dialog->setWindowTitle(windowTitle);
    dialog->setLayout(new QVBoxLayout);
    dialog->setAttribute(Qt::WA_TranslucentBackground);
    
    return dialog;
}

void opengl_scene::display_player_chat(const QString& line)
{
    if (cl_is_player_valid())
    {
        cl_player_chat(line.toUtf8());
    }

    // 서버 안 떠 있을 때는 아래처럼 해야되긴 하는데...
    /*
    if (m_logger)
    {
        const char* n = cl_player_name();

        m_logger->add_chat(n, line);
    }
    */
}

void opengl_scene::display_queued_message(const QString& line)
{
    if (m_logger)
    {
        m_logger->add_message(line);
    }
}

QPointF opengl_scene::screen_point_from_world(double x, double y)
{
    QPointF p(x + sceneRect().width() / 2, -y + sceneRect().height() / 2);

    return p;
}

void opengl_scene::drawBackground(QPainter *painter, const QRectF &)
{
    ++m_accum_count;
    m_accum_frame_time += m_frame_time;
    
    if (m_accum_count % 128 == 0)
    {
        m_accum_frame_time /= m_accum_count;
        m_accum_count = 1;
    }

    QElapsedTimer et;
    et.start();

#if !CLIENTQT_MULTITHREADED
    cl_frame_move(m_frame_time);
#endif

    render_frame(painter, m_frame_time);

    // 렌더링 커맨드를 하나 처리한다. (아껴야지)
    opengl_scene_command scene_command;
    if (g_scene_command_queue.try_pop(scene_command))
    {
        scene_command(this);
    }

    m_frame_time = static_cast<decltype(m_frame_time)>(et.nsecsElapsed()) / 1e9;
}

void opengl_scene::render_frame(QPainter *painter, const double /*delta_time*/)
{
    if (painter->paintEngine()->type() != QPaintEngine::OpenGL
        && painter->paintEngine()->type() != QPaintEngine::OpenGL2)
    {
        qWarning("opengl_scene: drawBackground needs a QGLWidget to be set as viewport on the graphics view");
        return;
    }

    QString ft_str;
    ft_str.sprintf("<font color='white'>%.2f ms</font>", m_accum_frame_time / m_accum_count * 1e3);
    m_frame_time_label->setText(ft_str);

    static const double tile_size = 256.0/8;

    if (!rs_is_initialized())
    {
        rs_initialize();
        rs_set_debug_output_handler(debug_line_output_handler);

        rs_register_shader_program(MMORES("default.vert"), MMORES("default.frag"));

        int sprite_count = cl_character_sprite_count();
        for (int i = 0; i < sprite_count; ++i)
        {
        }

        rs_register_texture(1, MMORES("001-Fighter01.png"));
        rs_register_texture(2, MMORES("grassland01bg.png"));
        rs_register_texture(3, MMORES("particle0.png"));
        rs_register_texture(4, MMORES("001-Grassland01.png"));
        rs_register_texture(5, MMORES("018-Thief03.png"));
        rs_register_texture(6, MMORES("023-Gunner01.png"));
        rs_register_texture(7, MMORES("fakeshadow.png"));

        double collision_r = 8.0;
        
        rs_register_vertex_buffer_rectangle("vb_user", 128 / 4, 192 / 4 );
        rs_register_vertex_buffer_cube("vb_tile", tile_size, tile_size, 32 );
        rs_register_vertex_buffer_rectangle("vb_fakeshadow", 22, 14 );
        rs_register_vertex_buffer_rectangle("vb_2", tile_size, tile_size );
        rs_register_vertex_buffer_rectangle("vb_3", 32*5, 32*5 );
        rs_register_vertex_buffer_rectangle("vb_4", 32*4, 32*5 );
        rs_register_vertex_buffer_rectangle("vb_circle", collision_r, 32);
    }


    rs_projection(static_cast<int>(width()), static_cast<int>(height()));
    rs_lookat(0.0, 0.0);

    painter->beginNativePainting();

    rs_clear_screen();

    rs_render_tile_params params;
    memset(&params, 0, sizeof(rs_render_tile_params));
    params.resource_name = 2;
    params.xrepeat = 1;
    params.yrepeat = 1;
    params.uindex = 0;
    params.vindex = 0;

    double phase = GetTickCount()/1000.0;
    for (int i = 0; i < 20; ++i)
    {
        for (int j = 0; j < 15; ++j)
        {
            params.x = i * tile_size;
            params.y = j * tile_size;
            params.z = 30*sin((i+j)/30.0*(2*M_PI) + phase);
            rs_render_tile(&params);
        }
    }

    // 캐릭터를 그리자~ 그리자~
    if (int ve_count = cl_visible_entity_count())
    {
        QVector<cl_visible_entity> ve(ve_count);

        const int ve_real_count = cl_visible_entities(&ve[0], ve.size());

        ve.resize(ve_real_count);

        std::sort(ve.begin(), ve.end(), [](const cl_visible_entity& e1, const cl_visible_entity& e2) -> bool {
            return e1.y > e2.y;
        });

        
        glDisable(GL_DEPTH_TEST);
        std::for_each(ve.begin(), ve.end(), [phase](const cl_visible_entity& e) {

            int tile_index_x = (e.x - tile_size/2) / tile_size;
            int tile_index_y = (e.y - tile_size/2) / tile_size;

            float zoffset = 30*sin((tile_index_x+tile_index_y)/30.0*(2*M_PI) + phase);

            
            // 그! 림! 자!
            rs_render_simple_image(e.x, e.y, zoffset + 5, "vb_fakeshadow", 7);

            // 캐! 릭! 터!
            rs_render_image(e.x, e.y, zoffset + e.z, "vb_user", e.resource_name, e.uindex, e.vindex);

            
        });

        
    }

    rs_clear_context();

    painter->endNativePainting();

    cl_frame_move(0.017);

    logger_line queued_line;
    while (g_logger_line_queue.try_pop(queued_line))
    {
        QString msg;

        if (queued_line.first > 0)
        {
            QString speaker = QString::fromUtf8( cl_entity_name(queued_line.first) );

            msg = QString("[%1] : %2").arg(speaker).arg(queued_line.second);
        }
        else
        {
            msg = queued_line.second;
        }
        
        display_queued_message(msg);

        if (cl_is_player_valid() && queued_line.first != 0)
        {
            double x = 0, y = 0;
            cl_entity_pos(&x, &y, queued_line.first);
            create_chat_bubble(queued_line, screen_point_from_world(x, y), 3.0);
        }

        // 시작 메뉴에 깜빡거리게 하는 기능인데, 지금은 영 귀찮다.
        // QApplication::alert(m_view, 0);
    }

    std::for_each(m_chat_bubbles.begin(), m_chat_bubbles.end(), [&](chat_bubble& i)
    {
        double x = 0, y = 0;
        cl_entity_pos(&x, &y, i.id);
        
        QPointF world_pos = screen_point_from_world(x, y);
        world_pos.setX( world_pos.x() - i.first->rect().width()/2 ); // 가운데 정렬
        world_pos.setY( world_pos.y() - 40 ); // 캐릭터 머리 위에 보이도록
        i.first->setPos(world_pos);

        i.second -= 0.02;
    });

    m_chat_bubbles.erase(std::remove_if(m_chat_bubbles.begin(), m_chat_bubbles.end(), [](chat_bubble& i) -> bool
    {
        if (i.second <= 0)
        {
            i.first->close();
            return true;
        }

        return false;
    }), m_chat_bubbles.end());

    QTimer::singleShot(0, this, SLOT(update()));
}

static Model *loadModel(const QString &filePath)
{
    return new Model(filePath);
}

void opengl_scene::loadModel()
{
    loadModel(QFileDialog::getOpenFileName(0, tr("Choose model"), QString(), QLatin1String("*.obj")));
}

void opengl_scene::loadModel(const QString &filePath)
{
    if (filePath.isEmpty())
        return;

    m_modelButton->setEnabled(false);
    QApplication::setOverrideCursor(Qt::BusyCursor);
    setModel(::loadModel(filePath));
    modelLoaded();
}

void opengl_scene::modelLoaded()
{
    m_modelButton->setEnabled(true);
    QApplication::restoreOverrideCursor();
}

void opengl_scene::setModel(Model *model)
{
    delete m_model;
    m_model = model;

    m_labels[0]->setText(tr("File: %0").arg(m_model->fileName()));
    m_labels[1]->setText(tr("Points: %0").arg(m_model->points()));
    m_labels[2]->setText(tr("Edges: %0").arg(m_model->edges()));
    m_labels[3]->setText(tr("Faces: %0").arg(m_model->faces()));

    update();
}

void opengl_scene::enableWireframe(bool enabled)
{
    m_wireframeEnabled = enabled;
    update();
}

void opengl_scene::enableNormals(bool enabled)
{
    m_normalsEnabled = enabled;
    update();
}

void opengl_scene::setModelColor()
{
    const QColor color = QColorDialog::getColor(m_modelColor);
    if (color.isValid()) {
        m_modelColor = color;
        update();
    }
}

void opengl_scene::setBackgroundColor()
{
    const QColor color = QColorDialog::getColor(m_backgroundColor);
    if (color.isValid()) {
        m_backgroundColor = color;
        update();
    }
}

void opengl_scene::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    QGraphicsScene::mouseMoveEvent(event);
    if (event->isAccepted())
        return;
    if (event->buttons() & Qt::LeftButton) {
        const QPointF delta = event->scenePos() - event->lastScenePos();
        const Point3d angularImpulse = Point3d(static_cast<float>(delta.y()), static_cast<float>(delta.x()), 0.0f) * 0.1f;

        m_rotation += angularImpulse;
        m_accumulatedMomentum += angularImpulse;

        event->accept();
        update();
    }
}

void opengl_scene::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    QGraphicsScene::mousePressEvent(event);
    if (event->isAccepted())
        return;

    m_mouseEventTime = m_time.elapsed();
    m_angularMomentum = m_accumulatedMomentum = Point3d();
    event->accept();
}

void opengl_scene::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    QGraphicsScene::mouseReleaseEvent(event);
    if (event->isAccepted())
        return;

    const int delta = m_time.elapsed() - m_mouseEventTime;
    m_angularMomentum = m_accumulatedMomentum * (1000.0 / qMax(1, delta));
    event->accept();
    update();
}

void opengl_scene::wheelEvent(QGraphicsSceneWheelEvent *event)
{
    QGraphicsScene::wheelEvent(event);
    if (event->isAccepted())
        return;

    m_distance *= qPow(1.2, -event->delta() / 120);
    event->accept();
    update();
}

static bool g_key_press_state[4]; // left, right, up, down
static int g_last_pressed_key = -1;

void update_key_press_state()
{
    int id = cl_player_id();

    if (id <= 0)
        return;

    if (g_last_pressed_key >= 0)
    {
        switch (g_last_pressed_key)
        {
        case 0: cl_entity_command(CAC_ENTITY_MOVE_LEFT, id); break;
        case 1: cl_entity_command(CAC_ENTITY_MOVE_RIGHT, id); break;
        case 2: cl_entity_command(CAC_ENTITY_MOVE_UP, id); break;
        case 3: cl_entity_command(CAC_ENTITY_MOVE_DOWN, id); break;
        }
    }
    else if (g_key_press_state[0])
    {
        cl_entity_command(CAC_ENTITY_MOVE_LEFT, id);
    }
    else if (g_key_press_state[1])
    {
        cl_entity_command(CAC_ENTITY_MOVE_RIGHT, id);
    }
    else if (g_key_press_state[2])
    {
        cl_entity_command(CAC_ENTITY_MOVE_UP, id);
    }
    else if (g_key_press_state[3])
    {
        cl_entity_command(CAC_ENTITY_MOVE_DOWN, id);
    }
    else
    {
        cl_entity_command(CAC_ENTITY_MOVE_STOP, id);
    }
}

void opengl_scene::keyPressEvent(QKeyEvent *event)
{
    if (m_inputEdit && m_inputEdit->isVisible() && event->key() == Qt::Key_Return && !event->isAutoRepeat())
    {
        bool updated = false;

        if (m_inputEdit->isReadOnly() && !m_inputEdit->isEnabled())
        {
            m_inputEdit->setEnabled(true);
            m_inputEdit->setReadOnly(false);
            updated = true;
        }

        if (!m_inputEdit->hasFocus())
        {
            m_inputEdit->setFocus();
        }

        if (updated)
        {
            return;
        }
    }


    QGraphicsScene::keyPressEvent(event);
    if (event->isAccepted())
        return;

    if (!event->isAutoRepeat())
    {
        switch (event->key())
        {
        case Qt::Key_Left:  g_key_press_state[0] = true; g_last_pressed_key = 0; update_key_press_state(); event->accept(); break;
        case Qt::Key_Right: g_key_press_state[1] = true; g_last_pressed_key = 1; update_key_press_state(); event->accept(); break;
        case Qt::Key_Up:    g_key_press_state[2] = true; g_last_pressed_key = 2; update_key_press_state(); event->accept(); break;
        case Qt::Key_Down:  g_key_press_state[3] = true; g_last_pressed_key = 3; update_key_press_state(); event->accept(); break;
        case Qt::Key_D:     cl_interact(); break;
        }
    }

    if (m_escmenu_win_proxy && event->key() == Qt::Key_Escape)
    {
        setActiveWindow(m_escmenu_win_proxy);
        
        //setFocusItem(m_escmenu_win_proxy);

        m_escmenu_win_proxy->setVisible(true);
        m_escmenu_win_proxy->setFocus();
        //m_escmenu_win->setFocus();

        
    }
}

void opengl_scene::keyReleaseEvent(QKeyEvent *event)
{
    QGraphicsScene::keyPressEvent(event);
    if (event->isAccepted())
        return;

    if (!event->isAutoRepeat())
    {
        switch (event->key())
        {
        case Qt::Key_Left:  g_key_press_state[0] = false; g_last_pressed_key = -1; update_key_press_state(); event->accept(); break;
        case Qt::Key_Right: g_key_press_state[1] = false; g_last_pressed_key = -1; update_key_press_state(); event->accept(); break;
        case Qt::Key_Up:    g_key_press_state[2] = false; g_last_pressed_key = -1; update_key_press_state(); event->accept(); break;
        case Qt::Key_Down:  g_key_press_state[3] = false; g_last_pressed_key = -1; update_key_press_state(); event->accept(); break;
        }

        
    }
}
