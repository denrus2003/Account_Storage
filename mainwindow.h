#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <openssl/evp.h>
#include <QString>
#include <QByteArray>
#include <QCryptographicHash>
#include <QFile>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>
#include <QMessageBox>
#include <QClipboard>

#include "record.h"
#include "record_widget.h"

enum State {
    LogIn, ViewRecords, ReadRecord, SaveFile, NewRecord, EditRecordDecrypt, EditRecordEncrypt, ChangePin, ResetPin
};

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
private slots:
    void removeRecordRowClicked(RecordRowWidget* widget);
    void editRecordRowClicked(RecordRowWidget* widget);

    void on_passLineEdit_returnPressed();
    void on_passPushButton_clicked();
    void on_searchLineEdit_textEdited(const QString &arg1);
    void on_recordListWidget_activated(const QModelIndex &index);
    void on_newRecordPushButton_clicked();
    void on_savePushButton_clicked();
    void on_cancelRecordPushButton_clicked();
    void on_saveRecordPushButton_clicked();

private:
    void filterRows(QString filter = "");
    void ListWidget();
    void addRecordWidget(Record* record, int index);

    int readFile(QByteArray &hash, QJsonDocument &document, QJsonParseError &error);
    void writeFile(QByteArray &hash);
    void addNewRecord(QByteArray &hash);
    void setFieldBeforeRecordChange(QByteArray &hash);
    void changeSelectedRecord(QByteArray &hash);
    void parseFileJson(QByteArray &hash);
    int encryptData(QByteArray &key, QByteArray &inputBytes, QByteArray &outputBytes);
    int decryptData(QByteArray &key, QByteArray &inputBytes, QByteArray &outputBytes);

    void finishLogin();
    void LoadPin();
    bool verifyPassword(QByteArray &hash);
    void readRecord(QByteArray &hash);
    Ui::MainWindow *ui;
    QList<Record> records;
    State state;
    int selectedRecord;
};

#endif // MAINWINDOW_H
