#include <QtTest/QtTest>

#include "simulator/Simulator.h"
#include "services/AudioPlayer.h"

#include <qpps/simulator.h>

class tst_AudioPlayer : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void testBasic();
};

void tst_AudioPlayer::testBasic()
{
    QPps::Simulator::self()->reset();
    QnxCar2Simulator::initDemoData();

    // TODO: write tests
    //AudioPlayer player;
}

QTEST_MAIN(tst_AudioPlayer)
#include "tst_audioplayer.moc"
