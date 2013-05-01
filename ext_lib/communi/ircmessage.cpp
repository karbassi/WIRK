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

#include "ircmessage.h"
#include "ircmessage_p.h"
#include "ircsession.h"
#include "ircsession_p.h"
#include "irccommand.h"
#include <QVariant>
#include <QDebug>

/*!
    \file ircmessage.h
    \brief #include &lt;IrcMessage&gt;
 */

/*!
    \class IrcMessage ircmessage.h <IrcMessage>
    \ingroup core
    \ingroup message
    \brief The IrcMessage class is the base class of all IRC message classes.

    IRC messages are received from an IRC server. IrcSession translates received
    messages into IrcMessage instances and emits the IrcSession::messageReceived()
    signal upon message received.

    Subclasses of IrcMessage contain specialized accessors for parameters that
    are specific to the particular type of message. See IrcMessage::Type for
    the list of supported message types.

    \sa IrcSession::messageReceived(), IrcMessage::Type
 */

/*!
    \enum IrcMessage::Type
    This enum describes the supported message types.
 */

/*!
    \var IrcMessage::Unknown
    \brief An unknown message (IrcMessage).
 */

/*!
    \var IrcMessage::Nick
    \brief A nick message (IrcNickMessage).
 */

/*!
    \var IrcMessage::Quit
    \brief A quit message (IrcQuitMessage).
 */

/*!
    \var IrcMessage::Join
    \brief A join message (IrcJoinMessage).
 */

/*!
    \var IrcMessage::Part
    \brief A part message (IrcPartMessage).
 */

/*!
    \var IrcMessage::Topic
    \brief A topic message (IrcTopicMessage).
 */

/*!
    \var IrcMessage::Invite
    \brief An invite message (IrcInviteMessage).
 */

/*!
    \var IrcMessage::Kick
    \brief A kick message (IrcKickMessage).
 */

/*!
    \var IrcMessage::Mode
    \brief A mode message (IrcModeMessage).
 */

/*!
    \var IrcMessage::Private
    \brief A private message (IrcPrivateMessage).
 */

/*!
    \var IrcMessage::Notice
    \brief A notice message (IrcNoticeMessage).
 */

/*!
    \var IrcMessage::Ping
    \brief A ping message (IrcPingMessage).
 */

/*!
    \var IrcMessage::Pong
    \brief A pong message (IrcPongMessage).
 */

/*!
    \var IrcMessage::Error
    \brief An error message (IrcErrorMessage).
 */

/*!
    \var IrcMessage::Numeric
    \brief A numeric message (IrcNumericMessage).
 */

/*!
    \enum IrcMessage::Flags
    This enum describes the supported message flags.
 */

/*!
    \var IrcMessage::None
    \brief The message has no flags.
 */

/*!
    \var IrcMessage::Own
    \brief The message is user's own message.
 */

/*!
    \var IrcMessage::Identified
    \brief The message is identified.
 */

/*!
    \var IrcMessage::Unidentified
    \brief The message is unidentified.
 */

static const QMetaObject* irc_command_meta_object(const QString& command)
{
    static QHash<QString, const QMetaObject*> metaObjects;
    if (metaObjects.isEmpty()) {
        metaObjects.insert("NICK", &IrcNickMessage::staticMetaObject);
        metaObjects.insert("QUIT", &IrcQuitMessage::staticMetaObject);
        metaObjects.insert("JOIN", &IrcJoinMessage::staticMetaObject);
        metaObjects.insert("PART", &IrcPartMessage::staticMetaObject);
        metaObjects.insert("TOPIC", &IrcTopicMessage::staticMetaObject);
        metaObjects.insert("INVITE", &IrcInviteMessage::staticMetaObject);
        metaObjects.insert("KICK", &IrcKickMessage::staticMetaObject);
        metaObjects.insert("MODE", &IrcModeMessage::staticMetaObject);
        metaObjects.insert("PRIVMSG", &IrcPrivateMessage::staticMetaObject);
        metaObjects.insert("NOTICE", &IrcNoticeMessage::staticMetaObject);
        metaObjects.insert("PING", &IrcPingMessage::staticMetaObject);
        metaObjects.insert("PONG", &IrcPongMessage::staticMetaObject);
        metaObjects.insert("ERROR", &IrcErrorMessage::staticMetaObject);
        metaObjects.insert("CAP", &IrcCapabilityMessage::staticMetaObject);
    }

    const QMetaObject* metaObject = metaObjects.value(command.toUpper());
    if (!metaObject) {
        bool ok = false;
        command.toInt(&ok);
        if (ok)
            metaObject = &IrcNumericMessage::staticMetaObject;
    }
    if (!metaObject)
        metaObject = &IrcMessage::staticMetaObject;
    return metaObject;
}

