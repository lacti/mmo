#pragma once

#include "point3d.h"

class Model;
class chat_input_box;
class logger_box;

typedef QPair<int, QString> logger_line;

class opengl_scene : public QGraphicsScene
{
    Q_OBJECT

public:
    explicit opengl_scene(QWidget* view);
    virtual ~opengl_scene();

    void drawBackground(QPainter* painter, const QRectF& rect);
    void render_frame(QPainter* painter, const double delta_time);
    void show_inven(bool b)
    {
        if (m_inven_win_proxy)
            m_inven_win_proxy->setVisible(b);
    }

public slots:
    void enableWireframe(bool enabled);
    void enableNormals(bool enabled);
    void setModelColor();
    void setBackgroundColor();
    void loadModel();
    void loadModel(const QString& filePath);
    void modelLoaded();
    void display_player_chat(const QString& line);
    void display_queued_message(const QString& line);
    void resize_ui_layer(const QRectF& rect);
    void sign_in(const QString& server, const QString& name);
    void prepare_exit();

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent* event);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent* event);
    void mouseMoveEvent(QGraphicsSceneMouseEvent* event);
    void wheelEvent(QGraphicsSceneWheelEvent* wheelEvent);
    void keyPressEvent(QKeyEvent* event);
    void keyReleaseEvent(QKeyEvent* event);

private:
    QWidget*    create_dialog(const QString& windowTitle) const;
    void        create_chat_bubble(const logger_line& line, const QPointF& pos, double lifetime);
    QPointF     screen_point_from_world(double x, double y);
    void        setModel(Model* model);

    bool                        m_wireframeEnabled;
    bool                        m_normalsEnabled;
    QColor                      m_modelColor;
    QColor                      m_backgroundColor;
    Model*                      m_model;
    QTime                       m_time;
    int                         m_lastTime;
    int                         m_mouseEventTime;
    float                       m_distance;
    Point3d                     m_rotation;
    Point3d                     m_angularMomentum;
    Point3d                     m_accumulatedMomentum;
    QLabel*                     m_labels[4];
    QWidget*                    m_modelButton;
    QWidget*                    m_controls;
    chat_input_box*             m_inputEdit;
    QGraphicsProxyWidget*       m_proxyWidget;
    logger_box*                 m_logger;
    QWidget*                    m_logger_win;
    QWidget*                    m_signin_win;
    QWidget*                    m_escmenu_win;
    QGraphicsProxyWidget*       m_logger_win_proxy;
    QGraphicsProxyWidget*       m_signin_win_proxy;
    QGraphicsProxyWidget*       m_escmenu_win_proxy;
    QGraphicsProxyWidget*       m_inven_win_proxy;
    QGraphicsRectItem*          m_lightItem;

    struct chat_bubble
    {
        chat_bubble()
            : first(nullptr)
            , second(0)
            , id(0)
        {
        }

        chat_bubble(QGraphicsProxyWidget* w, double lifetime, int entity_id)
            : first(w)
            , second(lifetime)
            , id(entity_id)
        {
        }

        QGraphicsProxyWidget* first;
        double second;
        int id;
    };

    QVector<chat_bubble>             m_chat_bubbles;
    QWidget*                         m_view;
    double                           m_frame_time;
    double                           m_accum_frame_time;
    int                              m_accum_count;
    QLabel*                          m_frame_time_label;

};
