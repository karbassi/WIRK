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

#ifndef IRCMESSAGE_H
#define IRCMESSAGE_H

#include "ircsender.h"
#include <QtCore/qobject.h>
#include <QtCore/qdatetime.h>
#include <QtCore/qstringlist.h>

class IrcSession;
class IrcCommand;
class IrcMessagePrivate;

class IrcMessage : public QObject
{
    Q_OBJECT
    Q_PROPERTY(IrcSession* session READ session)
    Q_PROPERTY(Type type READ type)
    Q_PROPERTY(Flags flags READ flags)
    Q_PROPERTY(bool valid READ isValid)
    Q_PROPERTY(QString command READ command)
    Q_PROPERTY(IrcSender sender READ sender WRITE setSender)
    Q_PROPERTY(QStringList parameters READ parameters WRITE setParameters)
    Q_PROPERTY(QDateTime timeStamp READ timeStamp WRITE setTimeStamp)
    Q_ENUMS(Type Flag)
    Q_FLAGS(Flags)

public:
    enum Type {
        Unknown,
        Nick,
        Quit,
        Join,
        Part,
        Topic,
        Invite,
        Kick,
        Mode,
        Private,
        Notice,
        Ping,
        Pong,
        Error,
        Numeric,
        Capability
    };

    enum Flag {
        None = 0x0,
        Own = 0x1,
        Identified = 0x2,
        Unidentified = 0x4
    };
    Q_DECLARE_FLAGS(Flags, Flag)

    Q_INVOKABLE explicit IrcMessage(IrcSession* session);
    virtual ~IrcMessage();

    IrcSession* session() const;

    Type type() const;
    Flags flags() const;
    QString command() const;

    IrcSender sender() const;
    void setSender(const IrcSender& sender);

    QStringList parameters() const;
    void setParameters(const QStringList& parameters);

    virtual bool isValid() const;

    QDateTime timeStamp() const;
    void setTimeStamp(const QDateTime& timeStamp);

    QByteArray encoding() const;
    void setEncoding(const QByteArray& encoding);

    Q_INVOKABLE QByteArray toData() const;
    Q_INVOKABLE static IrcMessage* fromData(const QByteArray& data, IrcSession* session);
    Q_INVOKABLE static IrcMessage* fromCommand(const QString& sender, IrcCommand* command, IrcSession* session);
    Q_INVOKABLE static IrcMessage* fromParameters(const QString& sender, const QString& command, const QStringList& parameters, IrcSession* session);

protected:
    QScopedPointer<IrcMessagePrivate> d_ptr;
    Q_DECLARE_PRIVATE(IrcMessage)
    Q_DISABLE_COPY(IrcMessage)
};

Q_DECLARE_OPERATORS_FOR_FLAGS(IrcMessage::Flags)

class IrcNickMessage : public IrcMessage
{
    Q_OBJECT
    Q_PROPERTY(QString nick READ nick)

public:
    Q_INVOKABLE explicit IrcNickMessage(IrcSession* session);

    QString nick() const;

    bool isValid() const;

private:
    Q_DISABLE_COPY(IrcNickMessage)
};

class IrcQuitMessage : public IrcMessage
{
    Q_OBJECT
    Q_PROPERTY(QString reason READ reason)

public:
    Q_INVOKABLE explicit IrcQuitMessage(IrcSession* session);

    QString reason() const;

    bool isValid() const;

private:
    Q_DISABLE_COPY(IrcQuitMessage)
};

class IrcJoinMessage : public IrcMessage
{
    Q_OBJECT
    Q_PROPERTY(QString channel READ channel)

public:
    Q_INVOKABLE explicit IrcJoinMessage(IrcSession* session);

    QString channel() const;

    bool isValid() const;

private:
    Q_DISABLE_COPY(IrcJoinMessage)
};

class IrcPartMessage : public IrcMessage
{
    Q_OBJECT
    Q_PROPERTY(QString channel READ channel)
    Q_PROPERTY(QString reason READ reason)

public:
    Q_INVOKABLE explicit IrcPartMessage(IrcSession* session);

    QString channel() const;
    QString reason() const;

    bool isValid() const;

private:
    Q_DISABLE_COPY(IrcPartMessage)
};

class IrcTopicMessage : public IrcMessage
{
    Q_OBJECT
    Q_PROPERTY(QString channel READ channel)
    Q_PROPERTY(QString topic READ topic)

public:
    Q_INVOKABLE explicit IrcTopicMessage(IrcSession* session);

    QString channel() const;
    QString topic() const;

