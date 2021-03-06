#include "mainwindow.h"

#include <QApplication>
#include <QSplashScreen>
#include <QtDebug>
#include <QFile>
#include <QTextStream>
#include <QSettings>
#include <QStandardPaths>

#include <tsumanager.h>
#include <settingswindow.h>
#include <updatemanager.h>

void logMessageHandler(QtMsgType type, const QMessageLogContext & context, const QString & msg)
{
    QString txt;

    QString txtContext = QString("");
    QString funcName = QString::fromUtf8(context.function);
    QString message = QString(msg);

    // strip out all chars after '(' like '(class QWidget *)' from function name
    funcName = funcName.left(funcName.indexOf("("));

    // strip out all chars like 'void __cdecl ' before function name
    funcName = funcName.right(funcName.length() - funcName.lastIndexOf(" ") - 1);

    // strip out double quotes around string message (if present)
    if (message.left(1) == "\"") {
        message = message.right(message.length() - 1);

        if (message.right(1) == "\"") {
            message = message.left(message.length() - 1);
        }
    }

    txtContext = QString("%0::%1").arg(funcName).arg(context.line);

    //if (QLibraryInfo::isDebugBuild()) {
    //    txtContext = QString("%0::%1::%2").arg(context.file).arg(context.function).arg(context.line);
    //}

    QString date = QDateTime::currentDateTime().toString(Qt::SystemLocaleDate);

    QSettings settings(qApp->property("iniFilePath").toString(), QSettings::IniFormat);
    int debugLevel = settings.value("Debug/Level", 1).toInt();

    bool shouldLog = false;
    switch (debugLevel) {
    case 0: // debug
        shouldLog = true;
        break;
    case 1: // info
        if (type != QtDebugMsg)
            shouldLog = true;
        break;
    case 2: // warning
        if (type == QtWarningMsg || type == QtCriticalMsg || type == QtFatalMsg)
            shouldLog = true;
        break;
    case 3: // critical
        if (type == QtCriticalMsg || type == QtFatalMsg)
            shouldLog = true;
        break;
    case 4: // fatal
        if (type == QtFatalMsg)
            shouldLog = true;
        break;
    default:
        // info
        shouldLog = true;
        break;
    }

    switch (type) {
    case QtDebugMsg:
        txt = "Debug   ";
        break;
    case QtInfoMsg:
        txt = "Info    ";
        break;
    case QtWarningMsg:
        txt = "Warning ";
        break;
    case QtCriticalMsg:
        txt = "Critical";
        break;
    case QtFatalMsg:
        txt = "Fatal   ";
        break;
    }

    if (shouldLog) {
        txt = QString("%0 - %1 - %2 -> %3").arg(date).arg(txt).arg(txtContext).arg(message);
        QFile outFile("Tsunami.log");
        outFile.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text);
        QTextStream ts(&outFile);
        ts << txt << endl;
        QTextStream out(stdout);
        out << txt << endl;
    }
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    qRegisterMetaType<tsuEvents::tsuEvent>();
    qRegisterMetaType<std::string>();
    qRegisterMetaType<QVector<tsuEvents::tsuEvent>>();
    qRegisterMetaType<QPair<int,int>>();

    // https://stackoverflow.com/questions/4954140/how-to-redirect-qdebug-qwarning-qcritical-etc-output
    qInstallMessageHandler(logMessageHandler);

    // REMOVED TO LET INI FILE TO BE PLACE IN CORRECT FOLDER (...local/Tsunami INSTEAD OF ...local/Adunanza/Tsunami)
//    QApplication::setOrganizationName(QStringLiteral(APP_ORGANIZATION_NAME));

    QApplication::setOrganizationDomain(QStringLiteral(APP_ORGANIZATION_DOMAIN));
    QApplication::setApplicationName(QStringLiteral(APP_PROJECT_NAME));

//    QSettings settings(QSettings::IniFormat, QSettings::SystemScope, QStringLiteral(APP_PROJECT_NAME), QStringLiteral(APP_PROJECT_NAME));
    QString filePath = QString("%0/%1.ini").arg(QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation))
                                           .arg(QStringLiteral(APP_PROJECT_NAME));
    filePath = QDir::toNativeSeparators(filePath);
    a.setProperty("iniFilePath", filePath); // QApplication property are application wide qApp->property("")

    QSettings settings(filePath, QSettings::IniFormat);
    bool justUpdated = settings.value("justUpdated", false).toBool();

    updatemanager *um = new updatemanager();

    if (!justUpdated) {
        qDebug("checking for update");
        um->checkUpdate();

        while (!um->isFinished()) {
            a.processEvents();
        }
    } else {
        um->showSplashScreen(3000);
    }

    if (um->appNeedRestart()) {
        settings.setValue("justUpdated", true);
        qDebug("restarting");

        // due to squirrel standard behaviour, qApp->arguments()[0] returs path to a tsunami version older than one just downloaded
        // retrieve path of tsunami.exe loader outside current path and launch
        QString fileName = QFileInfo(QCoreApplication::applicationFilePath()).fileName();
        QDir oldPath(QCoreApplication::applicationDirPath());
        oldPath.cdUp();
        QString newPath = oldPath.filePath(fileName);

        qApp->quit();
//        QProcess::startDetached(qApp->arguments()[0], qApp->arguments());
        QProcess::startDetached(newPath, qApp->arguments());
        return 0;
    } else {
        qDebug("showing main window");
        MainWindow w;
        w.show();
        return a.exec();
    }
}
