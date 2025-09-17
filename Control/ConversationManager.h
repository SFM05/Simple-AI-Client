#ifndef CONVERSATIONMANAGER_H
#define CONVERSATIONMANAGER_H

#include <QHash>
#include <QList>
#include "Model/Conversation.h"

class ConversationManager
{
private:
    QHash<QString, Conversation> m_conversations;

    ConversationManager() = default;
    ~ConversationManager() = default;

public:
    static ConversationManager &instance();

    ConversationManager(const ConversationManager &) = delete;
    ConversationManager &operator=(const ConversationManager &) = delete;

    bool load_from_file(const QString &);
    bool save_to_file(const QString &) const;

    QString new_conversation();
    void delete_conversation(const QString &);
    void set_title(const QString &id, const QString &title);
    Conversation &get_conversation(const QString &);

    QList<QPair<QString, QString>> all_conversations();
};

#endif // CONVERSATIONMANAGER_H