/*!
    Constructs a new IrcMessage with \a session.
 */
IrcMessage::IrcMessage(IrcSession* session) : QObject(session), d_ptr(new IrcMessagePrivate)
{
    Q_D(IrcMessage);
    d->session = session;
}

/*!
    Destructs the IRC message.
 */
IrcMessage::~IrcMessage()
{
}

/*!
    This property holds the message session.

    \par Access functions:
    \li IrcSession* <b>session</b>() const
 */
IrcSession* IrcMessage::session() const
{
    Q_D(const IrcMessage);
    return d->session;
}

/*!
    This property holds the message type.

    \par Access functions:
    \li IrcMessage::Type <b>type</b>() const
 */
IrcMessage::Type IrcMessage::type() const
{
    Q_D(const IrcMessage);
    return d->type;
}

/*!
    This property holds the message flags.

    \par Access functions:
    \li IrcMessage::Flags <b>flags</b>() const
 */
IrcMessage::Flags IrcMessage::flags() const
{
    Q_D(const IrcMessage);
    if (d->flags == -1) {
        d->flags = IrcMessage::None;
        IrcSender sender = d->sender();
        if (sender.isValid() && sender.name() == d->session->nickName())
            d->flags |= IrcMessage::Own;

        if ((d->type == IrcMessage::Private || d->type == IrcMessage::Notice) &&
                IrcSessionPrivate::get(d->session)->capabilities.contains("identify-msg")) {
            QString msg = property("message").toString();
            if (msg.startsWith("+"))
                d->flags |= IrcMessage::Identified;
            else if (msg.startsWith("-"))
                d->flags |= IrcMessage::Unidentified;
        }
    }
    return IrcMessage::Flags(d->flags);
}

/*!
    This property holds the message command.

    \par Access functions:
    \li QString <b>command</b>() const
 */
QString IrcMessage::command() const
{
    Q_D(const IrcMessage);
    return d->command();
}

/*!
    This property holds the message sender.

    \par Access functions:
    \li IrcSender <b>sender</b>() const
    \li void <b>setSender</b>(const IrcSender& sender)
 */
IrcSender IrcMessage::sender() const
{
    Q_D(const IrcMessage);
    return d->sender();
}

void IrcMessage::setSender(const IrcSender& sender)
{
    Q_D(IrcMessage);
    d->message.prefix = sender.prefix().toUtf8();
    d->content.dirty = true;
}

/*!
    This property holds the message parameters.

    \par Access functions:
    \li QStringList <b>parameters</b>() const
    \li void <b>setParameters</b>(const QStringList& parameters)
 */
QStringList IrcMessage::parameters() const
{
    Q_D(const IrcMessage);
    return d->params();
}

void IrcMessage::setParameters(const QStringList& parameters)
{
    Q_D(IrcMessage);
    d->message.params.clear();
    foreach (const QString& param, parameters)
        d->message.params += param.toUtf8();
    d->content.dirty = true;
}

/*!
    This property holds the message time stamp.

    \par Access functions:
    \li QDateTime <b>timeStamp</b>() const
    \li void <b>setTimeStamp</b>(const QDateTime& timeStamp)
 */
QDateTime IrcMessage::timeStamp() const
{
    Q_D(const IrcMessage);
    return d->timeStamp;
}

void IrcMessage::setTimeStamp(const QDateTime& timeStamp)
{
    Q_D(IrcMessage);
    d->timeStamp = timeStamp;
}

