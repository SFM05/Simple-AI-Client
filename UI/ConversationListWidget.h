#ifndef CONVERSATIONLISTWIDGET_H
#define CONVERSATIONLISTWIDGET_H

#include <QAction>
#include <QDialog>
#include <QDialogButtonBox>
#include <QLabel>
#include <QLineEdit>
#include <QListWidget>
#include <QMenu>
#include <QPushButton>
#include <QVBoxLayout>
#include <QWidget>

class RenameDialog : public QDialog
{
    Q_OBJECT

public:
    explicit RenameDialog(const QString &currentTitle, QWidget *parent = nullptr);
    QString getNewTitle() const;

private:
    QLineEdit *m_titleEdit;
};

class ConversationListWidget : public QWidget
{
    Q_OBJECT

public:
    ConversationListWidget(QListWidget *ConversationList,
                           QPushButton *NewConversationButton,
                           QWidget *parent = nullptr);

    void refreshConversations();

signals:
    void conversationSelected(const QString &conversationId);
    void conversationCreated(const QString &conversationId);
    void conversationDeleted(const QString &conversationId);
    void conversationRenamed(const QString &conversationId, const QString &newTitle);

private slots:
    void onNewConversationClicked();
    void onItemClicked(QListWidgetItem *item);
    void onItemRightClicked(const QPoint &pos);
    void onRenameActionTriggered();
    void onDeleteActionTriggered();

private:
    void createContextMenu();

    QListWidget *m_listWidget;
    QPushButton *m_newConversationButton;

    QMenu *m_contextMenu;
    QAction *m_renameAction;
    QAction *m_deleteAction;

    QString m_currentContextMenuId;
};

#endif // CONVERSATIONLISTWIDGET_H
