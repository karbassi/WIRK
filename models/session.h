#ifndef SESSION_H
#define SESSION_H

#include <QStandardItemModel>
#include "channel.h"

class Server;
class Channel;

class Session : public QStandardItemModel
{
    Q_OBJECT
public:
    explicit Session(QObject *parent = 0);
    virtual ~Session();
    
    Server* addServer(QString host, int port, QString username, QString nickname, QString realname, QString password, bool isSSL);
    QStandardItem* getServerMenuItem(QString inServer);
    Server* getServer(QString inServer);

    void selectItem(QString string);
    void emitMessageReceived(Server *server, Channel *channel, QString message, Channel::MessageType type = Channel::Default);

public slots:
    void removeServer(QString inServer);

signals:
    void messageReceived(Server *server, Channel *channel, QString message, Channel::MessageType type);
    void selectItem(QModelIndex index);

};

#endif // SESSION_H
