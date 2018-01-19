#include "dirmonitor.h"

#include <QSocketNotifier>
#include <QtDebug>
#include <QTextStream>

#include <errno.h>
#include <fcntl.h>
#include <sys/types.h>
#include <unistd.h>

DirMonitor::DirMonitor(const QString &path, QObject *parent)
   : QObject(parent),
     m_path(path),
     m_notifier(0)
{
    // delta mode doesn't seem to work with .all, but keep the suffix to document that we want it...
    const QString openPath = path + QStringLiteral("/.all?delta");
    const int fd = ::open(openPath.toLocal8Bit(), O_RDONLY);
    if (fd == -1) {
        qWarning() << QObject::tr("Unable to open %1: %2")
                      .arg(openPath, QString::fromLocal8Bit(strerror(errno)));
        return;
    }

    m_notifier = new QSocketNotifier(fd, QSocketNotifier::Read, this);
    connect(m_notifier, SIGNAL(activated(int)), this, SLOT(readData()));
}

void DirMonitor::readData()
{
    if (!m_notifier)
        return;

    char buffer[1 << 16];

    forever {
        const int count = ::read(m_notifier->socket(), buffer, sizeof(buffer));
        if (count == -1) {
            qWarning() << QObject::tr("Unable to read data from %1: %2")
                          .arg(m_path, QString::fromLocal8Bit(strerror(errno)));
            return;
        } else if (count == 0) {
            // end of data
            break;
        }

        const QList<QByteArray> lines = QByteArray::fromRawData(buffer, count).split('\n');
        if (!prettyPrintLines(lines)) {
            // stop in case of bad format
            return;
        }
    }
}

static bool qualifiersChanged(const QByteArray &qualifiers)
{
    // minimum length for a qualifier change is 4: e.g. [-n]
    return qualifiers.length() >= 4 && QByteArrayLiteral("+-").contains(qualifiers.at(1));
}

bool DirMonitor::prettyPrintLines(const QList<QByteArray> &lines)
{
    QTextStream out(stdout);
    QString objectName;

    foreach (QByteArray line, lines) {
        // ignore empty lines
        if (line.isEmpty())
            continue;

        QByteArray qualifiers;
        if (line.startsWith('[')) {
            const int closingBrace = line.indexOf(']');
            if (closingBrace == -1) {
                qDebug() << QObject::tr("Invalid format (no closing brace ']') from %1: '%2'")
                            .arg(m_path, QString::fromLocal8Bit(line));
                return false;
            }
            qualifiers = line.left(closingBrace + 1);
            line.remove(0, closingBrace + 1);
        }

        QByteArray plusminus;
        // handle +, -, #, * that only occur on the ".all" pseudo-object
        if (QByteArray("+-#*").contains(line.at(0))) {
            plusminus += line.at(0);
            line.remove(0, 1);
        }

        QByteArray dir;
        QByteArray indent;
        if (line.startsWith('@')) { // object prefix
            plusminus += line.at(0); // small hack, avoids another byte array
            line.remove(0, 1);
            dir += m_path;
            dir += '/';
            objectName = line;
            if (plusminus == "-")
                m_seenAttributes.remove(objectName);
        } else {
            // indent attribute changes and filter out redundant updates
            indent = QByteArrayLiteral("    ");
            const int firstColon = line.indexOf(':');
            const QString attrName = line.left(firstColon);
            const QString value = line.mid(firstColon);

            if (plusminus == "-" && m_seenAttributes.contains(objectName)) {
                m_seenAttributes[objectName].remove(attrName);
            } else {
                // suppress redundant updates; do that only when an attributes's value *and*
                // metadata are unchanged
                const bool metadataUnchanged = plusminus.isEmpty() &&
                                               !qualifiersChanged(qualifiers);
                if (metadataUnchanged && m_seenAttributes.contains(objectName)) {
                    const QHash<QString, QString> &object = m_seenAttributes[objectName];
                    if (object.contains(attrName) && object[attrName] == value)
                        continue;
                }
                m_seenAttributes[objectName][attrName] = value;
            }
        }

        out << QString::fromLocal8Bit(indent + plusminus + qualifiers + dir + line + '\n');
    }
    return true;
}
