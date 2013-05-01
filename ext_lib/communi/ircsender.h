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

#ifndef IRCSENDER_H
#define IRCSENDER_H

#include <QtCore/qstring.h>
#include <QtCore/qmetatype.h>
#include <QtCore/qshareddata.h>

class IrcSenderPrivate;

class IrcSender
{
public:
    IrcSender();
    IrcSender(const QString& prefix);
    IrcSender(const IrcSender& other);
    IrcSender& operator=(const IrcSender& other);
    ~IrcSender();

    bool isValid() const;

    QString prefix() const;
    void setPrefix(const QString& prefix);

    QString name() const;
    void setName(const QString& name);

    QString user() const;
    void setUser(const QString& user);

    QString host() const;
    void setHost(const QString& host);

private:
    mutable QSharedDataPointer<IrcSenderPrivate> d;
};

Q_DECLARE_METATYPE(IrcSender)

#endif // IRCSENDER_H