/*!
    This property holds the FALLBACK encoding for the message.

    The fallback encoding is used when the message is detected not
    to be valid UTF-8 and the consequent auto-detection of message
    encoding fails. See QTextCodec::availableCodes() for the list of
    supported encodings.

    The default value is ISO-8859-15.

    \par Access functions:
    \li QByteArray <b>encoding</b>() const
    \li void <b>setEncoding</b>(const QByteArray& encoding)

    \sa QTextCodec::availableCodecs(), QTextCodec::codecForLocale()
 */
QByteArray IrcMessage::encoding() const
{
    Q_D(const IrcMessage);
    return d->encoding;
}

void IrcMessage::setEncoding(const QByteArray& encoding)
{
    Q_D(IrcMessage);
    extern bool irc_is_supported_encoding(const QByteArray& encoding); // ircmessagedecoder.cpp
    if (!irc_is_supported_encoding(encoding)) {
        qWarning() << "IrcMessage::setEncoding(): unsupported encoding" << encoding;
        return;
    }
    d->encoding = encoding;
    d->content.dirty = true;
}

/*!
    Creates a new message from \a data and \a session.
 */
IrcMessage* IrcMessage::fromData(const QByteArray& data, IrcSession* session)
{
    IrcMessage* message = 0;

    IrcMessageData messageData = IrcMessageData::fromData(data);
    if (messageData.valid) {
        const QMetaObject* metaObject = irc_command_meta_object(messageData.command);
        Q_ASSERT(metaObject);
        message = qobject_cast<IrcMessage*>(metaObject->newInstance(Q_ARG(IrcSession*, session)));
        Q_ASSERT(message);
        message->d_ptr->message = messageData;
    }
    return message;
}

/*!
    Creates a new message from \a sender and \a command with \a session.
 */
IrcMessage* IrcMessage::fromCommand(const QString& sender, IrcCommand* command, IrcSession* session)
{
    return fromData(":" + sender.toUtf8() + " " + command->toString().toUtf8(), session);
}

/*!
    Creates a new message from \a sender, \a command and \a parameters with \a session.
 */
IrcMessage* IrcMessage::fromParameters(const QString& sender, const QString& command, const QStringList& parameters, IrcSession* session)
{
    const QMetaObject* metaObject = irc_command_meta_object(command);
    Q_ASSERT(metaObject);
    IrcMessage* message = qobject_cast<IrcMessage*>(metaObject->newInstance(Q_ARG(IrcSession*, session)));
    Q_ASSERT(message);

    IrcMessageData data;
    data.prefix = sender.toUtf8();
    data.command = command.toUtf8();
    foreach (const QString& param, parameters)
        data.params += param.toUtf8();
    data.valid = !command.isEmpty();
    message->d_ptr->message = data;
    return message;
}

/*!
    \property bool IrcMessage::valid
    This property is \c true if the message is valid; otherwise \c false.

    A message is considered valid if the sender is valid
    and the parameters match the message.

    \par Access functions:
    \li bool <b>isValid</b>() const
 */
bool IrcMessage::isValid() const
{
    Q_D(const IrcMessage);
    return d->session && d->message.valid && sender().isValid();
}

/*!
    Returns the message as received from an IRC server.
 */
QByteArray IrcMessage::toData() const
{
    Q_D(const IrcMessage);
    return d->message.data;
}

/*!
    \class IrcNickMessage ircmessage.h <IrcMessage>
    \ingroup message
    \brief The IrcNickMessage class represents a nick IRC message.
 */

/*!
    Constructs a new IrcNickMessage with \a session.
 */
IrcNickMessage::IrcNickMessage(IrcSession* session) : IrcMessage(session)
{
    Q_D(IrcMessage);
    d->type = Nick;
}

/*!
    This property holds the new nick.

    \par Access functions:
    \li QString <b>nick</b>() const
 */
QString IrcNickMessage::nick() const
{
    Q_D(const IrcMessage);
    return d->param(0);
}

bool IrcNickMessage::isValid() const
{
    return IrcMessage::isValid() && !nick().isEmpty();
}

