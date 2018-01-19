#include "SimulatorUi.h"

#include "InsertNewAttributeDialog.h"
#include "SimulatorControl.h"
#include "SimulatorModel.h"

#include "modules/mediaplayer/MediaPlayerModule.h"
#include "modules/settings/SettingsModule.h"

#include "qnxcar/AlbumsModel.h"
#include "qnxcar/ArtistsModel.h"
#include "qnxcar/GenresModel.h"
#include "qnxcar/DatabaseManager.h"
#include "qnxcar/PlaylistModel.h"
#include "qnxcar/ProfileModel.h"
#include "qnxcar/SongsModel.h"
#include "qnxcar/SourcesModel.h"
#include "qnxcar/VideosModel.h"
#include "qnxcar/SettingsModels.h"
#include "services/BluetoothDevicesModel.h"

#include <qpps/simulator.h>

#include <QFormLayout>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QLineEdit>
#include <QPushButton>
#include <QCheckBox>
#include <QStandardItemModel>
#include <QTreeView>
#include <QTableView>
#include <QTabWidget>
#include <QTimer>

using namespace QnxCar;

SimulatorUi::SimulatorUi(QWidget *parent)
    : QWidget(parent)
    , m_control(new SimulatorControl(this))
{
    setWindowTitle(QStringLiteral("PPS Simulator"));

    QHBoxLayout *layout = new QHBoxLayout(this);
    QTabWidget *tab = new QTabWidget;
    tab->addTab(createPpsTree(), QStringLiteral("PPS Tree"));
    tab->addTab(createGlobalSettings(), QStringLiteral("Global Settings"));
    tab->addTab(createMediaLibraryModuleView(), QStringLiteral("Media Library Module"));
    tab->addTab(createSettingsModuleView(), tr("Settings Module"));
    layout->addWidget(tab);
}

void SimulatorUi::setModel(SimulatorModel *model)
{
    Q_ASSERT(!m_treeView->model()); // only allow to set once for now

    QSortFilterProxyModel *proxyModel = new QSortFilterProxyModel(this);
    proxyModel->setSourceModel(model);
    proxyModel->setDynamicSortFilter(true);
    proxyModel->setSortRole(Qt::DisplayRole);
    m_treeView->setModel(proxyModel);
    m_treeView->sortByColumn(0, Qt::AscendingOrder);
}

QWidget * SimulatorUi::createPpsTree()
{
    QWidget *ppsTree = new QWidget;
    QHBoxLayout *layout = new QHBoxLayout(ppsTree);

    m_treeView = new QTreeView;
    m_treeView->header()->hide();
    m_treeView->setUniformRowHeights(true);
    m_treeView->setSortingEnabled(true);
    layout->addWidget(m_treeView);

    QWidget *editPage = new QWidget;
    QFormLayout *formLayout = new QFormLayout(editPage);
    layout->addWidget(editPage);

    m_valueEdit = new QLineEdit;
    connect(m_valueEdit, SIGNAL(returnPressed()), this, SLOT(publish()));
    m_encodingEdit = new QLineEdit;
    connect(m_encodingEdit, SIGNAL(returnPressed()), this, SLOT(publish()));
    m_publishButton = new QPushButton(QStringLiteral("Update"));
    m_deleteButton = new QPushButton(QStringLiteral("Delete Attribute"));
    m_addNewButton = new QPushButton(QStringLiteral("Insert New Attribute"));

    formLayout->addRow(QStringLiteral("Value"), m_valueEdit);
    formLayout->addRow(QStringLiteral("Encoding"), m_encodingEdit);
    formLayout->addRow(QString(), m_publishButton);
    formLayout->addRow(QString(), m_deleteButton);
    formLayout->addRow(QString(), m_addNewButton);

    connect(m_treeView, SIGNAL(activated(QModelIndex)),
            this, SLOT(currentItemChanged(QModelIndex)));

    connect(m_publishButton, SIGNAL(clicked()), SLOT(publish()));
    connect(m_deleteButton, &QPushButton::clicked, this, &SimulatorUi::deleteAttribute);
    connect(m_addNewButton, &QPushButton::clicked, this, &SimulatorUi::insertNewAttribute);

    return ppsTree;
}

