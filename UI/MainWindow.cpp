#include "mainwindow.h"
#include <QAction>
#include <QApplication>
#include <QDateTime>
#include <QDebug>
#include <QInputDialog>
#include <QListWidgetItem>
#include <QMenu>
#include <QRegularExpression>
#include <QScrollBar>
#include <QShortcut>
#include <QStandardItem>
#include <QStandardItemModel>
#include <QTextBrowser>

#include "ChatWidget.h"
#include "Control/AiRegistry.h"
#include "Control/ConversationManager.h"
#include "ConversationListWidget.h"
#include "Model/Conversation.h"
#include "Network/ApiClient.h"
#include "modelmanagement.h"
#include "ui_MainWindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , m_currentConversationId("")
{
    AiRegistry::instance().load_from_file("modelConfig.json");
    ConversationManager::instance().load_from_file("conversations.json");

    ui->setupUi(this);
    setWindowTitle("AI客户端");
    ApiClient::initialize();

    m_conversation_manager = new ConversationListWidget(ui->ConversationList,
                                                        ui->NewConversationButton,
                                                        this);

    m_conversation_box = new ChatWidget(ui->ConversationBox,
                                        ui->PromptEdit,
                                        ui->SendButton,
                                        ui->TitleLabel,
                                        this);

    connect(m_conversation_manager,
            &ConversationListWidget::conversationSelected,
            m_conversation_box,
            &ChatWidget::loadConversation);
    connect(m_conversation_manager,
            &ConversationListWidget::conversationCreated,
            m_conversation_box,
            &ChatWidget::loadConversation);
    connect(m_conversation_manager,
            &ConversationListWidget::conversationRenamed,
            m_conversation_box,
            &ChatWidget::loadConversation);
    connect(m_conversation_manager,
            &ConversationListWidget::conversationDeleted,
            m_conversation_box,
            &ChatWidget::loadConversation);



    connect(m_conversation_manager,
            &ConversationListWidget::conversationSelected,
            this,
            &MainWindow::set_conversation_id);
    connect(m_conversation_manager,
            &ConversationListWidget::conversationCreated,
            this,
            &MainWindow::set_conversation_id);
    connect(m_conversation_manager,
            &ConversationListWidget::conversationDeleted,
            this,
            &MainWindow::set_conversation_id);


    connect(ApiClient::instance(), &ApiClient::Finished, this, &MainWindow::on_Finished);

    connect(ui->InterpretButton,
            &QPushButton::clicked,
            this,
            &MainWindow::on_InterpretButton_clicked);

    // QString name="阿里云";
    // QString url = "https://dashscope.aliyuncs.com/compatible-mode/v1/chat";
    // QString key = "sk-24618e6c3a054e478af5ef322ee9fa40";
    // QString provider_id=AiRegistry::instance().add_provider(name,url,key);

    // AiRegistry::instance().add_model(provider_id,"qwen-max");
    // AiRegistry::instance().add_model(provider_id,"qwen-turbo");
    // qDebug()<<AiRegistry::instance().all_providers();

    message = "";

    setup_model_list();

    connect(ui->ModelManagementButton,
            &QPushButton::clicked,
            this,
            &MainWindow::open_model_management);


    connect(this,
            &MainWindow::all_Finished,
            m_conversation_box,
            &ChatWidget::finishAIMessage);

    // 初始化当前会话
    ui->ConversationBox->clear();
    ui->InterpretButton->setEnabled(false);
    // 样式优化：圆角与背景
    setStyleSheet("QListWidget, QTextBrowser, QComboBox, QPushButton {"
                  "  border-radius: 8px;"
                  "}"
                  "QTextBrowser {"
                  "  background: #ffffff;"
                  "  border: 1px solid #e6e6e6;"
                  "}"
                  "QListWidget {"
                  "  background: #ffffff;"
                  "  border: 1px solid #e6e6e6;"
                  "}"
                  "QComboBox {"
                  "  padding: 4px 8px;"
                  "  border: 1px solid #e6e6e6;"
                  "  background: #fafafa;"
                  "}"
                  "QPushButton {"
                  "  padding: 6px 10px;"
                  "  background: #1677ff;"
                  "  color: white;"
                  "  border: none;"
                  "}"
                  "QPushButton:disabled {"
                  "  background: #a0c4ff;"
                  "  color: #f5f5f5;"
                  "}");

    // 快捷键：Ctrl+Enter 发送，Esc 中止
    auto sendShortcut = new QShortcut(QKeySequence(Qt::CTRL | Qt::Key_Return), this);
    connect(sendShortcut, &QShortcut::activated, this, &MainWindow::on_SendButton_clicked);
    // auto sendShortcut2 = new QShortcut(QKeySequence(Qt::CTRL | Qt::Key_Enter), this);
    // connect(sendShortcut2, &QShortcut::activated, this, &MainWindow::on_SendButton_clicked);
    // auto stopShortcut = new QShortcut(QKeySequence(Qt::Key_Escape), this);
    // connect(stopShortcut, &QShortcut::activated, this, &MainWindow::on_InterpretButton_clicked);
}

