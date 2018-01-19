#ifndef ABSTRACTMODULE_H
#define ABSTRACTMODULE_H

#include <QJsonValue>
#include <QString>

/**
 * @short An interface for all module facade classes
 *
 * This interface is used by the ModuleManager to inform the
 * modules about the fact that the associated button in the
 * navigator bar has been pressed even though the module was
 * active already.
 */
class AbstractModule
{
public:
    /**
     * Called by the module manager when the module tab is activated
     * while already being the current module
     */
    virtual void moduleReset() {}

    /**
     * @brief moduleSelected is called when the tab get activated
     */
    virtual void moduleSelected() {}

    /**
     * Returns the navigator tab identifier used for PPS communication
     */
    virtual QString navigatorTabName() const = 0;

    /**
     * @brief moduleLeave is called when the tab get deactivated
     */
    virtual void moduleDeselected() {}

    /**
     * Reimplement to handle launch requests from the outside (e.g. ASR)
     *
     * @param dat the "dat" argument containing the command to execute.
     * This can be a single string (e.g. "Toronto"), or a json object
     * (E.g. '{"action":"audio"}' for media player). TODO link documentation of
     * possible commands once there is one.
     *
     */
    virtual void handleLaunchRequest(const QJsonValue &dat) { Q_UNUSED(dat) }
};

#endif
