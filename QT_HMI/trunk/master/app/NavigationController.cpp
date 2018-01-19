#include "NavigationController.h"

#include <QFile>
#include <QIODevice>
#include <QRect>

#include <screen/screen.h>
#include "Layout.h"

NavigationController::NavigationController(QObject *parent) :
    QThread(parent)
  , m_applicationWindow(0)
  , m_navigationWindow(0)
  , m_screenContext(0)
  , m_show(false)
  , m_guiLoaded(false)
{
}

bool NavigationController::guiLoaded() const
{
    return m_guiLoaded;
}

void NavigationController::setGuiLoaded(bool loaded)
{
    m_guiLoaded = loaded;

    if (loaded && m_show)
        setNavigationVisible(true);
    else if (loaded) {
        setNavigationVisible(false);
    }
}

void NavigationController::run()
{
    Q_ASSERT(m_windowGeometry.isValid());

    screen_context_t *screen_ctx = new screen_context_t();

    int rc = screen_create_context(screen_ctx, SCREEN_APPLICATION_CONTEXT);
    if (rc != 0)
        qCritical("NavigationController: Failed create navigation screen context");

    m_screenContext = screen_ctx;

    screen_window_t *navigationWindow = new screen_window_t();
    rc = screen_create_window(navigationWindow, *screen_ctx);
    if (rc != 0)
        qCritical("Failed create navigation window");

    m_applicationWindow = navigationWindow;

    //TODO: Set display
//    screen_display_t display = QGuiApplication::primaryScreen()->handle();
//    rc = screen_set_window_property_pv(navigationWindow, SCREEN_PROPERTY_DISPLAY,(void**)&display);

    int val[2];
    val[0] = ZORDER_UNDERGROUND;
    rc = screen_set_window_property_iv(*navigationWindow, SCREEN_PROPERTY_ZORDER, val);
    if (rc != 0)
        qCritical("NavigationController: Failed setting zorder for navigation window group");

    val[0] = 0;
    rc = screen_set_window_property_iv(*navigationWindow, SCREEN_PROPERTY_VISIBLE, val);
    if (rc != 0)
        qCritical("NavigationController: Failed setting visibility for navigation window group");

    // Window won't be visible unless it has some buffers so make one dummy buffer that is 1x1
    val[0] = SCREEN_USAGE_NATIVE;
    rc = screen_set_window_property_iv(*navigationWindow, SCREEN_PROPERTY_USAGE, val);
    if (rc != 0)
        qCritical("NavigationController: Failed setting zorder for navigation window group");

    val[0] = 1;
    val[1] = 1;
    rc = screen_set_window_property_iv(*navigationWindow, SCREEN_PROPERTY_BUFFER_SIZE, val);
    if (rc != 0)
        qCritical("NavigationController: Failed setting window buffer size");

    rc = 0;
    rc = screen_create_window_buffers(*navigationWindow, 1);
    if (rc != 0)
        qCritical("NavigationController: Failed to create window buffer");

    val[0] = m_windowGeometry.width();
    val[1] = m_windowGeometry.height();
    rc = screen_set_window_property_iv(*navigationWindow, SCREEN_PROPERTY_SIZE, val);
    if (rc != 0)
        qCritical("NavigationController: Failed setting window size");

    val[0] = m_windowGeometry.left();
    val[1] = m_windowGeometry.top();
    rc = screen_set_window_property_iv(*navigationWindow, SCREEN_PROPERTY_POSITION, val);
    if (rc != 0)
        qCritical("NavigationController: Failed setting window position");

    val[0] = 0;
    rc = screen_set_window_property_iv(*navigationWindow, SCREEN_PROPERTY_COLOR, val);
    if (rc != 0)
        qCritical("NavigationController: Failed setting window colour");

    val[0] = SCREEN_TRANSPARENCY_DISCARD;
    rc = screen_set_window_property_iv(*navigationWindow, SCREEN_PROPERTY_TRANSPARENCY, val);
    if (rc != 0)
        qCritical("NavigationController: Failed setting window transparency");

    val[0] = 1;
    rc = screen_set_window_property_iv(*navigationWindow, SCREEN_PROPERTY_SWAP_INTERVAL, val);
    if (rc != 0)
        qCritical("NavigationController: Failed setting window swap interval");

    val[0] = m_windowGeometry.width();
    val[1] = m_windowGeometry.height();
    rc = screen_set_window_property_iv(*navigationWindow, SCREEN_PROPERTY_SOURCE_SIZE, val);
    if (rc != 0)
        qCritical("NavigationController: Failed setting window source size");

    val[0] = 0;
    val[1] = 0;
    rc = screen_set_window_property_iv(*navigationWindow, SCREEN_PROPERTY_SOURCE_POSITION, val);
    if (rc != 0)
        qCritical("NavigationController: Failed setting window source position");

    rc = screen_create_window_group(*navigationWindow, "ebnavigation");
    if (rc != 0)
        qCritical("NavigationController: Failed creating navigation window group");

    const char * id = "ebnavigation";
    rc = screen_set_window_property_cv(*navigationWindow, SCREEN_PROPERTY_ID_STRING, strlen(id), id);
    if (rc != 0)
        qCritical("NavigationController: Failed setting ID");


    screen_buffer_t buffer;
    rc = screen_get_window_property_pv(*navigationWindow, SCREEN_PROPERTY_RENDER_BUFFERS, (void **)&buffer);
    if (rc != 0)
        qCritical("NavigationController: Failed to query window buffer");

    int dirtyRect[] = {0, 0, 1, 1};
    rc = screen_post_window(*navigationWindow, buffer, 1, dirtyRect, 0);
    if (rc != 0)
        qCritical("NavigationController: Failed to post window buffer");

    QFile file(QStringLiteral("/pps/system/navigator/windowgroup"));

    if (!file.open(QIODevice::Append))
        qCritical("NavigationController: Failed passing window group to navigation");

    const QByteArray data("[n]navigation::ebnavigation");

    if (file.write(data) != data.length())
         qCritical("NavigationController: Failed writing window group to navigation");

    file.close();

    screen_event_t screen_ev;
    rc = screen_create_event(&screen_ev);
    if (rc != 0)
        qCritical("NavigationController: Failed to create screen event");

    int type = 0;

    //Wait for the window of the navigation to be posted
    do {
        rc = screen_get_event(*screen_ctx, screen_ev, 100000000);

        rc = screen_get_event_property_iv(screen_ev, SCREEN_PROPERTY_TYPE, &type);

    } while (type != SCREEN_EVENT_POST);

    screen_window_t *winHandle = new screen_window_t();

    rc = screen_get_event_property_pv(screen_ev, SCREEN_PROPERTY_WINDOW, (void **)winHandle);
    if (rc != 0)
        qCritical("NavigationController: Failed to get window handle");

    screen_destroy_event(screen_ev);

    m_navigationWindow = winHandle;

    val[0] = ZORDER_NAVIGATION_OVERLAY;
    rc = screen_set_window_property_iv(*winHandle, SCREEN_PROPERTY_ZORDER, val);
    if (rc != 0)
        qCritical("NavigationController: Failed setting zorder for navigation window");

    if (m_show && m_guiLoaded) {
        val[0] = 1;
        rc = screen_set_window_property_iv(*navigationWindow, SCREEN_PROPERTY_VISIBLE, val);
        if (rc != 0)
            qCritical("NavigationController: Failed setting visibility for navigation window group");
    } else {
        val[0] = 0;
        rc = screen_set_window_property_iv(*winHandle, SCREEN_PROPERTY_VISIBLE, val);
        if (rc != 0)
            qCritical("NavigationController: Failed setting visibility for navigation window");
    }


    rc = screen_flush_context(*screen_ctx, 0);
    if (rc != 0)
        qCritical("NavigationController: Failed flushing screen context");

    Q_EMIT loadingFinished();
}

void NavigationController::setNavigationVisible(bool visible)
{
    m_show = visible;
    if (!m_applicationWindow || !m_navigationWindow || !m_screenContext || !m_guiLoaded)
        return;

    int rc = 0;
    int val;

    if (visible)
        val = 1;
    else
        val = 0;

    rc = screen_set_window_property_iv(*m_applicationWindow, SCREEN_PROPERTY_VISIBLE, &val);
    if (rc != 0)
        qCritical("NavigationController: Failed setting visibility for navigation window");

    rc = screen_set_window_property_iv(*m_navigationWindow, SCREEN_PROPERTY_VISIBLE, &val);
    if (rc != 0)
        qCritical("NavigationController: Failed setting visibility for navigation window");

    rc = screen_flush_context(*m_screenContext, 0);
    if (rc != 0)
        qCritical("NavigationController: Failed flushing screen context");
}

void NavigationController::setWindowGeometry(const QRect &geometry)
{
    m_windowGeometry = geometry;
}
