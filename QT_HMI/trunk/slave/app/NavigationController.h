#ifndef NAVIGATIONCONTROLLER_H
#define NAVIGATIONCONTROLLER_H

#include <QRect>
#include <QThread>

typedef struct _screen_window *screen_window_t;
typedef struct _screen_context *screen_context_t;

class NavigationController : public QThread
{
    Q_OBJECT
public:
    explicit NavigationController(QObject *parent = 0);
    void setWindowGeometry(const QRect &rect);

    bool guiLoaded() const;
    void setGuiLoaded(bool loaded);

Q_SIGNALS:
    void loadingFinished();

public Q_SLOTS:
    void setNavigationVisible(bool visible);

protected:
    void run();

private:
    screen_window_t *m_applicationWindow;
    screen_window_t *m_navigationWindow;
    screen_context_t *m_screenContext;
    bool m_show;
    bool m_guiLoaded;
    QRect m_windowGeometry;
};

#endif
