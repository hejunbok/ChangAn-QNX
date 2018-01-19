#include <QtTest/QtTest>

#include "simulator/Simulator.h"
#include "services/VoiceControl.h"

#include <qpps/simulator.h>

using QnxCar::VoiceControl;

class tst_VoiceControl : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void testDefaultValues();
};

void tst_VoiceControl::testDefaultValues()
{
    QPps::Simulator::self()->reset();
    QnxCar2Simulator::initDemoData();

    VoiceControl voice;

    // check default values
    QCOMPARE(voice.state(), VoiceControl::IdleState);
    QCOMPARE(voice.speechState(), VoiceControl::NoSpeech);
    QCOMPARE(voice.confidence(), 0);
    QCOMPARE(voice.utterance(), QStringLiteral(""));
}

QTEST_MAIN(tst_VoiceControl)
#include "tst_voicecontrol.moc"
