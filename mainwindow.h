#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSystemTrayIcon>
#include <QPointer>
#include <QtGui>
#include <QWidget>
#include <QLabel>
#include <QtGlobal>
#include <QDebug>
#include <QCoreApplication>
#include <QFileDialog>
#include <QTranslator>

#include "tsumanager.h"
#include "downloadwindow.h"
#include "searchwindow.h"
#include "settingswindow.h"
#include "statisticswindow.h"

#include "libtorrent/add_torrent_params.hpp"
#include "libtorrent/torrent_handle.hpp"
#include "libtorrent/bencode.hpp"
#include "libtorrent/torrent_info.hpp"

#include <changelog.h>
#include <QcGaugeWidget/qcgaugewidget.h>

namespace Ui
{
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    tsuManager *sessionManager;
    QPointer<QSystemTrayIcon> m_systrayIcon;

signals:
    void stopSessionManager();

public slots:
    void updateStatusBarStatistics(const QString & msg);
    void updateStatusBarLabel(const QString & msg);
//    void updateGauge(const float &downValue, const float &upValue);
    void popupInfo(const QString & msg);

    void externalIpAssigned();
    void dhtBootstrapExecuted();
    void listenerUpdate(QString type, bool success);

    void sessionStatisticUpdate(const quint64 &sent, const quint64 &received);

private slots:
    void toggleVisibility(QSystemTrayIcon::ActivationReason e = QSystemTrayIcon::Trigger);
    void balloonClicked();

    void btnMenuClick();
    void btnAddClick();
    void loadLanguage();

private:
    Ui::MainWindow *ui;
    settingswindow *settingsPage;
    searchwindow *searchPage;
    downloadwindow *downloadPage;
    statisticswindow *statisticsPage;
    QLabel *statusLabel;

    QTranslator p_translator;

    QThread *p_session_thread;

    QcGaugeWidget *p_gauge;
    QcDegreesItem *p_gaugeDegreesItem;
    QcValuesItem *p_gaugeValuesItem;
    QcNeedleItem *p_gaugeDownNeedle;
    QcNeedleItem *p_gaugeUpNeedle;
    QcLabelItem *p_gaugeUnitLabel;
    QcLabelItem *p_gaugeSpeedLabel;

    void createTrayIcon();
    void initializeScreen();

    void readSettings();
    void writeSettings();

    void updateGauge(const float &downValue, const float &upValue);

    QString convertSize(const int &size);
    QString convertSizeUnit(const int &size);

    // QWidget interface
protected:
    void closeEvent(QCloseEvent *event) override;
    void changeEvent(QEvent *e) override;

};

#endif // MAINWINDOW_H
