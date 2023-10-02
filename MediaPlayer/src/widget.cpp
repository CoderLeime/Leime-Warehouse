#include "widget.h"
#include "ui_widget.h"
#include <QDebug>
#include <QPainter>
#include "threadpool.h"
#include "av_player.h"
#include <QResizeEvent>
#include "vframe.h"
#include <QFileDialog>
#include <QUrl>

Q_DECLARE_METATYPE(QSharedPointer<YUV420Frame>)

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
    , m_duration(1)
    , m_seekTarget(0)
    , m_ptsSliderPressed(false)
    , m_vFmt("视频文件(*.mp4 *.mov *.avi *.mkv *.wmv *.flv *.webm *.mpeg *.mpg *.3gp *.m4v *.rmvb *.vob *.ts *.mts *.m2ts *.f4v *.divx *.xvid)")
{
    ui->setupUi(this);    
    setWindowTitle(QString("VideoPlayer"));
    ui->label_seperator->setAlignment(Qt::AlignCenter);
    ui->label_pts->setAlignment(Qt::AlignCenter);
    ui->label_duration->setAlignment(Qt::AlignCenter);
    ui->label_volume->setAlignment(Qt::AlignCenter);
    ui->lineEdit_input->setText("D:/MusicResources/Music/MV/xcz.mp4");//http://60.204.208.153/xcz.mp4

    ui->slider_AVPts->setEnabled(false);
    ui->btn_forward->setEnabled(false);
    ui->btn_back->setEnabled(false);
    ui->btn_pauseon->setEnabled(false);

    //自定义数据类型在槽中作为参数传递需先注册
    qRegisterMetaType<QSharedPointer<YUV420Frame>>("QSharedPointer<YUV420Frame>");
    m_player=new AVPlayer;
    connect(m_player,&AVPlayer::frameChanged,ui->opengl_widget,&OpenGLWidget::onShowYUV,Qt::QueuedConnection);

    connect(ui->btn_pauseon,&QPushButton::clicked,this,&Widget::pauseOnBtnClickSlot);

    connect(ui->btn_play,&QPushButton::clicked,this,[&](){
            const QString url=ui->lineEdit_input->text();
            if(url.count()) {
                if(m_player->play(url)) {
                    ui->slider_AVPts->setEnabled(true);
                    ui->btn_forward->setEnabled(true);
                    ui->btn_back->setEnabled(true);
                    ui->btn_pauseon->setEnabled(true);
                }
            }
    });

    connect(ui->btn_forward,&QPushButton::clicked,this,&Widget::seekForwardSlot);

    connect(ui->btn_back,&QPushButton::clicked,this,&Widget::seekBackSlot);

    connect(ui->slider_volume,&QSlider::valueChanged,this,&Widget::setVolume);

    connect(ui->btn_addFile,&QPushButton::clicked,this,&Widget::addFile);

    connect(m_player,&AVPlayer::AVDurationChanged,this,&Widget::durationChangedSlot);

    connect(m_player,&AVPlayer::AVPtsChanged,this,&Widget::ptsChangedSlot);

    connect(m_player,&AVPlayer::AVTerminate,this,&Widget::terminateSlot,Qt::QueuedConnection);

    connect(ui->slider_AVPts,&AVPtsSlider::sliderPressed,this,&Widget::ptsSliderPressedSlot);
    connect(ui->slider_AVPts,&AVPtsSlider::sliderMoved,this,&Widget::ptsSliderMovedSlot);
    connect(ui->slider_AVPts,&AVPtsSlider::sliderReleased,this,&Widget::ptsSliderReleaseSlot);

    connect(ui->opengl_widget,&OpenGLWidget::mouseDoubleClicked,[&](){
        if(this->isMaximized()) {
            this->showNormal();
        }
        else {
            showMaximized();
        }
    });
    connect(ui->opengl_widget,&OpenGLWidget::mouseClicked,this,&Widget::pauseOnBtnClickSlot);
}

void Widget::resizeEvent(QResizeEvent* event)
{
}

void Widget::keyReleaseEvent(QKeyEvent *event)
{
    switch (event->key()) {
        case Qt::Key_Right:
            seekForwardSlot();
            break;
        case Qt::Key_Left:
            seekBackSlot();
            break;
        case Qt::Key_Space:
            pauseOnBtnClickSlot();
            break;
        default:
            break;
    }
}

void Widget::durationChangedSlot(unsigned int duration)
{
    ui->label_duration->setText(QString("%1:%2").arg(duration/60,2,10,\
                 QLatin1Char('0')).arg(duration%60,2,10,QLatin1Char('0')));
    m_duration=duration;
}

void Widget::ptsChangedSlot(unsigned int pts)
{
    if(m_ptsSliderPressed)
        return;
    ui->slider_AVPts->setPtsPercent((double)pts/m_duration);
    ui->label_pts->setText(QString("%1:%2").arg(pts/60,2,10,\
                 QLatin1Char('0')).arg(pts%60,2,10,QLatin1Char('0')));
    //qDebug()<<"update pts"<<pts<<endl;
}

void Widget::terminateSlot()
{
    ui->label_pts->setText(QString("00:00"));
    ui->label_duration->setText(QString("00:00"));
    ui->slider_AVPts->setEnabled(false);
    ui->btn_forward->setEnabled(false);
    ui->btn_back->setEnabled(false);
    ui->btn_pauseon->setEnabled(false);
    m_player->clearPlayer();
}

void Widget::ptsSliderPressedSlot()
{
    m_ptsSliderPressed=true;
    m_seekTarget=(int)(ui->slider_AVPts->ptsPercent()*m_duration);
}

void Widget::ptsSliderMovedSlot()
{
    //qDebug()<<"ptsSlider value:"<<endl;
    m_seekTarget=(int)(ui->slider_AVPts->cursorXPercent()*m_duration);
    const QString& ptsStr=QString("%1:%2").arg(m_seekTarget/60,2,10,\
             QLatin1Char('0')).arg(m_seekTarget%60,2,10,QLatin1Char('0'));
    if(m_ptsSliderPressed)
        ui->label_pts->setText(ptsStr);
    else
        ui->slider_AVPts->setToolTip(ptsStr);
}

void Widget::ptsSliderReleaseSlot()
{
    m_player->seekTo(m_seekTarget);
    m_ptsSliderPressed=false;
}

void Widget::seekForwardSlot()
{
    m_player->seekBy(6);
    if(m_player->playState()==AVPlayer::AV_PAUSED)
        m_player->pause(false);
}

void Widget::seekBackSlot()
{
    m_player->seekBy(-6);
    if(m_player->playState()==AVPlayer::AV_PAUSED)
        m_player->pause(false);
}

void Widget::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)
    QPainter painter(this);
    painter.setBrush(QBrush(QColor(46,46,54)));
    painter.drawRect(rect());
}

Widget::~Widget()
{
    delete ui;
}

void Widget::pauseOnBtnClickSlot()
{
    switch (m_player->playState()) {
        case AVPlayer::AV_PLAYING:
            m_player->pause(true);
            ui->btn_pauseon->setText(QString("继续"));
            break;
        case AVPlayer::AV_PAUSED:
            m_player->pause(false);
            ui->btn_pauseon->setText(QString("暂停"));
            break;
        default:
            break;
    }
}

void Widget::addFile()
{
    QString url=QFileDialog::getOpenFileName(this, "选择文件", "D:/MusicResources/Music/MV", m_vFmt);
    ui->lineEdit_input->setText(url);
}

void Widget::setVolume(int volume)
{
    m_player->setVolume(volume);
    ui->label_volume->setText(QString("%1%").arg(volume));
}
