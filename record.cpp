#include "record.h"

Record::Record() {
    this->recordName = "";
    this->encData = QByteArray();
}

Record::Record(QJsonObject object) {
    this->recordName = object["recordName"].toString();
    this->encData = QByteArray::fromHex(object["data"].toString().toUtf8());
}

QJsonObject Record::toJson() {
    QJsonObject result {
        {"recordName", recordName},
        {"data", QString(encData.toHex())}
    };
    return result;
}
