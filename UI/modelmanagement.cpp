#include "modelmanagement.h"
#include "ui_modelmanagement.h"

#include <QDebug>
#include <QPushButton>
#include <QStandardItem>
#include <QStandardItemModel>

#include "Control/AiRegistry.h"
#include "Model/ModelConfig.h"

ModelManagement::ModelManagement(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::ModelManagement)
{
    ui->setupUi(this);
    setWindowTitle("模型管理");

    setup_provider_list();
    setup_model_list();

    ui->CancelProviderButton->setEnabled(false);
    ui->ConfirmProviderButton->setEnabled(false);
    ui->NewProviderButton->setEnabled(true);
    ui->DeleteProviderButton->setEnabled(false);

    connect(ui->CancelProviderButton,
            &QPushButton::clicked,
            this,
            &ModelManagement::onCancelProviderButtonClicked);
    connect(ui->ConfirmProviderButton,
            &QPushButton::clicked,
            this,
            &ModelManagement::onConfirmProviderButtonClicked);
    connect(ui->DeleteProviderButton,
            &QPushButton::clicked,
            this,
            &ModelManagement::onDeleteProviderButtonClicked);
    connect(ui->NewProviderButton,
            &QPushButton::clicked,
            this,
            &ModelManagement::onNewProviderButtonClicked);

    ui->CancelModelButton->setEnabled(false);
    ui->ConfirmModelButton->setEnabled(false);
    ui->NewModelButton->setEnabled(false);
    ui->DeleteModelButton->setEnabled(false);

    connect(ui->CancelModelButton,
            &QPushButton::clicked,
            this,
            &ModelManagement::onCancelModelButtonClicked);
    connect(ui->ConfirmModelButton,
            &QPushButton::clicked,
            this,
            &ModelManagement::onConfirmModelButtonClicked);
    connect(ui->DeleteModelButton,
            &QPushButton::clicked,
            this,
            &ModelManagement::onDeleteModelButtonClicked);
    connect(ui->NewModelButton,
            &QPushButton::clicked,
            this,
            &ModelManagement::onNewModelButtonClicked);
}

ModelManagement::~ModelManagement()
{
    delete ui;
}

void ModelManagement::setup_provider_list()
{
    //显式弃用以前的链接
    if (ui->ProviderList->selectionModel())
        disconnect(ui->ProviderList->selectionModel(),
                   &QItemSelectionModel::selectionChanged,
                   this,
                   &ModelManagement::onProviderListSelectionChanged);

    QStandardItemModel *provider = new QStandardItemModel(this);
    provider->clear();

    auto provider_list = AiRegistry::instance().all_providers();
    for (auto i : provider_list) {
        QStandardItem *item = new QStandardItem(i.second);
        item->setData(i.first, Qt::UserRole);
        provider->appendRow(item);
    }

    ui->ProviderList->setModel(provider);

    connect(ui->ProviderList->selectionModel(),
            &QItemSelectionModel::selectionChanged,
            this,
            &ModelManagement::onProviderListSelectionChanged);
}

void ModelManagement::refresh_provider_list()
{
    setup_provider_list();
}

void ModelManagement::setup_model_list(const QString &provider_id)
{
    //显式弃用以前的链接
    if (ui->ModelList->selectionModel())
        disconnect(ui->ModelList->selectionModel(),
                   &QItemSelectionModel::selectionChanged,
                   this,
                   &ModelManagement::onModelListSelectionChanged);

    QStandardItemModel *model = new QStandardItemModel(this);
    model->clear();

    if (provider_id != "") {
        auto model_list = AiRegistry::instance().all_models(provider_id);
        for (auto i : model_list) {
            QStandardItem *item = new QStandardItem(i.second);
            item->setData(i.first, Qt::UserRole);
            model->appendRow(item);
        }
    }

    ui->ModelList->setModel(model);

    connect(ui->ModelList->selectionModel(),
            &QItemSelectionModel::selectionChanged,
            this,
            &ModelManagement::onModelListSelectionChanged);
}

void ModelManagement::refresh_model_list(const QString &provider_id)
{
    setup_model_list(provider_id);
}

