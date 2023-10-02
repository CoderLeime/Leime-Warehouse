#ifndef AV_PLAYER_H
#define AV_PLAYER_H

#include "av_decoder.h"
#include <QObject>
#include <QSharedPointer>
#include <QSize>
#include "av_clock.h"

extern "C"
{
#include <libswscale/swscale.h>
#include <libavdevice/avdevice.h>
#include <libswresample/swresample.h>
#include <SDL.h>
#include <libavutil/imgutils.h>
}

using AVTool::Decoder;

class YUV420Frame;

typedef Decoder::MyFrame MyFrame;

class AVPlayer:public QObject
{
    Q_OBJECT

    friend void fillAStreamCallback(void* userdata, uint8_t* stream,int len);
public:
    enum PlayState
    {
        AV_STOPPED,
        AV_PLAYING,
        AV_PAUSED
    };

    AVPlayer();
    ~AVPlayer();

    int play(const QString& url);

    void pause(bool isPause);

    void clearPlayer();

    AVPlayer::PlayState playState();

    void setVFrameSize(const QSize& size)
    {
        m_imageWidth=size.width();
        m_imageHeight=size.height();
    }

    inline void setVolume(int volumePer)
    {
        m_volume=(volumePer*SDL_MIX_MAXVOLUME/100)%(SDL_MIX_MAXVOLUME+1);
    }

    inline void seekBy(int32_t time_s)
    {
        seekTo((int32_t)m_audioClock.getClock()+time_s);
    }

    inline void seekTo(int32_t time_s)
    {
        if(time_s<0)
            time_s=0;
        m_decoder->seekTo(time_s,time_s-(int32_t)m_audioClock.getClock());
    }

    inline uint32_t avDuration()
    {
        return m_duration;
    }

signals:
    void frameChanged(QSharedPointer<YUV420Frame> frame);
    void AVTerminate();
    void AVPtsChanged(unsigned int pts);
    void AVDurationChanged(unsigned int duration);

private:
    int initSDL();
    int initVideo();
    void videoCallback(std::shared_ptr<void> par);
    double computeTargetDelay(double delay);
    double vpDuration(MyFrame* lastFrame, MyFrame* curFrame);
    void displayImage(AVFrame* frame);
    void initAVClock();

private:
    //解码器实例
    Decoder* m_decoder;
    AVFormatContext* m_fmtCtx;

    AVCodecParameters* m_audioCodecPar;
    SwrContext* m_swrCtx;
    uint8_t* m_audioBuf;

    uint32_t m_audioBufSize;
    uint32_t m_audioBufIndex;
    uint32_t m_duration;

    uint32_t m_lastAudPts;

    enum AVSampleFormat m_targetSampleFmt;

    //记录音视频帧最新播放帧的时间戳，用于同步
//    double m_audioPts;
//    double m_videoPts;

    double m_frameTimer;

    //延时时间
    double m_delay;

    //音频播放时钟
    AVClock m_audioClock;

    //视频播放时钟
    AVClock m_videoClock;

    int m_targetChannels;
    int m_targetFreq;
    int m_targetChannelLayout; 
    int m_targetNbSamples;

    int m_volume;

    //同步时钟初始化标志，音视频异步线程
    //谁先读到初始标志位就由谁初始化时钟
    int m_clockInitFlag;

    int m_audioIndex;
    int m_videoIndex;

    int m_imageWidth;
    int m_imageHeight;

    //终止标志
    int m_exit;

    //记录暂停前的时间
    double m_pauseTime;

    //暂停标志
    int m_pause;

    AVFrame* m_audioFrame;

    AVCodecParameters* m_videoCodecPar;

    enum AVPixelFormat m_dstPixFmt;
    int m_swsFlags;
    SwsContext* m_swsCtx;

    uint8_t* m_buffer;

    uint8_t* m_pixels[4];
    int m_pitch[4];
};

#endif
