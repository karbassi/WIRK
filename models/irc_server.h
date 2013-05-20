#ifndef IRC_SERVER_H
#define IRC_SERVER_H

#include <QObject>
#include "ircsession.h"
#include "irccommand.h"
#include <QSslSocket>
#include "message_parser.h"

class irc_server : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString host READ getHost WRITE setHost)
    Q_PROPERTY(int port READ getPort WRITE setPort)
    Q_PROPERTY(QString username READ getUsername WRITE setUsername)
    Q_PROPERTY(QString nickname READ getNickname WRITE setNickname)
    Q_PROPERTY(QString realname READ getRealname WRITE setRealname)
    Q_PROPERTY(bool ssl READ isSSL WRITE setSSL)
    Q_PROPERTY(QString text READ getText NOTIFY textChanged)
    // TODO: List of channels

public:
    explicit irc_server(QObject *parent = 0);
    
    QString getHost();
    void setHost(QString host);

    int getPort();
    void setPort(int port);

    QString getUsername();
    void setUsername(QString username);

    QString getNickname();
    void setNickname(QString nickname);

    QString getRealname();
    void setRealname(QString realname);

    void sendMessage(QString message);

    bool isSSL();
    void setSSL(bool ssl);

    QString getText();

    void createConnection();

    // TODO:  saveSettings()
    // TODO:  retrieveFromSettings();
    // TODO:  getTreeView();

signals:
    void textChanged(QString text);
    
private slots:
    void processMessage(IrcMessage *message);
    void processError(QAbstractSocket::SocketError error);
    
private:
    QString m_host;
    int m_port;
    QString m_username;
    QString m_nickname;
    QString m_realname;
    bool m_ssl;
    IrcSession *m_session;
    QString m_text;
    message_parser *m_parser;
};

#endif // IRC_SERVER_H