void ModelManagement::onProviderListSelectionChanged(const QItemSelection &selected,
                                                     const QItemSelection &deselected)
{
    Q_UNUSED(deselected);

    QModelIndexList indexes = ui->ProviderList->selectionModel()->selectedIndexes();

    if (!indexes.isEmpty()) {
        QModelIndex index = indexes.first();
        QString provider_id = index.data(Qt::UserRole).toString();

        ProviderInf &provider = AiRegistry::instance().get_provider(provider_id);
        QString name = provider.provider_name();
        QString url = provider.url();
        QString key = provider.key();

        ui->ProviderName->setText(name);
        ui->URL->setText(url);
        ui->Key->setText(key);
        refresh_model_list(provider_id);
    }

    ui->CancelProviderButton->setEnabled(true);
    ui->ConfirmProviderButton->setEnabled(true);
    ui->DeleteProviderButton->setEnabled(true);
    ui->NewProviderButton->setEnabled(false);

    ui->CancelModelButton->setEnabled(false);
    ui->ConfirmModelButton->setEnabled(false);
    ui->NewModelButton->setEnabled(true);
    ui->DeleteModelButton->setEnabled(false);
}

void ModelManagement::onModelListSelectionChanged(const QItemSelection &selected,
                                                  const QItemSelection &deselected)
{
    Q_UNUSED(deselected);

    QModelIndexList indexes = ui->ModelList->selectionModel()->selectedIndexes();

    if (!indexes.isEmpty()) {
        QModelIndex index = indexes.first();
        QString model_id = index.data(Qt::UserRole).toString();

        ModelInf &model = AiRegistry::instance().get_model(model_id);
        QString external_name = model.external_model_name();
        QString display_name = model.display_model_name();

        ui->ApiName->setText(external_name);
        ui->ModelName->setText(display_name);
    }

    ui->CancelModelButton->setEnabled(true);
    ui->ConfirmModelButton->setEnabled(true);
    ui->NewModelButton->setEnabled(false);
    ui->DeleteModelButton->setEnabled(true);
}

void ModelManagement::onCancelProviderButtonClicked()
{
    ui->ProviderList->selectionModel()->clearSelection();

    ui->ProviderName->setText("");
    ui->URL->setText("");
    ui->Key->setText("");

    ui->ApiName->setText("");
    ui->ModelName->setText("");

    refresh_model_list();

    ui->CancelProviderButton->setEnabled(false);
    ui->ConfirmProviderButton->setEnabled(false);
    ui->DeleteProviderButton->setEnabled(false);
    ui->NewProviderButton->setEnabled(true);


    ui->CancelModelButton->setEnabled(false);
    ui->ConfirmModelButton->setEnabled(false);
    ui->NewModelButton->setEnabled(false);
    ui->DeleteModelButton->setEnabled(false);
}

void ModelManagement::onConfirmProviderButtonClicked()
{
    QModelIndexList indexes = ui->ProviderList->selectionModel()->selectedIndexes();

    if (!indexes.isEmpty()) {
        QModelIndex index = indexes.first();
        QString provider_id = index.data(Qt::UserRole).toString();

        QString name = ui->ProviderName->text();
        QString url = ui->URL->text();
        QString key = ui->Key->text();

        ProviderInf &provider = AiRegistry::instance().get_provider(provider_id);
        provider.set_name(name);
        provider.set_url(url);
        provider.set_key(key);
    }

    refresh_provider_list();
    refresh_model_list();

    ui->ProviderName->setText("");
    ui->URL->setText("");
    ui->Key->setText("");

    ui->CancelProviderButton->setEnabled(false);
    ui->ConfirmProviderButton->setEnabled(false);
    ui->DeleteProviderButton->setEnabled(false);
    ui->NewProviderButton->setEnabled(true);

    ui->CancelModelButton->setEnabled(false);
    ui->ConfirmModelButton->setEnabled(false);
    ui->NewModelButton->setEnabled(false);
    ui->DeleteModelButton->setEnabled(false);

    emit ModelUpdate();
}

void ModelManagement::onDeleteProviderButtonClicked()
{
    QModelIndexList indexes = ui->ProviderList->selectionModel()->selectedIndexes();

    if (!indexes.isEmpty()) {
        QModelIndex index = indexes.first();
        QString provider_id = index.data(Qt::UserRole).toString();

        ui->ProviderList->selectionModel()->clearSelection();

        ui->ProviderName->setText("");
        ui->URL->setText("");
        ui->Key->setText("");

        AiRegistry::instance().delete_provider(provider_id);
    }

    refresh_provider_list();
    refresh_model_list();

    ui->CancelProviderButton->setEnabled(false);
    ui->ConfirmProviderButton->setEnabled(false);
    ui->DeleteProviderButton->setEnabled(false);
    ui->NewProviderButton->setEnabled(true);

    ui->CancelModelButton->setEnabled(false);
    ui->ConfirmModelButton->setEnabled(false);
    ui->NewModelButton->setEnabled(false);
    ui->DeleteModelButton->setEnabled(false);

    emit ModelUpdate();
}

