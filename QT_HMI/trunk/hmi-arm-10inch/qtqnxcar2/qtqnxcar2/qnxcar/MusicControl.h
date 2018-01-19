#ifndef QTQNXCAR2_MUSIC_CONTROL_H
#define QTQNXCAR2_MUSIC_CONTROL_H

#include <QObject>

#include "qtqnxcar2_export.h"

namespace QnxCar {

/**
 * Access to heating, ventilation and air conditioning
 *
 * Wraps the @c /pps/qnxcar/hvac PPS object
 */
class QTQNXCAR2_EXPORT MusicControl : public QObject
{
    Q_OBJECT

    /// The level of the left heated seat in the range from 0 to 3
    Q_PROPERTY(int musicControlCmd READ musicControlCmd WRITE setMusicControlCmd NOTIFY musicControlCmdChanged)
    Q_PROPERTY(QString multimediaControlCmd READ multimediaControlCmd WRITE setMultimediaControlCmd NOTIFY multimediaControlCmdChanged)

public:
    /**
     * Creates a new Hvac object.
     *
     * @param parent The parent object.
     */
    explicit MusicControl(QObject *parent = 0);

    int getIndexByMusicControl(QString controlcmd);

    int musicControlCmd() const;
    Q_INVOKABLE void setMusicControlCmd(int musicControlCmd);
    QString multimediaControlCmd() const;
    Q_INVOKABLE void setMultimediaControlCmd(QString multimediaControlCmd);

Q_SIGNALS:
    void musicControlCmdChanged(int);
    void multimediaControlCmdChanged(QString);

private:
    class Private;
    Private* const d;
};

}

#endif
