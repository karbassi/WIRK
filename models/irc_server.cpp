#include "irc_server.h"

irc_server::irc_server(QObject *parent) : QObject(parent)
{
    m_parser = new message_parser(this);
}

QString irc_server::getHost() {
    return m_host;
}

void irc_server::setHost(QString host) {
    m_host = host;
}

int irc_server::getPort() {
    return m_port;
}

void irc_server::setPort(int port) {
    m_port = port;
}

QString irc_server::getUsername() {
    return m_username;
}

void irc_server::setUsername(QString username) {
    m_username = username;
}

QString irc_server::getNickname() {
    return m_nickname;
}

void irc_server::setNickname(QString nickname) {
    m_nickname = nickname;
}

QString irc_server::getRealname() {
    return m_realname;
}

void irc_server::setRealname(QString realname) {
    m_realname = realname;
}

bool irc_server::isSSL() {
    return m_ssl;
}

void irc_server::setSSL(bool ssl) {
    m_ssl = ssl;
}

QMap<QString, irc_channel*> irc_server::getChannels() {
    return m_channels;
}

void irc_server::setChannels(QMap<QString, irc_channel*> channels) {
    m_channels = channels;
}

void irc_server::createConnection() {

    m_session = new IrcSession(this);
    m_session->setHost(m_host);
    m_session->setPort(m_port);
    m_session->setUserName(m_username);
    m_session->setNickName(m_nickname);
    m_session->setRealName(m_realname);

    if(m_ssl) {
       QSslSocket* sslSocket = new QSslSocket(this);
        sslSocket->setPeerVerifyMode(QSslSocket::QueryPeer);
        sslSocket->ignoreSslErrors();
        m_session->setSocket(sslSocket);
    } else {
        m_session->setSocket(new QTcpSocket(this));
    }

    connect(m_session, SIGNAL(messageReceived(IrcMessage*)), this, SLOT(processMessage(IrcMessage*)));
    connect(m_session, SIGNAL(socketError(QAbstractSocket::SocketError)), this, SLOT(processError(QAbstractSocket::SocketError)));

    m_session->open();
}

QString irc_server::getText() {
    return m_text;
}

void irc_server::sendMessage(QString message) {
    if(message.at(0) == '/') {
        IrcCommand *command = m_parser->parseCommand(message);

        m_session->sendCommand(command);

    }
}

/***
 * This function parses the message searching for the username which is
 * propended to all messages by the server. This removes that first occurance
 * before returning the message_parser
 ***/
void irc_server::processMessage(IrcMessage *message)
{
    ParsedMessage* parsedMessage = m_parser->parse(message);
    QString fullMessage = parsedMessage->getMessage();
    QRegExp usernameRX("^(" + m_username + ")(.*)");
    int pos = usernameRX.indexIn(fullMessage);
    QString newString;
    if(pos > -1) {
        newString = usernameRX.cap(2);
        QRegExp highlightUsernameRX("(.*)(" + m_username + ")(.*)");
        pos = highlightUsernameRX.indexIn(newString);
        if(pos > -1) {
            newString = highlightUsernameRX.cap(1) + " <font color=\"Lime\">" + highlightUsernameRX.cap(2) + "</font> "
                    + highlightUsernameRX.cap(3);
        }
        newString += "<br />";
    } else {
        newString = fullMessage + "<br />";
    }


    m_text += newString;
    emit(textChanged(m_text));
}

void irc_server::processError(QAbstractSocket::SocketError error) {
    QString errorCode = QString::number(error);
    m_text += "Socket Error " + errorCode + "\n";
    emit(textChanged(m_text));
}
