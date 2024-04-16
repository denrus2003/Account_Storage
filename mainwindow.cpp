#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <openssl/evp.h>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);
    state = State::LogIn;
    ui->stackedWidget->setCurrentIndex(0);
    selectedRecord = -1;
}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::addRecordWidget(Record *record, int index) {
    RecordRowWidget *row = new RecordRowWidget(record, index, ui->recordListWidget);
    QListWidgetItem *item = new QListWidgetItem(ui->recordListWidget);
    item->setSizeHint(row->minimumSizeHint());
    ui->recordListWidget->addItem(item);
    ui->recordListWidget->setItemWidget(item, row);
    connect(row, &RecordRowWidget::removeRecord, this, &MainWindow::removeRecordRowClicked);
    connect(row, &RecordRowWidget::editRecord, this, &MainWindow::editRecordRowClicked);
}

void MainWindow::ListWidget() {
    for (int i = 0; i < records.size(); i++)
        addRecordWidget(&records[i], i);
}

void MainWindow::filterRows(QString filter) {
    for (int i = 0; i < records.size(); i++)
        this->ui->recordListWidget->item(i)->setHidden(!records[i].recordName.contains(filter, Qt::CaseInsensitive));
}

void MainWindow::removeRecordRowClicked(RecordRowWidget *widget) {
    records.remove(widget->index);
    ui->recordListWidget->clear();
    ListWidget();
}

void MainWindow::editRecordRowClicked(RecordRowWidget *widget) {
    selectedRecord = widget->index;
    state = State::EditRecordDecrypt;
    ui->stackedWidget->setCurrentIndex(0);
}

void MainWindow::parseFileJson(QByteArray &hash) {
    QJsonDocument document;
    QJsonParseError error;
    readFile(hash, document, error);
    QJsonArray array = document.object()["records"].toArray();
    for (int i = 0; i < array.size(); i++)
        records.append(Record(array[i].toObject()));
    ListWidget();
}

int MainWindow::readFile(QByteArray &hash, QJsonDocument &document, QJsonParseError &error) {
    QFile file("records.json");
    if (!file.open(QIODevice::ReadOnly)) {
        qDebug() << file.errorString();
        return 1;
    }
    QByteArray hexEncData = file.readAll();
    QByteArray encData = QByteArray::fromHex(hexEncData);
    QByteArray unEncData;
    int errorcode = decryptData(hash, encData, unEncData);
    if (errorcode) {
        qDebug() << "Error when decrypting! Errorcode: " << errorcode;
        return 1;
    }
    document = QJsonDocument::fromJson(unEncData, &error);
    return 0;
}

void MainWindow::writeFile(QByteArray &hash) {
    QJsonObject object;
    QJsonArray array;
    for (int i = 0; i < records.size(); i++) {
        QJsonObject jsonRecord = records[i].toJson();
        array.append(jsonRecord);
    }
    object["records"] = array;
    QFile file("records.json");
    if (!file.open(QIODevice::WriteOnly)) {
        qDebug() << file.errorString();
        return;
    }
    QByteArray unEncData = QJsonDocument(object).toJson();
    QByteArray encData;
    int errorcode = encryptData(hash, unEncData, encData);
    if (errorcode) {
        qDebug() << "Error when encrypting! Errorcode: " << errorcode;
        return;
    }
    QByteArray hexEncData = encData.toHex();
    file.write(hexEncData);
    file.close();
}

int MainWindow::encryptData(QByteArray &keyBytes, QByteArray &inputBytes, QByteArray &outputBytes) {
    QByteArray iv_qba = QByteArray::fromHex("b48d1d3c947c425b07b3de8bf6d96e84");
    QDataStream decryptedStream(inputBytes);
    QDataStream encryptedStream(&outputBytes, QIODevice::ReadWrite);
    const int bufferLen = 256;
    int encryptedLen, decryptedLen;
    unsigned char key[32], iv[16];
    unsigned char encryptedBuffer[bufferLen] = {0}, decryptedBuffer[bufferLen] = {0};

    memcpy(key, keyBytes.data(), 32);
    memcpy(iv, iv_qba.data(), 16);
    iv_qba.clear();

    EVP_CIPHER_CTX *ctx;
    ctx = EVP_CIPHER_CTX_new();
    if (!EVP_EncryptInit_ex2(ctx, EVP_aes_256_cbc(), key, iv, NULL)) {
        EVP_CIPHER_CTX_free(ctx);
        return 1;
    }
    do {
        decryptedLen = decryptedStream.readRawData(reinterpret_cast<char*>(decryptedBuffer), bufferLen);
        if (!EVP_EncryptUpdate(ctx, encryptedBuffer, &encryptedLen, decryptedBuffer, decryptedLen)) {
            EVP_CIPHER_CTX_free(ctx);
            return 2;
        }
        encryptedStream.writeRawData(reinterpret_cast<char*>(encryptedBuffer), encryptedLen);
    } while (decryptedLen > 0);
    if (!EVP_EncryptFinal_ex(ctx, encryptedBuffer, &encryptedLen)) {
        EVP_CIPHER_CTX_free(ctx);
        return 3;
    }
    encryptedStream.writeRawData(reinterpret_cast<char*>(encryptedBuffer), encryptedLen);
    EVP_CIPHER_CTX_free(ctx);
    return 0;
}

