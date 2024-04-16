#ifndef RECORD_H
#define RECORD_H

#include <QString>
#include <QByteArray>
#include <QJsonObject>
#include <QJsonDocument>

struct Record {
    Record();
    Record(QJsonObject object);
    QJsonObject toJson();

    QString recordName;
    QByteArray encData;
};

#endif // RECORD_H
