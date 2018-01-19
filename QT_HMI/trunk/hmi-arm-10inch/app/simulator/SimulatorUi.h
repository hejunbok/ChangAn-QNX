#ifndef SIMULATORUI_H
#define SIMULATORUI_H

#include <QWidget>

class SimulatorControl;
class SimulatorModel;

class QLineEdit;
class QModelIndex;
class QPushButton;
class QCheckBox;
class QTreeView;

class SimulatorUi : public QWidget
{
    Q_OBJECT

public:
    explicit SimulatorUi(QWidget *parent = 0);

    void setModel(SimulatorModel *model);

private Q_SLOTS:
    void publish();
    void deleteAttribute();
    void insertNewAttribute();
    void currentItemChanged(const QModelIndex &index);

    void toggleBluetoothService(const QString &service, bool toggle);
    void toggleConnectHandsFreeService(bool toggled);
    void toggleConnectMessageAccessService(bool toggled);
    void toggleConnectPhoneBookAccessService(bool toggled);

private:
    void refresh();

    SimulatorControl *m_control;

    QWidget * createPpsTree();
    QWidget * createGlobalSettings();
    QWidget *createMediaLibraryModuleView();
    QWidget *createSettingsModuleView();

    QTreeView* m_treeView;
    QLineEdit* m_valueEdit;
    QLineEdit* m_encodingEdit;
    QPushButton* m_publishButton;
    QPushButton* m_deleteButton;
    QPushButton* m_addNewButton;
    QCheckBox* m_connectHandsFreeService;
    QCheckBox* m_connectMessageAccessService;
    QCheckBox* m_connectPhoneBookAccessService;
    QString m_currentKey;
    QString m_currentPpsPath;
};

#endif