int MainWindow::decryptData(QByteArray &keyBytes, QByteArray &inputBytes, QByteArray &outputBytes) {
    QByteArray iv_qba = QByteArray::fromHex("b48d1d3c947c425b07b3de8bf6d96e84");
    QDataStream encryptedStream(inputBytes);
    QDataStream decryptedStream(&outputBytes, QIODevice::ReadWrite);
    const int bufferLen = 256;
    int encryptedLen, decryptedLen, tmplen;
    unsigned char key[32], iv[16];
    unsigned char encryptedBuffer[bufferLen] = {0}, decryptedBuffer[bufferLen] = {0};

    memcpy(key, keyBytes.data(), 32);
    memcpy(iv, iv_qba.data(), 16);
    iv_qba.clear();

    EVP_CIPHER_CTX *ctx;
    ctx = EVP_CIPHER_CTX_new();
    if (!EVP_DecryptInit_ex2(ctx, EVP_aes_256_cbc(), key, iv, NULL)) {
        EVP_CIPHER_CTX_free(ctx);
        return 1;
    }
    do {
        encryptedLen = encryptedStream.readRawData(reinterpret_cast<char*>(encryptedBuffer), bufferLen);
        if (!EVP_DecryptUpdate(ctx, decryptedBuffer, &decryptedLen, encryptedBuffer, encryptedLen)) {
            EVP_CIPHER_CTX_free(ctx);
            return 2;
        }
        decryptedStream.writeRawData(reinterpret_cast<char*>(decryptedBuffer), decryptedLen);
    } while (encryptedLen > 0);

    if (!EVP_DecryptFinal_ex(ctx, decryptedBuffer, &decryptedLen)) {
        EVP_CIPHER_CTX_free(ctx);
        return 3;
    }
    decryptedStream.writeRawData(reinterpret_cast<char*>(decryptedBuffer), decryptedLen);
    EVP_CIPHER_CTX_free(ctx);
    return 0;
}

void MainWindow::LoadPin() {
    QByteArray hash = QCryptographicHash::hash(ui->passLineEdit->text().toUtf8(), QCryptographicHash::Sha256);
    qDebug() << "Hash: " << hash.toHex();
    ui->passLineEdit->setText(QString().fill('*', ui->passLineEdit->text().size()));
    ui->passLineEdit->clear();
    if (!verifyPassword(hash)) {
        QMessageBox warning;
        warning.setText("Неверный мастер-пароль, попробуйте еще раз");
        warning.exec();
        return;
    }
    if (state == State::LogIn) {
        parseFileJson(hash);
        ui->stackedWidget->setCurrentIndex(1);
        state = State::ViewRecords;
    }
    else if (state == State::ReadRecord) {
        readRecord(hash);
        ui->stackedWidget->setCurrentIndex(1);
        state = State::ViewRecords;
    }
    else if (state == State::SaveFile) {
        writeFile(hash);
        ui->stackedWidget->setCurrentIndex(1);
        state = State::ViewRecords;
    }
    else if (state == State::NewRecord) {
        addNewRecord(hash);
        ui->stackedWidget->setCurrentIndex(1);
        state = State::ViewRecords;
    }
    else if (state == State::EditRecordDecrypt) {
        setFieldBeforeRecordChange(hash);
        ui->stackedWidget->setCurrentIndex(2);
        state = State::EditRecordEncrypt;
    }
    else if (state == State::EditRecordEncrypt) {
        changeSelectedRecord(hash);
        ui->stackedWidget->setCurrentIndex(1);
        state = State::ViewRecords;
    }
    hash.setRawData(const_cast<char*>( QByteArray().fill('*', 32).data() ), 32);
}

void MainWindow::setFieldBeforeRecordChange(QByteArray &hash) {
    QByteArray decryptedData;
    int errorcode = decryptData(hash, records[selectedRecord].encData, decryptedData);
    if (errorcode) {
        qDebug() << "Error when decrypting record " << selectedRecord << "! Errorcode: " << errorcode;
        return;
    }
    QJsonDocument document = QJsonDocument::fromJson(decryptedData);
    QJsonObject object = document.object();
    ui->recordNameLineEdit->setText(records[selectedRecord].recordName);
    ui->recordLoginLineEdit->setText(object["username"].toString());
    ui->recordPasswordLineEdit->setText(object["password"].toString());
    decryptedData.setRawData(const_cast<char*>( QByteArray().fill('*', decryptedData.size()).data() ), decryptedData.size());
}



