#include "ChatWidget.h"
#include "ChatItemWidget.h"
#include "Network/ApiClient.h"
#include "Control/AiRegistry.h"
#include <QListWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QListWidgetItem>
#include <QDebug>

ChatWidget::ChatWidget(QListWidget *ConversationBox,
                       QTextEdit *InputBox,
                       QPushButton *SendButton,
                       QLabel *TitleLabel,
                       QWidget *parent)
    : QWidget(parent)
    , m_listWidget(ConversationBox)
    , m_inputEdit(InputBox)
    , m_sendButton(SendButton)
    , m_titleLabel(TitleLabel)
{
    m_listWidget->setSizeAdjustPolicy(QAbstractScrollArea::AdjustToContents);
    m_listWidget->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
    connect(ApiClient::instance(),
            &ApiClient::TokenReceived,
            this,
            &ChatWidget::appendAIMessageStream);
}

void ChatWidget::loadConversation(const QString &conversationId)
{
    if (conversationId == "") {
        return;
    }

    auto &conversation = ConversationManager::instance().get_conversation(conversationId);

    if (m_currentConversationId==conversationId){
        m_titleLabel->setText(conversation.title());
        return;
    }

    m_currentConversationId = conversationId;
    m_listWidget->clear();

    m_titleLabel->setText(conversation.title());

    for (const auto &msg : conversation.messages()) {
        bool isUser = (msg.role() == Message_Inf::User);
        ChatItemWidget *itemWidget;

        if (msg.role() == Message_Inf::Assistant) {
            QString end = generatingEnd(msg);
            itemWidget = new ChatItemWidget(isUser, msg.content() + "\n\n*" + end + "*");
        }
        else{
            itemWidget = new ChatItemWidget(isUser, msg.content());
        }

        auto *item = new QListWidgetItem();
        m_listWidget->addItem(item);
        m_listWidget->setItemWidget(item, itemWidget);
        item->setSizeHint(itemWidget->sizeHint());
    }

}

void ChatWidget::appendUserMessage(const QString &text)
{
    if (m_currentConversationId==""){
        return;
    }

    auto &conversation = ConversationManager::instance().get_conversation(m_currentConversationId);
    Message userMsg(Message_Inf::User);
    userMsg.set_content(text);
    conversation.add_message(userMsg);

    auto *itemWidget = new ChatItemWidget(true, text);
    auto *item = new QListWidgetItem();
    m_listWidget->addItem(item);
    m_listWidget->setItemWidget(item, itemWidget);
    item->setSizeHint(itemWidget->sizeHint());

    m_listWidget->scrollToBottom();
}

void ChatWidget::appendAIMessageStream(const QString &token)
{
    if (m_currentConversationId==""){
        return;
    }

    if (!m_currentAIItem) {
        auto *itemWidget = new ChatItemWidget(false, "");
        m_currentAIWidget = itemWidget;
        m_currentAIItem = new QListWidgetItem();
        m_listWidget->addItem(m_currentAIItem);
        m_listWidget->setItemWidget(m_currentAIItem, m_currentAIWidget);
        m_currentAIItem->setSizeHint(m_currentAIWidget->sizeHint());
    }

    m_currentAIWidget->appendText(token);
    m_currentAIItem->setSizeHint(m_currentAIWidget->sizeHint());
    m_listWidget->scrollToBottom();

    auto &conv = ConversationManager::instance().get_conversation(m_currentConversationId);
    conv.get_last_message().add_content(token);
}

void ChatWidget::finishAIMessage()
{
    qDebug() << "all finished";
    auto &conv = ConversationManager::instance().get_conversation(m_currentConversationId);
    m_currentAIWidget->appendText("\n\n*" + generatingEnd(conv.get_last_message()) + "*");
    m_currentAIWidget = nullptr;
    m_currentAIItem = nullptr;
}

QString ChatWidget::generatingEnd(const Message &message)
{
    QString end = "";
    end += message.creating_time().toString("yyyy-MM-dd hh:mm:ss");
    end += ", ";
    end += AiRegistry::instance().get_model(message.model_id()).display_model_name();
    end += " (";
    end += AiRegistry::instance().get_provider(message.provider_id()).provider_name();
    end += "), 令牌数: ";
    end += QString::number(message.tokens_used());
    end += ", 首令牌延时: ";
    end += QString::number(message.first_token_latency());
    end += "ms, 终止状态: ";
    switch (message.status()) {
    case Message_Inf::stop:
        end += "正常";
        break;
    case Message_Inf::length:
        end += "达到最大令牌数";
        break;
    case Message_Inf::content_filter:
        end += "敏感内容";
        break;
    case Message_Inf::Cancelled:
        end += "用户中断";
        break;
    default:
        end += "Undefine";
        break;
    }
    return end;
}
