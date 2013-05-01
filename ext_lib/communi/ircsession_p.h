/*
* Copyright (C) 2008-2013 The Communi Project
*
* This library is free software; you can redistribute it and/or modify it
* under the terms of the GNU Lesser General Public License as published by
* the Free Software Foundation; either version 2 of the License, or (at your
* option) any later version.
*
* This library is distributed in the hope that it will be useful, but WITHOUT
* ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
* FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public
* License for more details.
*/

#ifndef IRCSESSION_P_H
#define IRCSESSION_P_H

#include "ircsession.h"

#include <QSet>
#include <QString>
#include <QByteArray>
#include <QMultiHash>
#include <QAbstractSocket>

class IrcSessionPrivate
{
    Q_DECLARE_PUBLIC(IrcSession)

public:
    IrcSessionPrivate(IrcSession* session);

    void _irc_connected();
    void _irc_disconnected();
    void _irc_error(QAbstractSocket::SocketError error);
    void _irc_state(QAbstractSocket::SocketState state);
    void _irc_readData();

    void setNick(const QString& nick);
    void setActive(bool active);
    void setConnected(bool connected);
    void receiveMessage(IrcMessage* msg);

    static IrcSessionPrivate* get(const IrcSession* session)
    {
        return session->d_ptr.data();
    }

    IrcSession* q_ptr;
    QByteArray encoding;
    IrcProtocol* protocol;
    QAbstractSocket* socket;
    QString host;
    int port;
    QString userName;
    QString nickName;
    QString realName;
    bool active;
    bool connected;
    QSet<QString> capabilities;
    QHash<QString, QString> info;
};

#endif // IRCSESSION_P_H
