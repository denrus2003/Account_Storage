#ifndef RECORDROWWIDGET_H
#define RECORDROWWIDGET_H

#include "record.h"

#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QHBoxLayout>
#include <QSizePolicy>

class RecordRowWidget : public QWidget {
    Q_OBJECT
public:
    RecordRowWidget(Record* record, int index, QWidget* parent = nullptr);
    ~RecordRowWidget();
    void updateText();
private slots:
    void removeButtonClicked();
    void editButtonClicked();
signals:
    void removeRecord(RecordRowWidget* self);
    void editRecord(RecordRowWidget* self);
public:
    int index;
    Record* record;
    QLabel numberLabel, nameLabel;
    QPushButton removeButton, editButton;
};

#endif // RECORDROWWIDGET_H