MainWindow::~MainWindow()
{
    AiRegistry::instance().save_to_file("modelConfig.json");
    ConversationManager::instance().save_to_file("conversations.json");
    delete ui;
}

void MainWindow::set_conversation_id(const QString &id)
{
    if (m_currentConversationId!=id){
        m_currentConversationId = id;
    }
}

//历史对话列表

//模型选择列表
void MainWindow::setup_model_list()
{
    // 清空现有内容（防止重复添加）
    ui->ModelList->clear();

    // 创建新模型
    QStandardItemModel *model = new QStandardItemModel(this);

    auto provider_list = AiRegistry::instance().all_providers();

    for (auto i : provider_list) {
        QStandardItem *item = new QStandardItem(i.second);
        item->setEditable(false);
        item->setFlags(item->flags() & ~Qt::ItemIsSelectable);
        item->setFont(QFont("Arial", 10, QFont::Bold));
        item->setEnabled(false); // 灰色不可选
        model->appendRow(item);

        auto model_list = AiRegistry::instance().all_models(i.first);
        for (auto j : model_list) {
            QStandardItem *item = new QStandardItem("  " + j.second);
            item->setData(j.first, Qt::UserRole);
            item->setEditable(false);
            model->appendRow(item);
        }
    }

    ui->ModelList->setModel(model);
}

void MainWindow::refresh_model_list()
{
    setup_model_list();
}

//模型管理模块
void MainWindow::open_model_management()
{
    ModelManagement *new_widget = new ModelManagement(this);
    new_widget->setWindowModality(Qt::NonModal); // 可选：非模态
    new_widget->setWindowFlags(Qt::Window);      // 强制作为独立窗口
    new_widget->setAttribute(Qt::WA_DeleteOnClose);
    new_widget->show();

    connect(new_widget, &ModelManagement::ModelUpdate, this, &MainWindow::refresh_model_list);
}

void MainWindow::on_InterpretButton_clicked()
{
    if (m_requestInProgress){
        ApiClient::instance()->interrupt();
        m_requestInProgress = false;
    }
}

void MainWindow::on_SendButton_clicked()
{
    if (ui->ModelList->currentIndex() < 0)
        return;

    QString model_id = ui->ModelList->currentData(Qt::UserRole).toString();
    auto model = AiRegistry::instance().get_model(model_id);
    QString provider_id = model.provider_id();
    auto provider = AiRegistry::instance().get_provider(provider_id);

    QString url = provider.url();
    QString key = provider.key();
    QString external_model_name = model.external_model_name();


    if (m_currentConversationId==""){
        return;
    }

    QString prompt = ui->PromptEdit->toPlainText();
    if (prompt.trimmed() == "")
        return;
    auto &conv = ConversationManager::instance().get_conversation(m_currentConversationId);

    m_conversation_box->appendUserMessage(prompt);
    Message userMsg(Message_Inf::User);
    userMsg.set_content(prompt);
    ui->PromptEdit->clear();

    // 模拟 AI 回复（你可以在这里调用 ApiClient 发送请求）
    // 示例：connect(apiClient, &ApiClient::TokenReceived, this, &ChatWidget::appendAIMessageStream);

    conv.add_message(Message(Message_Inf::Assistant, provider_id, model_id));
    ui->ConversationList->setEnabled(false);
    ui->SendButton->setEnabled(false);
    ui->InterpretButton->setEnabled(true);

    m_requestInProgress = true;
    ApiClient::instance()->send_request(url, key, external_model_name, conv, prompt);
}

void MainWindow::on_Finished(Message_Inf::Status status,int tokens)
{
    m_requestInProgress = false;
    ui->ConversationList->setEnabled(true);
    ui->SendButton->setEnabled(true);
    ui->InterpretButton->setEnabled(false);
    auto &conv = ConversationManager::instance().get_conversation(m_currentConversationId);
    conv.get_last_message().set_status(status);
    conv.get_last_message().set_tokens(tokens);
    qDebug() << QDateTime::currentDateTime().toString("yyyy.MM.dd hh:mm:ss.zzz") << "All Finished";
    emit all_Finished();
}
