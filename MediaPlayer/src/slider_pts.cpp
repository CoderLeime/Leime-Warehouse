#include "slider_pts.h"
#include <QMouseEvent>
#include <QPainter>
#include <QDebug>


AVPtsSlider::AVPtsSlider(QWidget* parent) :
		QSlider(parent),
        m_isEnter(false),
        m_percent(0.00)
{
    setMouseTracking(true);
}

AVPtsSlider::~AVPtsSlider()
{	
}

void AVPtsSlider::setPtsPercent(double percent)
{
	if (percent < 0.0 || percent>100.0)
		return;
    m_percent=percent;
    int value = (int)(percent * this->width());
	setValue(value);
}

double AVPtsSlider::ptsPercent()
{
	double percent = 0.0;
	percent = (double)this->value() / this->width();
	return percent;
}

bool AVPtsSlider::event(QEvent* e)
{
	if (e->type() == QEvent::Enter) {
        setStyleSheet("QSlider::sub-page:horizontal#slider_AVPts{"
                        "background:rgb(255,92,56);"
                        "margin-top:4px;"
                        "margin-bottom:4px;}"
                        "QSlider::add-page:horizontal#slider_AVPts{"
                        "background:rgb(83,83,83);"
                        "margin-top:4px;"
                        "margin-bottom:4px;}");
	}
	else if (e->type() == QEvent::Leave) {
        setStyleSheet("QSlider::sub-page:horizontal#slider_AVPts{"
                        "background:rgb(255,92,56);"
                        "margin-top:7px;"
                        "margin-bottom:7px;}"
                        "QSlider::add-page:horizontal#slider_AVPts{"
                        "background:rgb(83,83,83);"
                        "margin-top:7px;"
                        "margin-bottom:7px;}");
	}
    else if(e->type()==QEvent::Resize) {
        setMaximum(this->width()-1);
        setPtsPercent(m_percent);
        m_percent=ptsPercent();
    }
	return QSlider::event(e);
}

void AVPtsSlider::mousePressEvent(QMouseEvent* e)
{
	setValue(e->pos().x());  
	emit sliderPressed();
	m_isEnter = true;
}

void AVPtsSlider::mouseMoveEvent(QMouseEvent* e)
{
    int posX=e->pos().x();
    if(posX>width())
        posX=width();
    if(posX<0)
        posX=0;
    m_cursorXPer=(double)(posX)/width();
	if (m_isEnter) {
        setValue(posX);
	}
    emit sliderMoved();
}

void AVPtsSlider::mouseReleaseEvent(QMouseEvent* e)
{
    m_percent=ptsPercent();
	emit sliderReleased();
	m_isEnter = false;
}

void AVPtsSlider::paintEvent(QPaintEvent* e)
{
	return  QSlider::paintEvent(e);
}
