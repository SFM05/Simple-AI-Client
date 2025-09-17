#ifndef MODELSETTINGS_H
#define MODELSETTINGS_H

#include <QWidget>

namespace Ui {
class ModelSettings;
}

class ModelSettings : public QWidget
{
    Q_OBJECT

public:
    explicit ModelSettings(QWidget *parent = nullptr);
    ~ModelSettings();

private:
    Ui::ModelSettings *ui;
};

#endif // MODELSETTINGS_H
