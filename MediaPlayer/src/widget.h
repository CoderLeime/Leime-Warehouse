#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>

QT_BEGIN_NAMESPACE
namespace Ui { class Widget; }
QT_END_NAMESPACE

extern "C"
{
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
#include <libavdevice/avdevice.h>
#include <libavformat/version.h>
}

#include <memory>
#include <mutex>

class AVPlayer;

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();

protected:
    virtual void paintEvent(QPaintEvent *event) override;
    virtual void resizeEvent(QResizeEvent* event) override;
    virtual void keyReleaseEvent(QKeyEvent *event) override;

private:
    Ui::Widget *ui;

    AVPlayer* m_player;

    const QString m_vFmt;

    unsigned int m_duration;

    bool m_ptsSliderPressed;

    int m_seekTarget;
private Q_SLOTS:
    void pauseOnBtnClickSlot();

    void addFile();

    void setVolume(int volume);

    void ptsChangedSlot(unsigned int duration);

    void durationChangedSlot(unsigned int pts);

    void terminateSlot();

    void ptsSliderPressedSlot();

    void ptsSliderMovedSlot();

    void ptsSliderReleaseSlot();

    void seekForwardSlot();

    void seekBackSlot();
};
#endif // WIDGET_H
