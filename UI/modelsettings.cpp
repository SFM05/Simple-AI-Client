#include "modelsettings.h"
#include "ui_modelsettings.h"

ModelSettings::ModelSettings(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::ModelSettings)
{
    ui->setupUi(this);
}

ModelSettings::~ModelSettings()
{
    delete ui;
}