void MainWindow::changeSelectedRecord(QByteArray &hash) {
    QJsonObject data;
    data["username"] = ui->recordLoginLineEdit->text();
    data["password"] = ui->recordPasswordLineEdit->text();
    QByteArray decryptedBytes = QJsonDocument(data).toJson();
    QByteArray encryptedBytes;
    int errorcode = encryptData(hash, decryptedBytes, encryptedBytes);
    if (errorcode) {
        qDebug() << "Error when encrypting new record! Errorcode: " << errorcode;
        return;
    }
    records[selectedRecord].recordName = ui->recordNameLineEdit->text();
    records[selectedRecord].encData = encryptedBytes;
    decryptedBytes.setRawData(const_cast<char*>( QByteArray().fill('*', decryptedBytes.size()).data() ), decryptedBytes.size());
    encryptedBytes.setRawData(const_cast<char*>( QByteArray().fill('*', encryptedBytes.size()).data() ), encryptedBytes.size());
    ui->recordListWidget->clear();
    ListWidget();
}

bool MainWindow::verifyPassword(QByteArray &hash) {
    QJsonDocument document;
    QJsonParseError error;
    if (readFile(hash, document, error))
        return false;
    if (error.error != QJsonParseError::NoError) {
        qDebug() << "Error! " << error.errorString();
        return false;
    }
    return true;
}

void MainWindow::readRecord(QByteArray &hash) {
    QByteArray decryptedData;
    int errorcode = decryptData(hash, records[selectedRecord].encData, decryptedData);
    if (errorcode) {
        qDebug() << "Error when decrypting record " << selectedRecord << "! Errorcode: " << errorcode;
        return;
    }
    qDebug() << decryptedData;
    QClipboard *clipboard = QGuiApplication::clipboard();
    clipboard->setText(QString(decryptedData));
    decryptedData.setRawData(const_cast<char*>( QByteArray().fill('*', decryptedData.size()).data() ), decryptedData.size());
}

void MainWindow::addNewRecord(QByteArray &hash) {
    QJsonObject data;
    data["username"] = ui->recordLoginLineEdit->text();
    data["password"] = ui->recordPasswordLineEdit->text();
    QByteArray decryptedBytes = QJsonDocument(data).toJson();
    QByteArray encryptedBytes;
    int errorcode = encryptData(hash, decryptedBytes, encryptedBytes);
    if (errorcode) {
        qDebug() << "Error when encrypting new record! Errorcode: " << errorcode;
        return;
    }
    QJsonObject record;
    record["recordName"] = ui->recordNameLineEdit->text();
    record["data"] = QString(encryptedBytes.toHex());
    records.append(Record(record));
    addRecordWidget(&records.last(), records.size() - 1);
}

void MainWindow::on_passLineEdit_returnPressed() {
    LoadPin();
}

void MainWindow::on_passPushButton_clicked() {
    LoadPin();
}

void MainWindow::on_searchLineEdit_textEdited(const QString &arg1) {
    filterRows(arg1);
}

void MainWindow::on_recordListWidget_activated(const QModelIndex &index) {
    selectedRecord = index.row();
    state = State::ReadRecord;
    ui->stackedWidget->setCurrentIndex(0);
}

void MainWindow::on_newRecordPushButton_clicked() {
    state = State::NewRecord;
    ui->stackedWidget->setCurrentIndex(2);
}

void MainWindow::on_savePushButton_clicked() {
    state = State::SaveFile;
    ui->stackedWidget->setCurrentIndex(0);
}

void MainWindow::on_cancelRecordPushButton_clicked() {
    state = State::ViewRecords;
    ui->recordNameLineEdit->setText(QString().fill('*', ui->recordNameLineEdit->text().size()));
    ui->recordNameLineEdit->clear();
    ui->recordLoginLineEdit->setText(QString().fill('*', ui->recordLoginLineEdit->text().size()));
    ui->recordLoginLineEdit->clear();
    ui->recordPasswordLineEdit->setText(QString().fill('*', ui->recordPasswordLineEdit->text().size()));
    ui->recordPasswordLineEdit->clear();
    ui->stackedWidget->setCurrentIndex(1);
}

void MainWindow::on_saveRecordPushButton_clicked() {
    qDebug() << state;
    ui->stackedWidget->setCurrentIndex(0);
}

