#include "ConversationListWidget.h"
#include <QApplication>
#include <QListWidgetItem>
#include <QMessageBox>
#include <QDebug>
#include "Control/ConversationManager.h"

// RenameDialog implementation
RenameDialog::RenameDialog(const QString &currentTitle, QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle("重命名对话");
    setModal(true);
    resize(300, 100);

    QVBoxLayout *layout = new QVBoxLayout(this);

    QLabel *label = new QLabel("请输入新的对话标题：");
    layout->addWidget(label);

    m_titleEdit = new QLineEdit(currentTitle);
    layout->addWidget(m_titleEdit);

    QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok
                                                       | QDialogButtonBox::Cancel);
    connect(buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
    layout->addWidget(buttonBox);

    m_titleEdit->selectAll();
}

QString RenameDialog::getNewTitle() const
{
    return m_titleEdit->text();
}

// ConversationListWidget implementation
ConversationListWidget::ConversationListWidget(QListWidget *ConversationList,
                                               QPushButton *NewConversationButton,
                                               QWidget *parent)
    : QWidget(parent)
    , m_listWidget(ConversationList)
    , m_newConversationButton(NewConversationButton)
    , m_currentContextMenuId("")
    , m_contextMenu(nullptr)
    , m_renameAction(nullptr)
    , m_deleteAction(nullptr)
{
    connect(m_newConversationButton,
            &QPushButton::clicked,
            this,
            &ConversationListWidget::onNewConversationClicked);

    m_listWidget->setContextMenuPolicy(Qt::CustomContextMenu);

    connect(m_listWidget, &QListWidget::itemClicked, this, &ConversationListWidget::onItemClicked);

    connect(m_listWidget,
            &QListWidget::customContextMenuRequested,
            this,
            &ConversationListWidget::onItemRightClicked);

    createContextMenu();
    refreshConversations();
}

void ConversationListWidget::createContextMenu()
{
    m_contextMenu = new QMenu(this);

    m_renameAction = new QAction("更改标题", this);
    connect(m_renameAction,
            &QAction::triggered,
            this,
            &ConversationListWidget::onRenameActionTriggered);
    m_contextMenu->addAction(m_renameAction);

    m_deleteAction = new QAction("删除对话", this);
    connect(m_deleteAction,
            &QAction::triggered,
            this,
            &ConversationListWidget::onDeleteActionTriggered);
    m_contextMenu->addAction(m_deleteAction);
}

void ConversationListWidget::refreshConversations()
{
    m_listWidget->clear();

    ConversationManager &manager = ConversationManager::instance();
    QList<QPair<QString, QString>> conversations = manager.all_conversations();

    // 按最后更新时间排序（最新的在最上面）
    QMap<QString, QString> convMap;
    QMap<QString, QDateTime> timeMap;

    for (const auto &pair : conversations) {
        const QString &id = pair.first;
        convMap[id] = pair.second;
        timeMap[id] = manager.get_conversation(id).last_updated_time();
    }

    // 转换为列表并排序
    QList<QPair<QString, QDateTime>> sortedList;
    for (auto it = timeMap.constBegin(); it != timeMap.constEnd(); ++it) {
        sortedList.append(qMakePair(it.key(), it.value()));
    }

    std::sort(sortedList.begin(),
              sortedList.end(),
              [](const QPair<QString, QDateTime> &a, const QPair<QString, QDateTime> &b) {
                  return a.second > b.second; // 降序排列
              });

    for (const auto &pair : sortedList) {
        const QString &id = pair.first;
        QString title = convMap[id];
        if (title.isEmpty()) {
            title = "未命名对话";
        }

        QListWidgetItem *item = new QListWidgetItem(title);
        item->setData(Qt::UserRole, id);
        m_listWidget->addItem(item);
    }
}

void ConversationListWidget::onNewConversationClicked()
{
    ConversationManager &manager = ConversationManager::instance();
    QString newId = manager.new_conversation();
    qDebug()<<QDateTime::currentDateTime().toString("yyyy.MM.dd hh:mm:ss.zzz")<<"New Conversation. ID:"<<newId;

    // 设置默认标题
    QString defaultTitle = "新对话 " + QDateTime::currentDateTime().toString("MM-dd hh:mm");
    manager.set_title(newId, defaultTitle);

    refreshConversations();
    emit conversationCreated(newId);

    m_listWidget->setCurrentRow(0);
}

void ConversationListWidget::onItemClicked(QListWidgetItem *item)
{
    if (item) {
        QString id = item->data(Qt::UserRole).toString();
        emit conversationSelected(id);
    }
}

void ConversationListWidget::onItemRightClicked(const QPoint &pos)
{
    QListWidgetItem *item = m_listWidget->itemAt(pos);
    if (item) {
        m_currentContextMenuId = item->data(Qt::UserRole).toString();
        m_contextMenu->exec(QCursor::pos());
    }
}

void ConversationListWidget::onRenameActionTriggered()
{
    if (m_currentContextMenuId.isEmpty())
        return;

    ConversationManager &manager = ConversationManager::instance();
    Conversation &conversation = manager.get_conversation(m_currentContextMenuId);

    RenameDialog dialog(conversation.title());
    if (dialog.exec() == QDialog::Accepted) {
        QString newTitle = dialog.getNewTitle();
        if (!newTitle.isEmpty()) {
            manager.set_title(m_currentContextMenuId, newTitle);
            refreshConversations();
            emit conversationRenamed(m_currentContextMenuId, newTitle);
        }
    }
}

void ConversationListWidget::onDeleteActionTriggered()
{
    if (m_currentContextMenuId.isEmpty())
        return;

    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this,
                                  "确认删除",
                                  "确定要删除这个对话吗？",
                                  QMessageBox::Yes | QMessageBox::No);

    if (reply == QMessageBox::Yes) {
        ConversationManager &manager = ConversationManager::instance();
        manager.delete_conversation(m_currentContextMenuId);
        m_currentContextMenuId="";
        refreshConversations();
        emit conversationDeleted(m_currentContextMenuId);
    }
}
