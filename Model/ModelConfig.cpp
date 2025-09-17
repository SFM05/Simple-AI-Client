#include "Model/ModelConfig.h"
#include <QString>
#include <QUuid>

//ModelInf
ModelInf::ModelInf(const QString &provider_id,
                   const QString &id,
                   const QString &model,
                   const QString &name)
    : m_provider_id{provider_id}
    , m_id{id}
    , m_external_name{model}
    , m_display_name{name}
{}

void ModelInf::set_display_name(const QString &name)
{
    m_display_name = name;
}

void ModelInf::set_external_name(const QString &name)
{
    m_external_name = name;
}

QString ModelInf::provider_id() const
{
    return m_provider_id;
}

QString ModelInf::model_id() const
{
    return m_id;
}

QString ModelInf::external_model_name() const
{
    return m_external_name;
}

QString ModelInf::display_model_name() const
{
    //没有模型名的情况下默认使用供应商的名字
    if (m_display_name == "")
        return m_external_name;
    else
        return m_display_name;
}

//ProviderInf
ProviderInf::ProviderInf(const QString &id, const QString name, const QString url, const QString key)
    : m_id{id}
    , m_name{name}
    , m_url{url}
    , m_key{key}
{}

ProviderInf::ProviderInf(const QString &id,
                         const QString name,
                         const QString url,
                         const QString key,
                         const QList<QString> &supported_models)
    : m_id{id}
    , m_name{name}
    , m_url{url}
    , m_key{key}
    , m_supported_models{supported_models}
{}

void ProviderInf::set_name(const QString &name)
{
    m_name = name;
}

void ProviderInf::set_url(const QString &url)
{
    m_url = url;
}

void ProviderInf::set_key(const QString &key)
{
    m_key = key;
}

void ProviderInf::add_supported_model(const QString &model)
{
    m_supported_models.append(model);
}

void ProviderInf::delete_supported_model(const QString &model)
{
    m_supported_models.removeAll(model);
}

QString ProviderInf::provider_id() const
{
    return m_id;
}

QString ProviderInf::provider_name() const
{
    return m_name;
}

QString ProviderInf::url() const
{
    return m_url;
}

QString ProviderInf::key() const
{
    return m_key;
}

QList<QString> ProviderInf::supported_models() const
{
    return m_supported_models;
}
