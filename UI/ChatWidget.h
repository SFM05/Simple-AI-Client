#ifndef CHATWIDGET_H
#define CHATWIDGET_H

#include <QTextEdit>
#include <QListWidget>
#include <QPushButton>
#include <QString>
#include <QVBoxLayout>
#include <QWidget>
#include "Control/ConversationManager.h"
#include "Model/Conversation.h"
#include "Model/Message.h"
#include "ChatItemWidget.h"

class ApiClient; // 前向声明

class ChatWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ChatWidget(QListWidget *ConversationBox,
                        QTextEdit *InputBox,
                        QPushButton *SendButton,
                        QLabel *TitleLabel,
                        QWidget *parent = nullptr);

    void loadConversation(const QString &conversationId);
    void appendUserMessage(const QString &text);
    void appendAIMessageStream(const QString &token);
    void finishAIMessage();
    QString generatingEnd(const Message &);

private:
    QListWidget *m_listWidget;
    QTextEdit *m_inputEdit;
    QPushButton *m_sendButton;
    QLabel *m_titleLabel;
    QString m_currentConversationId;
    QListWidgetItem *m_currentAIItem = nullptr;
    ChatItemWidget *m_currentAIWidget = nullptr;
    Message m_AIMessage;
};

#endif // CHATWIDGET_H