/*!
    \class IrcQuitMessage ircmessage.h <IrcMessage>
    \ingroup message
    \brief The IrcQuitMessage class represents a quit IRC message.
 */

/*!
    Constructs a new IrcQuitMessage with \a session.
 */
IrcQuitMessage::IrcQuitMessage(IrcSession* session) : IrcMessage(session)
{
    Q_D(IrcMessage);
    d->type = Quit;
}

/*!
    This property holds the optional quit reason.

    \par Access functions:
    \li QString <b>reason</b>() const
 */
QString IrcQuitMessage::reason() const
{
    Q_D(const IrcMessage);
    return d->param(0);
}

bool IrcQuitMessage::isValid() const
{
    return IrcMessage::isValid();
}

/*!
    \class IrcJoinMessage ircmessage.h <IrcMessage>
    \ingroup message
    \brief The IrcJoinMessage class represents a join IRC message.
 */

/*!
    Constructs a new IrcJoinMessage with \a session.
 */
IrcJoinMessage::IrcJoinMessage(IrcSession* session) : IrcMessage(session)
{
    Q_D(IrcMessage);
    d->type = Join;
}

/*!
    This property holds the channel in question.

    \par Access functions:
    \li QString <b>channel</b>() const
 */
QString IrcJoinMessage::channel() const
{
    Q_D(const IrcMessage);
    return d->param(0);
}

bool IrcJoinMessage::isValid() const
{
    return IrcMessage::isValid() && !channel().isEmpty();
}

/*!
    \class IrcPartMessage ircmessage.h <IrcMessage>
    \ingroup message
    \brief The IrcPartMessage class represents a part IRC message.
 */

/*!
    Constructs a new IrcPartMessage with \a session.
 */
IrcPartMessage::IrcPartMessage(IrcSession* session) : IrcMessage(session)
{
    Q_D(IrcMessage);
    d->type = Part;
}

/*!
    This property holds the channel in question.

    \par Access functions:
    \li QString <b>channel</b>() const
 */
QString IrcPartMessage::channel() const
{
    Q_D(const IrcMessage);
    return d->param(0);
}

/*!
    This property holds the optional part reason.

    \par Access functions:
    \li QString <b>reason</b>() const
 */
QString IrcPartMessage::reason() const
{
    Q_D(const IrcMessage);
    return d->param(1);
}

bool IrcPartMessage::isValid() const
{
    return IrcMessage::isValid() && !channel().isEmpty();
}

/*!
    \class IrcTopicMessage ircmessage.h <IrcMessage>
    \ingroup message
    \brief The IrcTopicMessage class represents a topic IRC message.
 */

/*!
    Constructs a new IrcTopicMessage with \a session.
 */
IrcTopicMessage::IrcTopicMessage(IrcSession* session) : IrcMessage(session)
{
    Q_D(IrcMessage);
    d->type = Topic;
}

/*!
    This property holds the channel in question.

    \par Access functions:
    \li QString <b>channel</b>() const
 */
QString IrcTopicMessage::channel() const
{
    Q_D(const IrcMessage);
    return d->param(0);
}

/*!
    This property holds the new channel topic.

    \par Access functions:
    \li QString <b>topic</b>() const
 */
QString IrcTopicMessage::topic() const
{
    Q_D(const IrcMessage);
    return d->param(1);
}

bool IrcTopicMessage::isValid() const
{
    return IrcMessage::isValid() && !channel().isEmpty();
}

/*!
    \class IrcInviteMessage ircmessage.h <IrcMessage>
    \ingroup message
    \brief The IrcInviteMessage class represents an invite IRC message.
 */

/*!
    Constructs a new IrcInviteMessage with \a session.
 */
IrcInviteMessage::IrcInviteMessage(IrcSession* session) : IrcMessage(session)
{
    Q_D(IrcMessage);
    d->type = Invite;
}

/*!
    This property holds the user in question.

    \par Access functions:
    \li QString <b>user</b>() const
 */
QString IrcInviteMessage::user() const
{
    Q_D(const IrcMessage);
    return d->param(0);
}

