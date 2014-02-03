#pragma once

class browser_widget : public QWidget
{
    Q_OBJECT

public:
	explicit browser_widget(QWidget* parent) : QWidget(parent), scene(nullptr), painter(nullptr), image(nullptr)
	{
	}

	~browser_widget()
	{
	}

	void set_scene_painter_image(QGraphicsScene* in_scene, QPainter* in_painter, QImage* in_image)
	{
		scene = in_scene;
		painter = in_painter;
		image = in_image;
	}

private slots:
    void web_view_load_finished(bool b);

private:
	QGraphicsScene* scene;
	QPainter* painter;
	QImage* image;
};
