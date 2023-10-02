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

signals:
    void mouseClicked();
    void mouseDoubleClicked();

public slots:
    void onShowYUV(QSharedPointer<YUV420Frame> frame);

protected:
    void initializeGL() Q_DECL_OVERRIDE;
    void paintGL() Q_DECL_OVERRIDE;
    virtual void mouseReleaseEvent(QMouseEvent *event);
    virtual void mouseDoubleClickEvent(QMouseEvent *event);

private:
    QOpenGLShaderProgram *program;
    QOpenGLBuffer vbo;

    //opengl中y、u、v分量位置
    GLuint posUniformY,posUniformU,posUniformV;

    //纹理
    QOpenGLTexture *textureY = nullptr,*textureU = nullptr,*textureV = nullptr;

    //纹理ID，创建错误返回0
    GLuint m_idY,m_idU,m_idV;

    //原始数据
    QSharedPointer<YUV420Frame> m_frame;

    int m_isDoubleClick;

    QTimer m_timer;
};


#endif