/*!
    This property holds the channel in question.

    \par Access functions:
    \li QString <b>channel</b>() const
 */
QString IrcInviteMessage::channel() const
{
    Q_D(const IrcMessage);
    return d->param(1);
}

bool IrcInviteMessage::isValid() const
{
    return IrcMessage::isValid() && !user().isEmpty() && !channel().isEmpty();
}

/*!
    \class IrcKickMessage ircmessage.h <IrcMessage>
    \ingroup message
    \brief The IrcKickMessage class represents a kick IRC message.
 */

/*!
    Constructs a new IrcKickMessage with \a session.
 */
IrcKickMessage::IrcKickMessage(IrcSession* session) : IrcMessage(session)
{
    Q_D(IrcMessage);
    d->type = Kick;
}

/*!
    This property holds the channel in question.

    \par Access functions:
    \li QString <b>channel</b>() const
 */
QString IrcKickMessage::channel() const
{
    Q_D(const IrcMessage);
    return d->param(0);
}

/*!
    This property holds the user in question.

    \par Access functions:
    \li QString <b>user</b>() const
 */
QString IrcKickMessage::user() const
{
    Q_D(const IrcMessage);
    return d->param(1);
}

/*!
    This property holds the optional kick reason.

    \par Access functions:
    \li QString <b>reason</b>() const
 */
QString IrcKickMessage::reason() const
{
    Q_D(const IrcMessage);
    return d->param(2);
}

bool IrcKickMessage::isValid() const
{
    return IrcMessage::isValid() && !channel().isEmpty() && !user().isEmpty();
}

/*!
    \class IrcModeMessage ircmessage.h <IrcMessage>
    \ingroup message
    \brief The IrcModeMessage class represents a mode IRC message.
 */

/*!
    Constructs a new IrcModeMessage with \a session.
 */
IrcModeMessage::IrcModeMessage(IrcSession* session) : IrcMessage(session)
{
    Q_D(IrcMessage);
    d->type = Mode;
}

/*!
    This property holds the target channel or user in question.

    \par Access functions:
    \li QString <b>target</b>() const
 */
QString IrcModeMessage::target() const
{
    Q_D(const IrcMessage);
    return d->param(0);
}

/*!
    This property holds the channel or user mode.

    \par Access functions:
    \li QString <b>mode</b>() const
 */
QString IrcModeMessage::mode() const
{
    Q_D(const IrcMessage);
    return d->param(1);
}

/*!
    This property holds the mode argument.

    \par Access functions:
    \li QString <b>argument</b>() const
 */
QString IrcModeMessage::argument() const
{
    Q_D(const IrcMessage);
    return d->param(2);
}

bool IrcModeMessage::isValid() const
{
    return IrcMessage::isValid() && !target().isEmpty() && !mode().isEmpty();
}

/*!
    \class IrcPrivateMessage ircmessage.h <IrcMessage>
    \ingroup message
    \brief The IrcPrivateMessage class represents a private IRC message.
 */

/*!
    Constructs a new IrcPrivateMessage with \a session.
 */
IrcPrivateMessage::IrcPrivateMessage(IrcSession* session) : IrcMessage(session)
{
    Q_D(IrcMessage);
    d->type = Private;
}

/*!
    This property holds the target channel or user in question.

    \par Access functions:
    \li QString <b>target</b>() const
 */
QString IrcPrivateMessage::target() const
{
    Q_D(const IrcMessage);
    return d->param(0);
}

/*!
    This property holds the message.

    \par Access functions:
    \li QString <b>message</b>() const
 */
QString IrcPrivateMessage::message() const
{
    Q_D(const IrcMessage);
    QString msg = d->param(1);
    if (flags() & (Identified | Unidentified))
        msg.remove(0, 1);
    const bool act = isAction();
    const bool req = isRequest();
    if (act) msg.remove(0, 8);
    if (req) msg.remove(0, 1);
    if (act || req) msg.chop(1);
    return msg;
}

/*!
    \property bool IrcPrivateMessage::action
    This property is \c true if the message is an action; otherwise \c false.

    \par Access functions:
    \li bool <b>isAction</b>() const
 */
