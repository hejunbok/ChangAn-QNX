#include <QtTest/QtTest>

#include "simulator/Simulator.h"
#include "services/AudioControl.h"

#include <qpps/simulator.h>

using QnxCar::AudioControl;

class tst_AudioControl : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void testDefaultValues();
    void testSetProperties();
};

void tst_AudioControl::testDefaultValues()
{
    QPps::Simulator::self()->reset();
    QnxCar2Simulator::initDemoData();

    AudioControl audio;

    // check default values
    QCOMPARE(qRound(audio.speakerVolume()), 60);
    QCOMPARE(qRound(audio.handsetVolume()), 60);
    QCOMPARE(qRound(audio.balance()), 50);
    QCOMPARE(qRound(audio.bass()), 74);
    QCOMPARE(qRound(audio.fade()), 42);
    QCOMPARE(qRound(audio.mid()), 92);
    QCOMPARE(qRound(audio.treble()), 79);
    QCOMPARE(audio.currentOutputDevice(), AudioControl::Speaker);
}

void tst_AudioControl::testSetProperties()
{
    QPps::Simulator::self()->reset();
    QnxCar2Simulator::initDemoData();

    AudioControl audio;

    QCOMPARE(audio.currentOutputDevice(), AudioControl::Speaker);
    QCOMPARE(audio.currentOutputVolume(), 60.0);
    // set basic properties (valid values, different from default-values)
    audio.setSpeakerVolume(30.0);
    QCOMPARE(audio.speakerVolume(), 30.0);
    QCOMPARE(audio.handsetVolume(), 60.0);
    QCOMPARE(audio.currentOutputVolume(), 30.0);
    audio.setCurrentOutputVolume(80.0);
    QCOMPARE(audio.speakerVolume(), 80.0);
    QCOMPARE(audio.handsetVolume(), 60.0);
    audio.setHandsetVolume(50.0);
    QCOMPARE(audio.handsetVolume(), 50.0);
    audio.setBalance(30.0);
    QCOMPARE(audio.balance(), 30.0);
    audio.setBass(30.0);
    QCOMPARE(audio.bass(), 30.0);
    audio.setFade(30.0);
    QCOMPARE(audio.fade(), 30.0);
    audio.setMid(30.0);
    QCOMPARE(audio.mid(), 30.0);
    audio.setTreble(30.0);
    QCOMPARE(audio.treble(), 30.0);

    // set basic properties (out-of-range values)
    audio.setSpeakerVolume(101.0);
    QCOMPARE(audio.speakerVolume(), 100.0);
    audio.setHandsetVolume(101.0);
    QCOMPARE(audio.handsetVolume(), 100.0);
    audio.setBalance(101.0);
    QCOMPARE(audio.balance(), 100.0);
    audio.setBass(101.0);
    QCOMPARE(audio.bass(), 100.0);
    audio.setFade(101.0);
    QCOMPARE(audio.fade(), 100.0);
    audio.setMid(101.0);
    QCOMPARE(audio.mid(), 100.0);
    audio.setTreble(101.0);
    QCOMPARE(audio.treble(), 100.0);
}

QTEST_MAIN(tst_AudioControl)
#include "tst_audiocontrol.moc"
