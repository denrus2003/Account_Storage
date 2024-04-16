#include "record_widget.h"

RecordRowWidget::RecordRowWidget(Record* record, int index, QWidget* parent)
    : QWidget(parent){
    this->record = record;
    this->index = index;
    numberLabel.setText(QString("%1").arg(index));
    nameLabel.setText(record->recordName);
    removeButton.setText("X");
    editButton.setText("Edit");
    numberLabel.setMaximumWidth(60);
    removeButton.setMaximumWidth(30);
    editButton.setMaximumWidth(30);
    QHBoxLayout* layout = new QHBoxLayout(this);
    layout->addWidget(&numberLabel);
    layout->addWidget(&nameLabel);
    layout->addWidget(&editButton);
    layout->addWidget(&removeButton);
    connect(&removeButton, &QPushButton::clicked, this, &RecordRowWidget::removeButtonClicked);
    connect(&editButton, &QPushButton::clicked, this, &RecordRowWidget::editButtonClicked);
    this->setLayout(layout);
}

void RecordRowWidget::updateText() {
    nameLabel.setText(record->recordName);
}

RecordRowWidget::~RecordRowWidget() {}

void RecordRowWidget::removeButtonClicked() { emit removeRecord(this); }

void RecordRowWidget::editButtonClicked() { emit editRecord(this); }
