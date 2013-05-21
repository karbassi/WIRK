#ifndef IRC_SERVER_H
#define IRC_SERVER_H

#include <QObject>
#include "ircsession.h"
#include "irccommand.h"
#include <QSslSocket>
#include "irc_channel.h"
#include <QMap>

typedef QMap<QString, irc_channel*> IrcChannelMap;

class message_parser;

class irc_server : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString host READ getHost WRITE setHost)
    Q_PROPERTY(int port READ getPort WRITE setPort)
    Q_PROPERTY(QString username READ getUsername WRITE setUsername)
    Q_PROPERTY(QString nickname READ getNickname WRITE setNickname)
    Q_PROPERTY(QString realname READ getRealname WRITE setRealname)
    Q_PROPERTY(bool ssl READ isSSL WRITE setSSL)
    Q_PROPERTY(QString text READ getText WRITE setText NOTIFY textChanged)
    Q_PROPERTY(IrcChannelMap channels READ getChannels WRITE setChannels)

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
    void setText(QString text);
    void appendText(QString text);

    void createConnection();

    QMap<QString, irc_channel*> getChannels();
    void setChannels(QMap<QString, irc_channel*> channels);

    // TODO:  saveSettings()
    // TODO:  retrieveFromSettings();

signals:
    void textChanged(parsed_message *message);
    void channelChanged();
    
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
    QMap<QString, irc_channel*> m_channels;
};

#endif // IRC_SERVER_H
