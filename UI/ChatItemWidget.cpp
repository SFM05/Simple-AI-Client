#include "ChatItemWidget.h"
#include <QLabel>
#include <QVBoxLayout>
#include <QResizeEvent>

ChatItemWidget::ChatItemWidget(bool isUser, const QString &text, QWidget *parent)
    : QWidget(parent)
    , m_fullText(text)
    , m_isUser(isUser)
{
    m_label = new QLabel(this);
    m_label->setWordWrap(true);
    m_label->setTextFormat(Qt::MarkdownText);
    m_label->setText(m_fullText);
    m_label->setAlignment(isUser ? Qt::AlignRight : Qt::AlignLeft);

    updateStyleSheet();

    // 移除边框
    m_label->setFrameStyle(QFrame::NoFrame);

    // 设置大小策略
    m_label->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

    auto *layout = new QVBoxLayout(this);
    layout->addWidget(m_label);
    layout->setContentsMargins(10, 5, 10, 5);  // 添加一些内边距
    layout->setSpacing(0);
    setLayout(layout);

    // 初始调整大小
    adjustLabelSize();
}

void ChatItemWidget::appendText(const QString &text)
{
    m_fullText += text;
    m_label->setText(m_fullText);
    adjustLabelSize();
}

void ChatItemWidget::updateStyleSheet()
{
    QString bgColor = m_isUser ? "#DCF8C6" : "#E0E0E0";
    QString textAlign = m_isUser ? "right" : "left";

    QString style = QString(R"(
        QLabel {
            background-color: %1;
            border-radius: 10px;
            padding: 8px;
            text-align: %2;
        }
    )").arg(bgColor).arg(textAlign);

    m_label->setStyleSheet(style);
}

void ChatItemWidget::adjustLabelSize()
{
    // 强制 QLabel 重新计算大小
    m_label->setWordWrap(false);
    m_label->setWordWrap(true);

    // 更新布局
    layout()->activate();
    updateGeometry();
}

QSize ChatItemWidget::sizeHint() const
{
    // 让 QLabel 自己计算所需大小
    QSize labelSize = m_label->sizeHint();
    return QSize(labelSize.width() + 20, labelSize.height() + 10); // 加上边距
}

void ChatItemWidget::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);
    adjustLabelSize();
}
