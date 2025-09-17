#ifndef MESSAGE_H
#define MESSAGE_H

#include <QDateTime>
#include <QDebug>
#include <QString>

namespace Message_Inf {
enum Role { User, Assistant, System };                 //用户、ai、系统提示词
enum Status {
    stop,
    length,
    content_filter,
    Invalid,
    Cancelled
}; //正常停止、达到最大长度、触发敏感词、不可用
} // namespace Message_Inf

class Message
{
public:
    Message() = default;
    Message(Message_Inf::Role role,
            const QString &provider = "",
            const QString &model_id = ""); //创建时间自动生成，对于User和System默认为空

    //数据操作
    void add_content(const QString &);    //追加内容
    void set_content(const QString &);    //设置内容
    void set_first_token_latency(qint64); //如果使用add_content会自动更新
    void set_tokens(int);
    void set_status(Message_Inf::Status);
    void set_creating_time(QDateTime);
    void set_id(const QString &);

    //数据读取
    Message_Inf::Role role() const;
    QString content() const;
    QDateTime creating_time() const;
    qint64 first_token_latency() const;
    QString provider_id() const;
    QString model_id() const;
    int tokens_used() const;
    Message_Inf::Status status() const;

private:
    Message_Inf::Role m_role;     //对话角色
    QString m_content;            //对话内容
    QDateTime m_creating_time;     //请求发送时间
    qint64 m_first_token_latency; //首token响应时间
    QString m_provider_id;           //供应商
    QString m_model_id;         //调用模型名
    int m_tokens;                 //token数
    Message_Inf::Status m_status; //结束状态
};

#endif // MESSAGE_H
