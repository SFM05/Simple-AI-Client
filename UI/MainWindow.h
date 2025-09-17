#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "Network/ApiClient.h"
#include <QMainWindow>
#include <QString>
#include "ConversationListWidget.h"
#include "ChatWidget.h"
#include "Model/Message.h"

class QListWidgetItem;
class QPoint;

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();


signals:
    void all_Finished();

private slots:
    // 会话相关
    void on_InterpretButton_clicked();
    void on_SendButton_clicked();

private:
    void setup_model_list();
    void refresh_model_list();

    void open_model_management();
    void on_Finished(Message_Inf::Status status,int tokens);

    Ui::MainWindow *ui;
    QString message; //测试用

    ConversationListWidget *m_conversation_manager;
    ChatWidget *m_conversation_box;

    // 当前会话与流式状态
    QString m_currentConversationId;
    void set_conversation_id(const QString &id);

    bool m_requestInProgress = false;
    int m_assistantRow = -1;
    QString m_currentAssistantText;
    QString m_lastPrompt;
    Message_Inf::Status m_lastFinishStatus = Message_Inf::Invalid;
    QString m_lastTokenText;
};
#endif // MAINWINDOW_H