QWidget * SimulatorUi::createGlobalSettings()
{
    QWidget *globalSettings = new QWidget;
    QFormLayout *form = new QFormLayout(globalSettings);

    m_connectHandsFreeService = new QCheckBox(QStringLiteral("Connect Hands-Free Bluetooth service"));
    m_connectMessageAccessService = new QCheckBox(QStringLiteral("Connect Message Access Bluetooth service"));
    m_connectPhoneBookAccessService = new QCheckBox(QStringLiteral("Connect PhoneBook Access Bluetooth service"));
    form->addRow(QString(), m_connectHandsFreeService);
    form->addRow(QString(), m_connectMessageAccessService);
    form->addRow(QString(), m_connectPhoneBookAccessService);

    connect(m_connectHandsFreeService, SIGNAL(toggled(bool)), SLOT(toggleConnectHandsFreeService(bool)));
    connect(m_connectMessageAccessService, SIGNAL(toggled(bool)), SLOT(toggleConnectMessageAccessService(bool)));
    connect(m_connectPhoneBookAccessService, SIGNAL(toggled(bool)), SLOT(toggleConnectPhoneBookAccessService(bool)));

    return globalSettings;
}

QWidget *SimulatorUi::createMediaLibraryModuleView()
{
    QWidget *widget = new QWidget;
    QVBoxLayout *vbox = new QVBoxLayout(widget);

    MediaPlayerModule *mediaPlayerModule = new MediaPlayerModule(this);

    QTabWidget *tabWidget = new QTabWidget;

    QTreeView *sourcesView = new QTreeView;
    sourcesView->setRootIsDecorated(false);
    sourcesView->setModel(mediaPlayerModule->sourcesModel());
    tabWidget->addTab(sourcesView, tr("Sources Model"));

    QTreeView *artistsView = new QTreeView;
    artistsView->setRootIsDecorated(false);
    artistsView->setModel(mediaPlayerModule->artistsModel());
    tabWidget->addTab(artistsView, tr("Artists Model"));

    QTreeView *albumsView = new QTreeView;
    albumsView->setRootIsDecorated(false);
    albumsView->setModel(mediaPlayerModule->albumsModel());
    tabWidget->addTab(albumsView, tr("Albums Model"));

    QTreeView *genresView = new QTreeView;
    genresView->setRootIsDecorated(false);
    genresView->setModel(mediaPlayerModule->genresModel());
    tabWidget->addTab(genresView, tr("Genres Model"));

    QTreeView *songsView = new QTreeView;
    songsView->setRootIsDecorated(false);
    songsView->setModel(mediaPlayerModule->songsModel());
    tabWidget->addTab(songsView, tr("Songs Model"));

    QTreeView *videosView = new QTreeView;
    videosView->setRootIsDecorated(false);
    videosView->setModel(mediaPlayerModule->videosModel());
    tabWidget->addTab(videosView, tr("Videos Model"));

    QTreeView *playlistView = new QTreeView;
    playlistView->setRootIsDecorated(false);
    playlistView->setModel(mediaPlayerModule->playlistModel());
    tabWidget->addTab(playlistView, tr("Playlist Model"));

    vbox->addWidget(tabWidget);

    return widget;
}