    bool isValid() const;

private:
    Q_DISABLE_COPY(IrcTopicMessage)
};

class IrcInviteMessage : public IrcMessage
{
    Q_OBJECT
    Q_PROPERTY(QString user READ user)
    Q_PROPERTY(QString channel READ channel)

public:
    Q_INVOKABLE explicit IrcInviteMessage(IrcSession* session);

    QString user() const;
    QString channel() const;

    bool isValid() const;

private:
    Q_DISABLE_COPY(IrcInviteMessage)
};

class IrcKickMessage : public IrcMessage
{
    Q_OBJECT
    Q_PROPERTY(QString channel READ channel)
    Q_PROPERTY(QString user READ user)
    Q_PROPERTY(QString reason READ reason)

public:
    Q_INVOKABLE explicit IrcKickMessage(IrcSession* session);

    QString channel() const;
    QString user() const;
    QString reason() const;

    bool isValid() const;

private:
    Q_DISABLE_COPY(IrcKickMessage)
};

class IrcModeMessage : public IrcMessage
{
    Q_OBJECT
    Q_PROPERTY(QString target READ target)
    Q_PROPERTY(QString mode READ mode)
    Q_PROPERTY(QString argument READ argument)

public:
    Q_INVOKABLE explicit IrcModeMessage(IrcSession* session);

    QString target() const;
    QString mode() const;
    QString argument() const;

    bool isValid() const;

private:
    Q_DISABLE_COPY(IrcModeMessage)
};

class IrcPrivateMessage : public IrcMessage
{
    Q_OBJECT
    Q_PROPERTY(QString target READ target)
    Q_PROPERTY(QString message READ message)
    Q_PROPERTY(bool action READ isAction)
    Q_PROPERTY(bool request READ isRequest)

public:
    Q_INVOKABLE explicit IrcPrivateMessage(IrcSession* session);

    QString target() const;
    QString message() const;
    bool isAction() const;
    bool isRequest() const;

    bool isValid() const;

private:
    Q_DISABLE_COPY(IrcPrivateMessage)
};

class IrcNoticeMessage : public IrcMessage
{
    Q_OBJECT
    Q_PROPERTY(QString target READ target)
    Q_PROPERTY(QString message READ message)
    Q_PROPERTY(bool reply READ isReply)

public:
    Q_INVOKABLE explicit IrcNoticeMessage(IrcSession* session);

    QString target() const;
    QString message() const;
    bool isReply() const;

    bool isValid() const;

private:
    Q_DISABLE_COPY(IrcNoticeMessage)
};

class IrcPingMessage : public IrcMessage
{
    Q_OBJECT
    Q_PROPERTY(QString argument READ argument)

public:
    Q_INVOKABLE explicit IrcPingMessage(IrcSession* session);

    QString argument() const;

    bool isValid() const;

private:
    Q_DISABLE_COPY(IrcPingMessage)
};

class IrcPongMessage : public IrcMessage
{
    Q_OBJECT
    Q_PROPERTY(QString argument READ argument)

public:
    Q_INVOKABLE explicit IrcPongMessage(IrcSession* session);

    QString argument() const;

    bool isValid() const;

private:
    Q_DISABLE_COPY(IrcPongMessage)
};

class IrcErrorMessage : public IrcMessage
{
    Q_OBJECT
    Q_PROPERTY(QString error READ error)

public:
    Q_INVOKABLE explicit IrcErrorMessage(IrcSession* session);

    QString error() const;

    bool isValid() const;

private:
    Q_DISABLE_COPY(IrcErrorMessage)
};

class IrcNumericMessage : public IrcMessage
{
    Q_OBJECT
    Q_PROPERTY(int code READ code)

public:
    Q_INVOKABLE explicit IrcNumericMessage(IrcSession* session);

    int code() const;

    bool isValid() const;

private:
    Q_DISABLE_COPY(IrcNumericMessage)
};

class IrcCapabilityMessage : public IrcMessage
{
    Q_OBJECT
    Q_PROPERTY(QString subCommand READ subCommand)
    Q_PROPERTY(QStringList capabilities READ capabilities)

public:
    Q_INVOKABLE explicit IrcCapabilityMessage(IrcSession* session);

    QString subCommand() const;
    QStringList capabilities() const;

    bool isValid() const;

private:
    Q_DISABLE_COPY(IrcCapabilityMessage)
};

#ifndef QT_NO_DEBUG_STREAM
QDebug operator<<(QDebug debug, const IrcMessage* message);
#endif // QT_NO_DEBUG_STREAM

#endif // IRCMESSAGE_H
