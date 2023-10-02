#pragma once
#include <QSlider>


class AVPtsSlider :public QSlider
{
	Q_OBJECT

public:
    AVPtsSlider(QWidget* parent = Q_NULLPTR);
    ~AVPtsSlider();

    void setPtsPercent(double percent);
    double ptsPercent();

    inline double cursorXPercent()
    {
        return m_cursorXPer;
    }
signals:
	void valueChanged(int percent);
	void sliderPressed();
	void sliderReleased();
	void sliderMoved();

protected:
	bool event(QEvent* e);
	void mousePressEvent(QMouseEvent* e) override;
	void mouseMoveEvent(QMouseEvent* e);
	void mouseReleaseEvent(QMouseEvent* e);
	void paintEvent(QPaintEvent* e);

private:
    bool m_isEnter;
    double m_percent;
    double m_cursorXPer;
};