QWidget *SimulatorUi::createSettingsModuleView()
{
    QWidget *widget = new QWidget;
    QVBoxLayout *vbox = new QVBoxLayout(widget);

    ProfileModel *profileModel = new ProfileModel(DatabaseManager::database(QStringLiteral("personalization.db")), this);

    QTabWidget *tabWidget = new QTabWidget;

    // profile model
    {
        QWidget *profileModelWidget = new QWidget(this);
        QVBoxLayout *profileModelLayout = new QVBoxLayout(profileModelWidget);

        QPushButton *refreshButton = new QPushButton(tr("Refresh"));
        profileModelLayout->addWidget(refreshButton);
        connect(refreshButton, &QPushButton::clicked, profileModel, &ProfileModel::update);

        QTableView *profileTreeView = new QTableView(this);
        profileTreeView->setModel(profileModel);
        profileModelLayout->addWidget(profileTreeView);
        tabWidget->addTab(profileModelWidget, tr("Profile Model"));
    }

    // paired bluetooth devices model
    QTreeView *pairedDevicesView = new QTreeView(this);
    pairedDevicesView->setModel(new BluetoothDevicesModel(BluetoothDevicesModel::PairedDevices, this));
    tabWidget->addTab(pairedDevicesView, tr("Paired Bluetooth Devices"));

    // remote bluetooth devices model
    QTreeView *remoteDevicesView = new QTreeView(this);
    remoteDevicesView->setModel(new BluetoothDevicesModel(BluetoothDevicesModel::RemoteDevices, this));
    tabWidget->addTab(remoteDevicesView, tr("Remote Bluetooth Devices"));

    vbox->addWidget(tabWidget);

    return widget;
}

void SimulatorUi::publish()
{
    if (m_currentKey.isEmpty())
        return;

    QPps::Simulator::self()->registerClient(m_currentPpsPath, this);
    QPps::Simulator::self()->clientSetAttribute(this, m_currentKey, m_valueEdit->text().toLocal8Bit(),
                                                m_encodingEdit->text().toLocal8Bit());
    QPps::Simulator::self()->unregisterClient(this);
}

void SimulatorUi::deleteAttribute()
{
    if (m_currentKey.isEmpty())
        return;

    QPps::Simulator::self()->registerClient(m_currentPpsPath, this);
    QPps::Simulator::self()->clientRemoveAttribute(this, m_currentKey);
    QPps::Simulator::self()->unregisterClient(this);
}

void SimulatorUi::insertNewAttribute()
{
    InsertNewAttributeDialog dialog(this);
    if (dialog.exec() != QDialog::Accepted)
        return;

    const QString key = dialog.key();
    const QByteArray value = dialog.value().toLocal8Bit();
    const QByteArray encoding = dialog.encoding().toLocal8Bit();

    QPps::Simulator::self()->registerClient(m_currentPpsPath, this);
    QPps::Simulator::self()->clientSetAttribute(this, key, value, encoding);
    QPps::Simulator::self()->unregisterClient(this);
}

void SimulatorUi::toggleConnectHandsFreeService(bool toggled)
{
    toggleBluetoothService(QStringLiteral("hfp"), toggled);
}

void SimulatorUi::toggleConnectMessageAccessService(bool toggled)
{
    toggleBluetoothService(QStringLiteral("map"), toggled);
}

void SimulatorUi::toggleConnectPhoneBookAccessService(bool toggled)
{
    toggleBluetoothService(QStringLiteral("pbap"), toggled);
}

void SimulatorUi::toggleBluetoothService(const QString &service, bool toggled)
{
    m_control->setBluetoothDeviceEnabled(service, toggled);
}

void SimulatorUi::currentItemChanged(const QModelIndex &index)
{
    if (!index.isValid()) {
        m_valueEdit->clear();
        m_encodingEdit->clear();
        m_currentKey.clear();
        m_currentPpsPath.clear();
        return;
    }

    m_valueEdit->setText(index.data(SimulatorModel::ValueRole).toString());
    m_encodingEdit->setText(index.data(SimulatorModel::EncodingRole).toString());
    m_currentKey = index.data(SimulatorModel::KeyRole).toString();
    m_currentPpsPath = index.data(SimulatorModel::PpsPathRole).toString();
}
