#include "Conversation.h"
#include <QDateTime>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QList>
#include <QString>
#include "Message.h"

Conversation::Conversation(const QString &id, const QString &title)
    : m_id{id}
    , m_title{title}
    , m_created_time{QDateTime::currentDateTime()}
    , m_last_updated_time{QDateTime::currentDateTime()}
{}

//修改变量
void Conversation::set_title(const QString &title)
{
    m_title = title;
    m_last_updated_time = QDateTime::currentDateTime();
}

void Conversation::add_message(const Message &message)
{
    m_messages.push_back(message);
    m_last_updated_time = QDateTime::currentDateTime();
}

void Conversation::edit_message(int idx, const Message &message)
{
    m_messages[idx] = message;
    m_last_updated_time = QDateTime::currentDateTime();
}

Message &Conversation::get_message(int idx){
    m_last_updated_time = QDateTime::currentDateTime();
    return m_messages[idx];
}

Message &Conversation::get_last_message(){
    m_last_updated_time = QDateTime::currentDateTime();
    return *m_messages.rbegin();
}

void Conversation::delete_message(int idx)
{
    m_messages.erase(m_messages.cbegin() + idx);
    m_last_updated_time = QDateTime::currentDateTime();
}

void Conversation::set_created_time(QDateTime created_time)
{
    m_created_time = created_time;
}

void Conversation::set_last_updated_time(QDateTime last_updated_time)
{
    m_last_updated_time = last_updated_time;
}

void Conversation::set_id(const QString &id)
{
    m_id = id;
}

//读取变量
QString Conversation::id() const
{
    return m_id;
}

QString Conversation::title() const
{
    return m_title;
}

QDateTime Conversation::created_time() const
{
    return m_created_time;
}

QDateTime Conversation::last_updated_time() const
{
    return m_last_updated_time;
}

QList<Message> Conversation::messages() const
{
    return m_messages;
}
