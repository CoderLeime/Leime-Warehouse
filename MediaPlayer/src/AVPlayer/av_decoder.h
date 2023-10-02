#ifndef AV_DECODER_H
#define AV_DECODER_H

#include <QVector>
#include <mutex>
#include <condition_variable>

extern "C"
{
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
}

QT_BEGIN_NAMESPACE
namespace AVTool
{

class Decoder
{
    typedef struct PacketQueue
    {
        QVector<AVPacket> pktVec;
        int readIndex;
        int pushIndex;
        int size;
        std::mutex mutex;
        std::condition_variable cond;
    }PacketQueue;

    typedef struct FrameQueue
    {
        QVector<AVFrame> frameVec;
        int readIndex;
        int pushIndex;
        int shown;
        int size;
        std::mutex mutex;
        std::condition_variable cond;
    }FrameQueue;

public:
    Decoder();
    ~Decoder();

    void seekTo(int32_t target,int32_t seekRel);

    int getAFrame(AVFrame* frame);

    int getRemainingVFrame();
    //查看上一帧（即当前显示的画面帧）
    AVFrame* peekLastVFrame();
    //查看将要显示的帧
    AVFrame* peekVFrame();
    //查看将要显示帧再下一帧
    AVFrame* peekNextVFrame();
    //将读索引后移一位
    void setNextVFrame();

    inline int audioIndex() const
    {
        return m_audioIndex;
    }

    inline int videoIndex() const
    {
        return m_videoIndex;
    }

    inline AVFormatContext* formatContext() const
    {
        return m_fmtCtx;
    }

    inline AVCodecParameters* audioCodecPar() const
    {
        return m_audioCodecPar;
    }

    inline AVCodecParameters* videoCodecPar() const
    {
        return m_videoCodecPar;
    }

    inline uint32_t avDuration()
    {
        return m_duration;
    }

    inline int isExit()
    {
        return m_exit;
    }

    int decode(const QString& url);

    void exit();

private:
    PacketQueue m_audioPacketQueue;
    PacketQueue m_videoPacketQueue;

    FrameQueue m_audioFrameQueue;
    FrameQueue m_videoFrameQueue;

    AVFormatContext* m_fmtCtx;

    AVCodecParameters* m_audioCodecPar;
    AVCodecParameters* m_videoCodecPar;

    AVCodecContext* m_audioCodecCtx;
    AVCodecContext* m_videoCodecCtx;

    const int m_maxFrameQueueSize;
    const int m_maxPacketQueueSize;

    AVRational m_vidFrameRate;

    int m_audioIndex;
    int m_videoIndex;

    int m_exit;

    //是否执行跳转
    int m_isSeek;

    //跳转后等待目标帧标志
    int m_vidSeek;
    int m_audSeek;

    //跳转相对时间
    int64_t m_seekRel;

    //跳转绝对时间
    int64_t m_seekTarget;

    //流总时长/S
    uint32_t m_duration;

    char m_errBuf[100];

private:
    bool init();

    void setInitVal();

    void clearQueue();

    void demux(std::shared_ptr<void> par);

    void audioDecode(std::shared_ptr<void> par);
    void videoDecode(std::shared_ptr<void> par);

    int getPacket(PacketQueue* queue,AVPacket* pkt);
    void pushPacket(PacketQueue* queue,AVPacket* pkt);

    void pushAFrame(AVFrame* frame);

    void pushVFrame(AVFrame* frame);
};

}
QT_END_NAMESPACE

#endif // AV_DECODER_H
