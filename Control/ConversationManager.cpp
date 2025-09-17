#include "Control/ConversationManager.h"
#include <QHash>
#include <QList>
#include <QUuid>
#include <QFile>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QDebug>

#include "Model/Conversation.h"

ConversationManager &ConversationManager::instance()
{
    static ConversationManager instance;
    return instance;
}


bool ConversationManager::save_to_file(const QString &file_path) const
{
    QJsonArray conversationsArray;

    for (const auto &conversation : m_conversations) {
        QJsonObject convObj;
        convObj["id"] = conversation.id();
        convObj["title"] = conversation.title();
        convObj["created_time"] = conversation.created_time().toString(Qt::ISODate);
        convObj["last_updated_time"] = conversation.last_updated_time().toString(Qt::ISODate);

        QJsonArray messagesArray;
        for (const auto &message : conversation.messages()) {
            QJsonObject msgObj;
            msgObj["role"] = static_cast<int>(message.role());
            msgObj["content"] = message.content();
            msgObj["creating_time"] = message.creating_time().toString(Qt::ISODate);
            msgObj["first_token_latency"] = message.first_token_latency();
            msgObj["provider_id"] = message.provider_id();
            msgObj["model_id"] = message.model_id();
            msgObj["tokens"] = message.tokens_used();
            msgObj["status"] = static_cast<int>(message.status());

            messagesArray.append(msgObj);
        }

        convObj["messages"] = messagesArray;
        conversationsArray.append(convObj);
    }

    QJsonDocument doc(conversationsArray);

    QFile file(file_path);
    if (!file.open(QIODevice::WriteOnly)) {
        qWarning() << "Failed to open file for writing:" << file_path;
        return false;
    }

    file.write(doc.toJson());
    file.close();
    return true;
}


bool ConversationManager::load_from_file(const QString &file_path)
{
    QFile file(file_path);
    if (!file.open(QIODevice::ReadOnly)) {
        qWarning() << "Failed to open file for reading:" << file_path;
        return false;
    }

    QByteArray data = file.readAll();
    file.close();

    QJsonParseError parseError;
    QJsonDocument doc = QJsonDocument::fromJson(data, &parseError);
    if (parseError.error != QJsonParseError::NoError) {
        qWarning() << "JSON parse error:" << parseError.errorString();
        return false;
    }

    if (!doc.isArray()) {
        qWarning() << "Invalid JSON format: root is not an array.";
        return false;
    }

    QJsonArray conversationsArray = doc.array();
    m_conversations.clear();

    for (const auto &convValue : conversationsArray) {
        if (!convValue.isObject()) continue;

        QJsonObject convObj = convValue.toObject();
        QString id = convObj["id"].toString();
        QString title = convObj["title"].toString();
        QDateTime createdTime = QDateTime::fromString(convObj["created_time"].toString(), Qt::ISODate);
        QDateTime updatedTime = QDateTime::fromString(convObj["last_updated_time"].toString(), Qt::ISODate);

        Conversation conversation(id, title);
        conversation.set_created_time(createdTime);
        conversation.set_last_updated_time(updatedTime);
        // Manually set timestamps since constructor sets current time
        // You may need to add setters in Conversation class for this
        // For now, assuming you can modify the object directly or via friend/accessor

        QJsonArray messagesArray = convObj["messages"].toArray();
        for (const auto &msgValue : messagesArray) {
            if (!msgValue.isObject()) continue;

            QJsonObject msgObj = msgValue.toObject();
            Message_Inf::Role role = static_cast<Message_Inf::Role>(msgObj["role"].toInt());
            QString provider = msgObj["provider_id"].toString();
            QString model = msgObj["model_id"].toString();

            Message message(role, provider, model);
            message.set_content(msgObj["content"].toString());
            message.set_first_token_latency(msgObj["first_token_latency"].toVariant().toLongLong());
            message.set_tokens(msgObj["tokens"].toInt());
            message.set_status(static_cast<Message_Inf::Status>(msgObj["status"].toInt()));
            message.set_creating_time(
                QDateTime::fromString(msgObj["creating_time"].toString(), Qt::ISODate));
            // Set creating time manually if needed (you might want to add a setter)
            // This assumes you have access or a way to modify the internal time

            conversation.add_message(message);
        }

        conversation.set_last_updated_time(updatedTime);
        // Insert into manager map
        m_conversations.insert(id, conversation);
    }

    return true;
}



QString ConversationManager::new_conversation()
{
    QUuid uuid = QUuid::createUuid();
    QString id = uuid.toString().remove("{").remove("}");
    Conversation conversation{id};
    m_conversations.insert(id, conversation);
    return id;
}

void ConversationManager::delete_conversation(const QString &id)
{
    m_conversations.remove(id);
}

void ConversationManager::set_title(const QString &id, const QString &title)
{
    m_conversations[id].set_title(title);
}

Conversation &ConversationManager::get_conversation(const QString &id)
{
    return m_conversations[id];
}

QList<QPair<QString, QString>> ConversationManager::all_conversations()
{
    QList<QPair<QString, QString>> conversation_list;
    QList<Conversation> conversations = m_conversations.values();
    for (auto i : conversations) {
        conversation_list.append(qMakePair(i.id(), i.title()));
    }
    return conversation_list;
}
