#ifndef CHATITEMWIDGET_H
#define CHATITEMWIDGET_H

#include <QWidget>
#include <QLabel>
#include <QVBoxLayout>

class ChatItemWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ChatItemWidget(bool isUser, const QString &text, QWidget *parent = nullptr);

    void appendText(const QString &text);
    QSize sizeHint() const override;

protected:
    void resizeEvent(QResizeEvent *event) override;

private:
    void updateStyleSheet();
    void adjustLabelSize();

private:
    QLabel *m_label;
    QString m_fullText;
    bool m_isUser;
};


#endif // CHATITEMWIDGET_H
