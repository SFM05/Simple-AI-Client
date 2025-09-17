#ifndef MODELMANAGEMENT_H
#define MODELMANAGEMENT_H

#include <QWidget>
#include <QItemSelection>

namespace Ui {
class ModelManagement;
}

class ModelManagement : public QWidget
{
    Q_OBJECT

public:
    explicit ModelManagement(QWidget *parent = nullptr);
    ~ModelManagement();

    void refresh_provider_list();
    void refresh_model_list(const QString &provider_id="");

signals:
    void ModelUpdate();

private slots:
    void onProviderListSelectionChanged(const QItemSelection &selected,
                                        const QItemSelection &deselected);
    void onModelListSelectionChanged(const QItemSelection &selected,
                                     const QItemSelection &deselected);

    void onCancelProviderButtonClicked();
    void onConfirmProviderButtonClicked();
    void onNewProviderButtonClicked();
    void onDeleteProviderButtonClicked();

    void onCancelModelButtonClicked();
    void onConfirmModelButtonClicked();
    void onNewModelButtonClicked();
    void onDeleteModelButtonClicked();

private:
    Ui::ModelManagement *ui;

    void setup_provider_list();
    void setup_model_list(const QString &provider_id="");
};

#endif // MODELMANAGEMENT_H
