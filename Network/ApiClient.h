#ifndef APICLIENT_H
#define APICLIENT_H

#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QObject>
#include <QString>
#include "Model/Conversation.h"
#include "Model/Message.h"

class ApiClient : public QObject
{
    Q_OBJECT

public:
    // 获取单例实例（全局访问点）
    static ApiClient *instance();
    static void initialize(QObject *parent = nullptr);

    void send_request(const QString &url,
                      const QString &model_name,
                      const QString &key,
                      const Conversation &prior,
                      const QString &prompt,
                      float temp = 1.2,
                      int max_tokens = 1000); //错误处理未实现

    void interrupt();

signals:
    void TokenReceived(const QString &text);
    void Finished(Message_Inf::Status status, int tokens);
    void ErrorOccored(const QString &err); //未实现

private slots:
    void SSE_processing();

private:
    ApiClient(QObject *parent = nullptr);
    ~ApiClient();

    QNetworkAccessManager *m_networkManager;
    QNetworkReply *m_reply;

    int m_tokens = 0;
    Message_Inf::Status m_status = Message_Inf::Invalid;

    // 禁止拷贝
    Q_DISABLE_COPY(ApiClient)

    // 静态成员变量保存唯一实例
    static ApiClient *m_instance;
};

#endif // APICLIENT_H
