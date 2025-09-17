#ifndef MODELCONFIG_H
#define MODELCONFIG_H

#include <QList>
#include <QMap>
#include <QString>

class ModelInf
{
private:
    QString m_provider_id; //供应商id，不可随意更改
    QString m_id;          //模型id，不可更改
    QString m_external_name;       //调用api的模型名
    QString m_display_name;        //显示给用户的模型名

public:
    ModelInf() = default;
    ModelInf(const QString &provider_id,
             const QString &id,
             const QString &external_model,
             const QString &display_name = "");

    void set_display_name(const QString &);
    void set_external_name(const QString &);

    QString provider_id() const;
    QString model_id() const;
    QString external_model_name() const;
    QString display_model_name() const;
};

class ProviderInf
{
private:
    QString m_id;   //供应商id，不可更改
    QString m_name; //供应商
    QString m_url;  //api调用url
    QString m_key;  //api key，未加密！！！！加密待实现
    QList<QString> m_supported_models;

public:
    ProviderInf() = default;
    ProviderInf(const QString &id, const QString name, const QString url, const QString key);
    ProviderInf(const QString &id,
                const QString name,
                const QString url,
                const QString key,
                const QList<QString> &);

    void set_name(const QString &name);
    void set_url(const QString &url);
    void set_key(const QString &key);
    void add_supported_model(const QString &);
    void delete_supported_model(const QString &);

    QString provider_id() const;
    QString provider_name() const;
    QString url() const;
    QString key() const;
    QList<QString> supported_models() const;
};

#endif // MODELCONFIG_H
