#include "RadioController.h"

#include <QDebug>

RadioController::RadioController(QObject *parent)
    : QObject(parent)
    , m_frequencyModel(new QStringListModel(this))
    , m_frequencyMinimum(0.0)
    , m_frequencyMaximum(0.0)
    , m_frequencyStep(0.0)
{
}

int RadioController::frequencyToIndex(double frequency) const
{
    const QStringList frequencies = m_frequencyModel->stringList();

    return frequencies.indexOf(QString::number(frequency));
}

double RadioController::indexToFrequency(int index) const
{
    const QStringList frequencies = m_frequencyModel->stringList();
    if (index < 0 || index >= frequencies.count())
        return 0.0;

    return frequencies[index].toDouble();
}

void RadioController::updateModel()
{
    if (m_frequencyMinimum >= m_frequencyMaximum)
        return;

    if (qFuzzyIsNull(m_frequencyStep))
        return;

    QStringList values;
    for (double frequency = m_frequencyMinimum; ((frequency < m_frequencyMaximum) || qFuzzyCompare(frequency, m_frequencyMaximum)); frequency += m_frequencyStep) {
        values << QString::number(frequency);
    }

    m_frequencyModel->setStringList(values);
}

QAbstractItemModel *RadioController::frequencyModel() const
{
    return m_frequencyModel;
}

double RadioController::frequencyMinimum() const
{
    return m_frequencyMinimum;
}

void RadioController::setFrequencyMinimum(double frequency)
{
    if (qFuzzyCompare(m_frequencyMinimum, frequency))
        return;

    m_frequencyMinimum = frequency;
    updateModel(); // update frequencies before notification
    emit frequencyMinimumChanged();
}

double RadioController::frequencyMaximum() const
{
    return m_frequencyMaximum;
}

void RadioController::setFrequencyMaximum(double frequency)
{
    if (qFuzzyCompare(m_frequencyMaximum, frequency))
        return;

    m_frequencyMaximum = frequency;
    updateModel(); // update frequencies before notification
    emit frequencyMaximumChanged();
}

double RadioController::frequencyStep() const
{
    return m_frequencyStep;
}

void RadioController::setFrequencyStep(double step)
{
    if (qFuzzyCompare(m_frequencyStep, step))
        return;

    m_frequencyStep = step;
    updateModel(); // update frequencies before notification
    emit frequencyStepChanged();
}
