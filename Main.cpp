#include "./UI/mainwindow.h"
#include <QApplication>
#include "Control/AiRegistry.h"
#include "Control/ConversationManager.h"
#include "Network/ApiClient.h"
#include "Model/Conversation.h"
#include "Model/Message.h"
#include "Model/ModelConfig.h"
// #include <QTextCodec>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    ApiClient::initialize();

    MainWindow w;
    w.show();
    return a.exec();
}