void ModelManagement::onNewProviderButtonClicked()
{
    QString name = ui->ProviderName->text();
    QString url = ui->URL->text();
    QString key = ui->Key->text();

    AiRegistry::instance().add_provider(name, url, key);

    refresh_provider_list();
    refresh_model_list();

    ui->ProviderName->setText("");
    ui->URL->setText("");
    ui->Key->setText("");

    ui->CancelProviderButton->setEnabled(false);
    ui->ConfirmProviderButton->setEnabled(false);
    ui->DeleteProviderButton->setEnabled(false);
    ui->NewProviderButton->setEnabled(true);


    ui->CancelModelButton->setEnabled(false);
    ui->ConfirmModelButton->setEnabled(false);
    ui->NewModelButton->setEnabled(false);
    ui->DeleteModelButton->setEnabled(false);

    emit ModelUpdate();
}

void ModelManagement::onCancelModelButtonClicked()
{
    ui->ModelList->selectionModel()->clearSelection();

    ui->ApiName->setText("");
    ui->ModelName->setText("");

    ui->CancelModelButton->setEnabled(false);
    ui->ConfirmModelButton->setEnabled(false);
    ui->NewModelButton->setEnabled(true);
    ui->DeleteModelButton->setEnabled(false);
}

void ModelManagement::onConfirmModelButtonClicked()
{
    QModelIndexList indexes = ui->ModelList->selectionModel()->selectedIndexes();

    if (!indexes.isEmpty()) {
        QModelIndex index = indexes.first();
        QString model_id = index.data(Qt::UserRole).toString();

        QString display_name = ui->ModelName->text();
        QString external_name = ui->ApiName->text();

        ModelInf &model = AiRegistry::instance().get_model(model_id);
        model.set_display_name(display_name);
        model.set_external_name(external_name);

    }

    indexes = ui->ProviderList->selectionModel()->selectedIndexes();
    QModelIndex index = indexes.first();
    QString provider_id = index.data(Qt::UserRole).toString();
    refresh_model_list(provider_id);

    ui->ApiName->setText("");
    ui->ModelName->setText("");

    ui->CancelModelButton->setEnabled(false);
    ui->ConfirmModelButton->setEnabled(false);
    ui->DeleteModelButton->setEnabled(false);
    ui->NewModelButton->setEnabled(true);

    emit ModelUpdate();
}

void ModelManagement::onNewModelButtonClicked() {
    QString model_name = ui->ModelName->text();
    QString external_name = ui->ApiName->text();

    QModelIndexList indexes = ui->ProviderList->selectionModel()->selectedIndexes();
    QModelIndex index = indexes.first();
    QString provider_id = index.data(Qt::UserRole).toString();

    AiRegistry::instance().add_model(provider_id,external_name,model_name);

    refresh_model_list(provider_id);

    ui->ApiName->setText("");
    ui->ModelName->setText("");

    ui->CancelModelButton->setEnabled(false);
    ui->ConfirmModelButton->setEnabled(false);
    ui->DeleteModelButton->setEnabled(false);
    ui->NewModelButton->setEnabled(true);

    emit ModelUpdate();
}

void ModelManagement::onDeleteModelButtonClicked() {
    QModelIndexList indexes = ui->ModelList->selectionModel()->selectedIndexes();
    QModelIndex index = indexes.first();
    QString model_id = index.data(Qt::UserRole).toString();
    refresh_model_list(model_id);

    AiRegistry::instance().delete_model(model_id);

    indexes = ui->ProviderList->selectionModel()->selectedIndexes();
    index = indexes.first();
    QString provider_id = index.data(Qt::UserRole).toString();

    refresh_model_list(provider_id);

    ui->ApiName->setText("");
    ui->ModelName->setText("");

    ui->CancelModelButton->setEnabled(false);
    ui->ConfirmModelButton->setEnabled(false);
    ui->DeleteModelButton->setEnabled(false);
    ui->NewModelButton->setEnabled(true);

    emit ModelUpdate();
}
