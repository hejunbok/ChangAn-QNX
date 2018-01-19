#ifndef METADATA_H
#define METADATA_H

#include "qtqnxcar2_export.h"

#include <QUrl>

namespace QnxCar {
namespace MediaPlayer {

class QTQNXCAR2_EXPORT Metadata
{
public:
    Metadata();

    QString title;
    int duration;
    QUrl artwork;
    QString artist;
    QString album;
    QString genre;
    QString year;
    int width;
    int height;
    int disc;
    int track;
};

}
}

#endif // METADATA_H