bool IrcPrivateMessage::isAction() const
{
    Q_D(const IrcMessage);
    QByteArray msg = d->message.params.value(1);
    if (flags() & (Identified | Unidentified))
        msg.remove(0, 1);
    return msg.startsWith("\1ACTION ") && msg.endsWith('\1');
}

/*!
    \property bool IrcPrivateMessage::request
    This property is \c true if the message is a request; otherwise \c false.

    \par Access functions:
    \li bool <b>isRequest</b>() const
 */
bool IrcPrivateMessage::isRequest() const
{
    Q_D(const IrcMessage);
    QByteArray msg = d->message.params.value(1);
    if (flags() & (Identified | Unidentified))
        msg.remove(0, 1);
    return msg.startsWith('\1') && msg.endsWith('\1') && !isAction();
}

bool IrcPrivateMessage::isValid() const
{
    return IrcMessage::isValid() && !target().isEmpty() && !message().isEmpty();
}

/*!
    \class IrcNoticeMessage ircmessage.h <IrcMessage>
    \ingroup message
    \brief The IrcNoticeMessage class represents a notice IRC message.
 */

/*!
    Constructs a new IrcNoticeMessage with \a session.
 */
IrcNoticeMessage::IrcNoticeMessage(IrcSession* session) : IrcMessage(session)
{
    Q_D(IrcMessage);
    d->type = Notice;
}

/*!
    This property holds the target channel or user in question.

    \par Access functions:
    \li QString <b>target</b>() const
 */
QString IrcNoticeMessage::target() const
{
    Q_D(const IrcMessage);
    return d->param(0);
}

/*!
    This property holds the message.

    \par Access functions:
    \li QString <b>message</b>() const
 */
QString IrcNoticeMessage::message() const
{
    Q_D(const IrcMessage);
    QString msg = d->param(1);
    if (flags() & (Identified | Unidentified))
        msg.remove(0, 1);
    if (isReply()) {
        msg.remove(0, 1);
        msg.chop(1);
    }
    return msg;
}

/*!
    \property bool IrcNoticeMessage::reply
    This property is \c true if the message is a reply; otherwise \c false.

    \par Access functions:
    \li bool <b>isReply</b>() const
 */
bool IrcNoticeMessage::isReply() const
{
    Q_D(const IrcMessage);
    return d->message.params.value(1).startsWith('\1') && d->message.params.value(1).endsWith('\1');
}

bool IrcNoticeMessage::isValid() const
{
    return IrcMessage::isValid() && !target().isEmpty() && !message().isEmpty();
}

/*!
    \class IrcPingMessage ircmessage.h <IrcMessage>
    \ingroup message
    \brief The IrcPingMessage class represents a ping IRC message.
 */

/*!
    Constructs a new IrcPingMessage with \a session.
 */
IrcPingMessage::IrcPingMessage(IrcSession* session) : IrcMessage(session)
{
    Q_D(IrcMessage);
    d->type = Ping;
}

/*!
    This property holds the optional message argument.

    \par Access functions:
    \li QString <b>argument</b>() const
 */
QString IrcPingMessage::argument() const
{
    Q_D(const IrcMessage);
    return d->param(0);
}

bool IrcPingMessage::isValid() const
{
    return IrcMessage::isValid();
}

/*!
    \class IrcPongMessage ircmessage.h <IrcMessage>
    \ingroup message
    \brief The IrcPongMessage class represents a pong IRC message.
 */

/*!
    Constructs a new IrcPongMessage with \a session.
 */
IrcPongMessage::IrcPongMessage(IrcSession* session) : IrcMessage(session)
{
    Q_D(IrcMessage);
    d->type = Pong;
}

/*!
    This property holds the optional message argument.

    \par Access functions:
    \li QString <b>argument</b>() const
 */
QString IrcPongMessage::argument() const
{
    Q_D(const IrcMessage);
    return d->param(1);
}

bool IrcPongMessage::isValid() const
{
    return IrcMessage::isValid();
}

/*!
    \class IrcErrorMessage ircmessage.h <IrcMessage>
    \ingroup message
    \brief The IrcErrorMessage class represents an error IRC message.
 */

