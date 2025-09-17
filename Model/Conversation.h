#ifndef CONVERSATION_H
#define CONVERSATION_H

#include "Message.h"
#include <QDateTime>
#include <QList>
#include <QString>

class Conversation
{
public:
    Conversation() = default;
    Conversation(const QString &id, const QString &title = "新对话");

    //修改变量
    void set_title(const QString &);
    void add_message(const Message &);
    void edit_message(int, const Message &);
    Message& get_message(int);
    Message& get_last_message();
    void delete_message(int);
    void set_created_time(QDateTime);
    void set_last_updated_time(QDateTime);
    void set_id(const QString &);
    // void clear();

    //读取变量
    QString id() const;
    QString title() const;
    QDateTime created_time() const;
    QDateTime last_updated_time() const;
    QList<Message> messages() const;

private:
    QString m_id;                  //对话id
    QString m_title;               //对话标题
    QDateTime m_created_time;      //创建时间
    QDateTime m_last_updated_time; //最近更新时间
    QList<Message> m_messages;     //消息列表
};

#endif // CONVERSATION_H
