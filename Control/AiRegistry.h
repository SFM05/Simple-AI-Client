#ifndef AIREGISTRY_H
#define AIREGISTRY_H

#include <QHash>
#include <QList>
#include <QPair>
#include "Model/ModelConfig.h"

class AiRegistry
{
private:
    QHash<QString, ModelInf> m_models;
    QHash<QString, ProviderInf> m_providers;

    AiRegistry() = default;
    ~AiRegistry() = default;

public:
    static AiRegistry &instance();

    AiRegistry(const AiRegistry &) = delete;
    AiRegistry &operator=(const AiRegistry &) = delete;

    bool load_from_file(const QString &);
    bool save_to_file(const QString &);

    QString add_provider(const QString &name, const QString &url, const QString &key);
    void add_model(const QString &provider_id,
                   const QString &external_model_name,
                   const QString &display_model_name="");
    void delete_provider(const QString &id);
    void delete_model(const QString &id);

    ModelInf &get_model(const QString &);
    ProviderInf &get_provider(const QString &);
    QList<QPair<QString, QString>> all_providers() const;
    QList<QPair<QString, QString>> all_models(const QString &) const;
};

#endif // AIREGISTRY_H
