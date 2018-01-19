#ifndef RADIOCONTROLLER_HPP
#define RADIOCONTROLLER_HPP

#include <QStringListModel>

class RadioController : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QAbstractItemModel* frequencyModel READ frequencyModel CONSTANT)
    Q_PROPERTY(double frequencyMinimum READ frequencyMinimum WRITE setFrequencyMinimum NOTIFY frequencyMinimumChanged)
    Q_PROPERTY(double frequencyMaximum READ frequencyMaximum WRITE setFrequencyMaximum NOTIFY frequencyMaximumChanged)
    Q_PROPERTY(double frequencyStep READ frequencyStep WRITE setFrequencyStep NOTIFY frequencyStepChanged)

public:
    explicit RadioController(QObject *parent = 0);

    Q_INVOKABLE int frequencyToIndex(double frequency) const;
    Q_INVOKABLE double indexToFrequency(int index) const;

Q_SIGNALS:
    void frequencyMinimumChanged();
    void frequencyMaximumChanged();
    void frequencyStepChanged();

private:
    void updateModel();

    QAbstractItemModel *frequencyModel() const;
    double frequencyMinimum() const;
    void setFrequencyMinimum(double frequency);
    double frequencyMaximum() const;
    void setFrequencyMaximum(double frequency);
    double frequencyStep() const;
    void setFrequencyStep(double step);

    QStringListModel *m_frequencyModel;
    double m_frequencyMinimum;
    double m_frequencyMaximum;
    double m_frequencyStep;
};

#endif
