#ifndef OPENGL_WIDGET_H
#define OPENGL_WIDGET_H
#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QOpenGLBuffer>
#include <QTimer>
#include <memory>

class YUV420Frame;

QT_FORWARD_DECLARE_CLASS(QOpenGLShaderProgram)
QT_FORWARD_DECLARE_CLASS(QOpenGLTexture)

class OpenGLWidget : public QOpenGLWidget,protected QOpenGLFunctions
{
    Q_OBJECT

public:
    explicit OpenGLWidget(QWidget *parent = 0);
    ~OpenGLWidget();

public slots:
    //void slotShowYuv(uchar *ptr,uint width,uint height); //显示一帧Yuv图像
    void onShowYUV(QSharedPointer<YUV420Frame> frame);

protected:
    void initializeGL() Q_DECL_OVERRIDE;
    void paintGL() Q_DECL_OVERRIDE;

private:
    QOpenGLShaderProgram *program;
    QOpenGLBuffer vbo;

    //opengl中y、u、v分量位置
    GLuint posUniformY,posUniformU,posUniformV;

    //纹理
    QOpenGLTexture *textureY = nullptr,*textureU = nullptr,*textureV = nullptr;

    //纹理ID，创建错误返回0
    GLuint m_idY,m_idU,m_idV;

    //像素分辨率

    //原始数据
    QSharedPointer<YUV420Frame> m_frame;
};


#endif