/*!
    Constructs a new IrcErrorMessage with \a session.
 */
IrcErrorMessage::IrcErrorMessage(IrcSession* session) : IrcMessage(session)
{
    Q_D(IrcMessage);
    d->type = Error;
}

/*!
    This property holds the error.

    \par Access functions:
    \li QString <b>error</b>() const
 */
QString IrcErrorMessage::error() const
{
    Q_D(const IrcMessage);
    return d->param(0);
}

bool IrcErrorMessage::isValid() const
{
    return IrcMessage::isValid() && !error().isEmpty();
}

/*!
    \class IrcNumericMessage ircmessage.h <IrcMessage>
    \ingroup message
    \brief The IrcNumericMessage class represents a numeric IRC message.
 */

/*!
    Constructs a new IrcNumericMessage with \a session.
 */
IrcNumericMessage::IrcNumericMessage(IrcSession* session) : IrcMessage(session)
{
    Q_D(IrcMessage);
    d->type = Numeric;
}

/*!
    This property holds the numeric code.

    \par Access functions:
    \li int <b>code</b>() const
 */
int IrcNumericMessage::code() const
{
    Q_D(const IrcMessage);
    bool ok = false;
    int number = d->message.command.toInt(&ok);
    return ok ? number : -1;
}

bool IrcNumericMessage::isValid() const
{
    return IrcMessage::isValid() && code() != -1;
}

/*!
    \class IrcCapabilityMessage ircmessage.h <IrcMessage>
    \ingroup message
    \brief The IrcCapabilityMessage class represents a capability IRC message.
 */

/*!
    Constructs a new IrcCapabilityMessage with \a session.
 */
IrcCapabilityMessage::IrcCapabilityMessage(IrcSession* session) : IrcMessage(session)
{
    Q_D(IrcMessage);
    d->type = Capability;
}

/*!
    This property holds the subcommand.

    The defined capability subcommands are:
    LS, LIST, REQ, ACK, NAK, CLEAR, END

    \par Access functions:
    \li QString <b>subCommand</b>() const
 */
QString IrcCapabilityMessage::subCommand() const
{
    Q_D(const IrcMessage);
    return d->param(1);
}

/*!
    This property holds the subcommand.

    The following capability subcommands are defined:
    LS, LIST, REQ, ACK, NAK, CLEAR, END

    \par Access functions:
    \li QString <b>subCommand</b>() const
 */
QStringList IrcCapabilityMessage::capabilities() const
{
    Q_D(const IrcMessage);
    QStringList caps;
    QStringList params = d->params();
    if (params.count() > 2)
        caps = params.last().split(QLatin1Char(' '), QString::SkipEmptyParts);
    return caps;
}

/*!
    This property holds the capabilities.

    A list of capabilities may exist for the following
    subcommands: LS, LIST, REQ, ACK and NAK.

    \par Access functions:
    \li QStringList <b>capabilities</b>() const
 */
bool IrcCapabilityMessage::isValid() const
{
    return IrcMessage::isValid();
}

#ifndef QT_NO_DEBUG_STREAM
QDebug operator<<(QDebug debug, const IrcMessage* message)
{
    if (!message)
        return debug << "IrcMessage(0x0) ";
    debug.nospace() << message->metaObject()->className() << '(' << (void*) message;
    QStringList flags;
    if (message->flags() == IrcMessage::None)
        flags << "None";
    else if (message->flags() & IrcMessage::Identified)
        flags << "Identified";
    else if (message->flags() & IrcMessage::Unidentified)
        flags << "Unidentified";
    debug << ", flags = " << flags;
    if (!message->objectName().isEmpty())
        debug << ", name = " << message->objectName();
    if (message->sender().isValid())
        debug << ", sender = " << message->sender().name();
    if (!message->command().isEmpty())
        debug << ", command = " << message->command();
    if (!message->parameters().isEmpty())
        debug << ", params = " << message->parameters();
    debug.nospace() << ')';
    return debug.space();
}
#endif // QT_NO_DEBUG_STREAM
