#ifndef EXTERNALWINDOW_H
#define EXTERNALWINDOW_H

#include <QObject>
#include <QProcess>
#include <QRect>
#include "WindowManager.h"
class WindowManager;

class ExternalWindow : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool visible READ visible WRITE setVisible NOTIFY visibleChanged)
    Q_PROPERTY(QRect geometry READ geometry WRITE setGeometry NOTIFY geometryChanged)
    Q_PROPERTY(int x READ x WRITE setX NOTIFY xChanged)
    Q_PROPERTY(int y READ y WRITE setY NOTIFY yChanged)
    Q_PROPERTY(int width READ width WRITE setWidth NOTIFY widthChanged)
    Q_PROPERTY(int height READ height WRITE setHeight NOTIFY heightChanged)
    Q_PROPERTY(int z READ z WRITE setZ NOTIFY zChanged)
    Q_PROPERTY(bool shownOnScreen READ shownOnScreen NOTIFY shownOnScreenChanged)

public:
    explicit ExternalWindow(WindowManager *windowManager, QObject *parent = 0);

    bool visible() const;
    void setVisible(bool visible);

    QRect geometry() const;
    void setGeometry(const QRect &geometry);

    Q_PID pid() const;
    void setPid(Q_PID pid);

    int x() const;
    void setX(int x);

    int y() const;
    void setY(int y);

    int width() const;
    void setWidth(int width);

    int height() const;
    void setHeight(int height);

    int z() const;
    void setZ(int z);

    /**
     * Function will window clipping star points
     * @param x clipping x coordinate
     * @param y clipping y coordinate
     */
    void setClipPosition(int x, int y);

    /**
     * Function will set size of the clipping region
     * @param width - width of the clipping region
     * @param height  - height of the clipping region
     */
    void setClipSize(int width, int height);

    /**
     * Function sets sensitivity on current window
     * @param sensetivity value, see WindowManager::Sensitivities enumeration
     */
    void setSensitivity(const WindowManager::Sensitivities sensetivity);

    /**
     * Function will set size of the clipping region for source window,
     * is Q_INVOKABLE to be able to call this from QML
     * @param width - width of the clipping region
     * @param height  - height of the clipping region
     */
    Q_INVOKABLE void setSourceClipSize(int width, int height);

    /**
     * Function will set size of the source window,
     * is Q_INVOKABLE to be able to call this from QML
     * @param width - source window width
     * @param height - source window height
     */
    Q_INVOKABLE void setSourceSize(int width, int height);

    bool shownOnScreen() const;

public Q_SLOTS:
    void handleApplicationStarted();

Q_SIGNALS:
    void visibleChanged(bool visible);
    void geometryChanged(const QRect &geometry);
    void xChanged(int);
    void yChanged(int);
    void widthChanged(int);
    void heightChanged(int);
    void zChanged(int);
    void shownOnScreenChanged(bool);

private:
    WindowManager *m_windowManager;
    bool m_visible;
    QRect m_geometry;
    QPoint m_clipPosition;
    QSize m_clippingSize;
    QSize m_sourceClipSize;
    QSize m_sourceSize;
    int m_z;
    int m_sensitivity;
    Q_PID m_pid;
    bool m_shownOnScreen;
};

#endif
