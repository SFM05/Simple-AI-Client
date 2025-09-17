#include "Message.h"
#include <QDateTime>
#include <QDebug>
#include <QJsonObject>
#include <QString>

Message::Message(Message_Inf::Role role, const QString &provider, const QString &model_id)
    : m_role{role}
    , m_content{""}
    , m_creating_time{QDateTime::currentDateTime()}
    , m_first_token_latency{-1}
    , m_provider_id{provider}
    , m_model_id{model_id}
    , m_tokens{0}
    , m_status{Message_Inf::Invalid}
{}

//数据操作
void Message::add_content(const QString &adding)
{
    m_content = m_content + adding;
    //在m_first_token_latency为负值时更新
    if (m_first_token_latency < 0) {
        QDateTime current = QDateTime::currentDateTime();
        m_first_token_latency = m_creating_time.msecsTo(current);
    }
}

void Message::set_content(const QString &content)
{
    m_content = content;
}

void Message::set_first_token_latency(qint64 lateny)
{
    m_first_token_latency = lateny;
}

void Message::set_tokens(int tokens)
{
    m_tokens = tokens;
}

void Message::set_status(Message_Inf::Status status)
{
    m_status = status;
}

void Message::set_creating_time(QDateTime creating_time)
{
    m_creating_time = creating_time;
}

void Message::set_id(const QString &id)
{
    m_model_id = id;
}

//数据读取
Message_Inf::Role Message::role() const
{
    return m_role;
}

QString Message::content() const
{
    return m_content;
}

QDateTime Message::creating_time() const
{
    return m_creating_time;
}

qint64 Message::first_token_latency() const
{
    return m_first_token_latency;
}

QString Message::provider_id() const
{
    return m_provider_id;
}

QString Message::model_id() const
{
    return m_model_id;
}

int Message::tokens_used() const
{
    return m_tokens;
}

Message_Inf::Status Message::status() const
{
    return m_status;
}
