#include "ApiClient.h"
#include <QDebug>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QObject>
#include <QString>
#include <QDateTime>
#include "Model/Conversation.h"
#include "Model/Message.h"

ApiClient *ApiClient::m_instance = nullptr;
void ApiClient::initialize(QObject *parent)
{
    if (m_instance == nullptr) {
        m_instance = new ApiClient(parent);
    }
}
ApiClient *ApiClient::instance()
{
    return m_instance;
}

ApiClient::ApiClient(QObject *parent)
    : QObject{parent}
    , m_networkManager(new QNetworkAccessManager(this))
    , m_reply{nullptr}
{}

ApiClient::~ApiClient()
{
    if (m_reply) {
        m_reply->abort();
        m_reply->deleteLater();
    }
}

void ApiClient::send_request(const QString &url,
                             const QString &key,
                             const QString &model_name,
                             const Conversation &prior,
                             const QString &prompt,
                             float temp,
                             int max_tokens)
{
    QNetworkRequest request;
    request.setUrl(QUrl(url + "/completions"));
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setRawHeader("Authorization", ("Bearer " + key).toUtf8());

    QJsonObject json;
    json["model"] = model_name;
    json["stream"] = true;

    QJsonArray messages;

    QList<Message> conversations=prior.messages();
    for (auto i = conversations.cbegin(); i != conversations.cend() - 1; i++) {
        switch (i->role()) {
        case Message_Inf::System:
            messages.append(QJsonObject({{"role", "system"}, {"content", i->content()}}));
            break;
        case Message_Inf::User:
            messages.append(QJsonObject({{"role", "user"}, {"content", i->content()}}));
            break;
        case Message_Inf::Assistant:
            messages.append(QJsonObject({{"role", "assistant"}, {"content", i->content()}}));
            break;
        }
    }
    messages.append(QJsonObject({{"role", "user"}, {"content", prompt}}));
    json["messages"] = messages;

    QJsonDocument doc(json);
    QByteArray data = doc.toJson();

    m_reply = m_networkManager->post(request, data);

    qDebug() << QDateTime::currentDateTime().toString("yyyy.MM.dd hh:mm:ss.zzz")
             << "Request Started";
    qDebug() << "\tURL:" << url + "/completions" << ", model:" << model_name;

    m_tokens = 0;

    connect(m_reply, &QNetworkReply::readyRead, this, &ApiClient::SSE_processing);
}

void ApiClient::SSE_processing()
{
    if (!m_reply)
        return;

    //逐步读取数据
    while (m_reply->canReadLine()) {
        QByteArray line = m_reply->readLine().trimmed();
        if (line.isEmpty())
            continue;

        //SSE格式：{data: }
        if (line.startsWith("data: ")) {
            QByteArray jsonData = line.mid(6); //去掉"date: "
            if (jsonData == "[DONE]") {
                emit Finished(m_status, m_tokens);
                break;
            }

            QJsonParseError error;
            QJsonDocument doc = QJsonDocument::fromJson(jsonData, &error);
            if (error.error != QJsonParseError::NoError) {
                continue;
            }

            QJsonObject obj = doc.object();
            if (!obj.contains("choices") || !obj["choices"].isArray()) {
                continue;
            }
            QJsonArray choices = obj["choices"].toArray();
            if (choices.isEmpty()) {
                continue;
            }
            QJsonObject choice = choices.at(0).toObject();
            QJsonObject delta = choice["delta"].toObject();
            QString content = delta["content"].toString();
            // 过滤偶发性重复空格/换行引起的重复刷屏
            if (content == "\n" || content == "\r" || content == "\r\n") {
                content = "\n";
            }

            if (!content.isEmpty()) {
                emit TokenReceived(content);
                m_tokens++;
                qDebug() << QDateTime::currentDateTime().toString("yyyy.MM.dd hh:mm:ss.zzz") << "SSE Received token";
            }

            QString status = choice["finish_reason"].toString();
            if (status == "stop"){
                m_status=Message_Inf::stop;
                qDebug()<<QDateTime::currentDateTime().toString("yyyy.MM.dd hh:mm:ss.zzz")<<"Request ended. Reason: Normal";
            }
            else if (status == "length"){
                m_status=Message_Inf::length;
                qDebug()<<QDateTime::currentDateTime().toString("yyyy.MM.dd hh:mm:ss.zzz")<<"Request ended. Reason: Reaching max length";
            }
            else if (status == "content_filter"){
                m_status=Message_Inf::content_filter;
                qDebug()<<QDateTime::currentDateTime().toString("yyyy.MM.dd hh:mm:ss.zzz")<<"Request ended. Reason: Senstive content";
            }
        }
    }
}

void ApiClient::interrupt()
{
    if (m_reply) {
        m_reply->abort();
        m_status = Message_Inf::Cancelled;
        qDebug() << QDateTime::currentDateTime().toString("yyyy.MM.dd hh:mm:ss.zzz")
                 << "Request ended. Reason: Cancelled by user";
        emit Finished(m_status, m_tokens);
    }
}
