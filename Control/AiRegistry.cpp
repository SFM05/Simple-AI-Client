#include "Control/AiRegistry.h"
#include <QHash>
#include <QList>
#include <QPair>
#include <QUuid>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QVariantMap>
#include "Model/ModelConfig.h"

AiRegistry &AiRegistry::instance()
{
    static AiRegistry instance;
    return instance;
}




bool AiRegistry::save_to_file(const QString &file_path)
{
    QJsonObject root;

    // 保存 providers
    QJsonArray providersArray;
    for (const auto &provider : m_providers) {
        QJsonObject obj;
        obj["id"] = provider.provider_id();
        obj["name"] = provider.provider_name();
        obj["url"] = provider.url();
        obj["key"] = provider.key();

        QJsonArray modelsArray;
        for (const auto &modelId : provider.supported_models()) {
            modelsArray.append(modelId);
        }
        obj["supported_models"] = modelsArray;

        providersArray.append(obj);
    }
    root["providers"] = providersArray;

    // 保存 models
    QJsonArray modelsArray;
    for (const auto &model : m_models) {
        QJsonObject obj;
        obj["provider_id"] = model.provider_id();
        obj["id"] = model.model_id();
        obj["external_name"] = model.external_model_name();
        obj["display_name"] = model.display_model_name();
        modelsArray.append(obj);
    }
    root["models"] = modelsArray;

    QFile file(file_path);
    if (!file.open(QIODevice::WriteOnly)) {
        return false;
    }

    QJsonDocument doc(root);
    file.write(doc.toJson());
    file.close();
    return true;
}

bool AiRegistry::load_from_file(const QString &file_path)
{
    QFile file(file_path);
    if (!file.open(QIODevice::ReadOnly)) {
        return false;
    }

    QByteArray data = file.readAll();
    file.close();

    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(data, &error);
    if (error.error != QJsonParseError::NoError) {
        return false;
    }

    QJsonObject root = doc.object();

    // 清空现有数据
    m_providers.clear();
    m_models.clear();

    // 加载 providers
    QJsonArray providersArray = root["providers"].toArray();
    for (const auto &value : providersArray) {
        QJsonObject obj = value.toObject();
        QString id = obj["id"].toString();
        QString name = obj["name"].toString();
        QString url = obj["url"].toString();
        QString key = obj["key"].toString();

        QList<QString> supportedModels;
        QJsonArray modelsArray = obj["supported_models"].toArray();
        for (const auto &modelValue : modelsArray) {
            supportedModels.append(modelValue.toString());
        }

        ProviderInf provider(id, name, url, key, supportedModels);
        m_providers.insert(id, provider);
    }

    // 加载 models
    QJsonArray modelsArray = root["models"].toArray();
    for (const auto &value : modelsArray) {
        QJsonObject obj = value.toObject();
        QString provider_id = obj["provider_id"].toString();
        QString id = obj["id"].toString();
        QString external_name = obj["external_name"].toString();
        QString display_name = obj["display_name"].toString();

        ModelInf model(provider_id, id, external_name, display_name);
        m_models.insert(id, model);
    }

    return true;
}




QString AiRegistry::add_provider(const QString &name, const QString &url, const QString &key)
{
    QUuid uuid = QUuid::createUuid();
    QString id = uuid.toString().remove("{").remove("}");
    ProviderInf provider{id, name, url, key};
    m_providers.insert(id, provider);
    return id;
}

void AiRegistry::add_model(const QString &provider_id,
                           const QString &external_model_name,
                           const QString &display_model_name)
{
    QUuid uuid = QUuid::createUuid();
    QString id = uuid.toString().remove("{").remove("}");
    ModelInf model{provider_id, id, external_model_name, display_model_name};
    m_models.insert(id, model);
    m_providers[provider_id].add_supported_model(id);
}

void AiRegistry::delete_provider(const QString &id)
{
    QList<QString> models_to_delete = m_providers[id].supported_models();
    for (auto model_id : models_to_delete) {
        m_models.remove(model_id);
    }
    m_providers.remove(id);
}

void AiRegistry::delete_model(const QString &id)
{
    QString provider_id = m_models[id].provider_id();
    m_models.remove(id);
    m_providers[provider_id].delete_supported_model(id);
}

ModelInf &AiRegistry::get_model(const QString &id)
{
    return m_models[id];
}

ProviderInf &AiRegistry::get_provider(const QString &id)
{
    return m_providers[id];
}

QList<QPair<QString, QString>> AiRegistry::all_providers() const
{
    QList<QPair<QString, QString>> providers_list;
    QList<ProviderInf> providers = m_providers.values();
    for (auto i : providers) {
        providers_list.append(qMakePair(i.provider_id(), i.provider_name()));
    }
    return providers_list;
}

QList<QPair<QString, QString>> AiRegistry::all_models(const QString &id) const
{
    QList<QPair<QString, QString>> models_list;
    QList<QString> models = m_providers[id].supported_models();
    for (QString i : models) {
        QString model_name = m_models[i].display_model_name();
        models_list.append(qMakePair(i, model_name));
    }
    return models_list;
}
