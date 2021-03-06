#include "channel.h"
#include "server.h"
#include "user.h"
#include "session.h"
#include "irccommand.h"

Channel::Channel(QString inName, ChannelType type, QStandardItem *inMenuItem, Server *parent) : QObject(parent)
{
    text = "<body>";
    users = new QStandardItemModel(this);
    menuItem = inMenuItem;
    this->setName(inName);
    this->setType(type);
    if(type == ChannelTypeNormal) {
        this->setIsJoined(false);
    } else {
        this->setIsJoined(true);
    }
}

Channel::~Channel()
{

}

QString Channel::getName() {
    return name;
}

void Channel::setName(QString inName) {
    name = inName;
    menuItem->setText(inName);
}

QString Channel::getText() {
    return text;
}

void Channel::appendText(QString inText) {
    this->appendText("", inText, Channel::Info);
}

void Channel::appendText(QString sender, QString inText, MessageType type) {
    // Postpend post with images in image tags
    QString postpendedImageTags = "";
    QStringList foundLinks;
    QRegExp imageRegex(".*(href=\"(([^>]+)\\.*)\").*");
    int pos = 0;
    while ((pos = imageRegex.indexIn(inText, pos)) != -1) {
        QString imageUrl = imageRegex.cap(2);
        if(!imageUrl.startsWith("http", Qt::CaseInsensitive)) {
            imageUrl = "http://" + imageUrl;
        }
        postpendedImageTags += QString("<br /><a href=\"%1\"><img src=\"%1\" /></a>").arg(imageUrl);
        foundLinks.append(imageUrl);
        pos += imageRegex.matchedLength();
    }
    inText += postpendedImageTags;

    // Build HTML wrapper for message
    Server *server = this->getServer();
    QString currentUser = server->getNickname();
    bool textContainsUser = inText.contains(currentUser, Qt::CaseInsensitive);
    QDateTime currentTime = QDateTime::currentDateTime();
    QString currentTimeStr = currentTime.toString("h:mmap");

    QString tableRow = "";
    if (type == Channel::Emote)
    {
        tableRow = "<table class=\"msg-emote\" width=\"100%\"><tr>";
    }
    else if (type == Channel::Topic)
    {
        tableRow = "<table class=\"msg-topic\" width=\"100%\"><tr>";
    }
    else if (type == Channel::Info)
    {
        tableRow = "<table class=\"msg-info\" width=\"100%\"><tr>";
    }
    else if (textContainsUser)
    {
        tableRow = "<table class=\"msg-mentioned\" width=\"100%\"><tr>";
    }
    else
    {
        tableRow = "<table width=\"100%\"><tr>";
    }
        tableRow += "<th class=\"col-name\" width=\"140\" align=\"right\"><span class=\"user\">" + sender + "</span></th>";
        tableRow += "<td class=\"col-message\"><p class=\"message\">" + inText + "</p></td>";
        tableRow += "<td class=\"col-meta\" width=\"50\"><h6 class=\"metainfo\">" + currentTimeStr +"</h6></td>";
        tableRow += "</tr></table>";
    text += tableRow;
    Session *session = server->getSession();
    session->emitMessageReceived(server, this, tableRow, foundLinks, type);
}

Channel::ChannelType Channel::getType()
{
    return type;
}

void Channel::setType(Channel::ChannelType inType)
{
    type = inType;
}

bool Channel::getIsJoined()
{
    return isJoined;
}

void Channel::setIsJoined(bool inIsJoined)
{
    isJoined = inIsJoined;
    if(isJoined) {
        menuItem->setForeground(QBrush((QColor(255,255,255))));
    } else {
        menuItem->setForeground(QBrush((QColor(125,125,125))));
        users->clear();
    }
}

void Channel::join()
{
    Server *server = this->getServer();
    IrcCommand *command = IrcCommand::createJoin(name, NULL);
    server->sendCommand(command);
}

void Channel::part()
{
    Server *server = this->getServer();
    IrcCommand *command = IrcCommand::createPart(name, NULL);
    server->sendCommand(command);
}

QStandardItemModel* Channel::getUsers() {
    return users;
}

void Channel::addUsers(QStringList inUsers) {
    QRegExp nickRegex("^(~|&|@|%|\\+)?(.*)");
    for (int i = 0; i < inUsers.count(); i++) {
        QString newUserName = inUsers[i];
        int pos = nickRegex.indexIn(newUserName);
        QString namePart;
        QChar flagPart;
        if(pos > -1) {
            QString flagPartString = nickRegex.cap(1);
            if(flagPartString.length() > 0) {
                flagPart = nickRegex.cap(1).at(0);
            }
            namePart = nickRegex.cap(2);
        } else {
            namePart = newUserName;
        }
        this->addUser(namePart, flagPart);
    }
}

User* Channel::addUser(QString inUser, QChar prefix) {
    QStandardItem *newMenuItem = new QStandardItem();
    User *newUser = new User(inUser, prefix, newMenuItem, this);
    newMenuItem->setData(QVariant::fromValue<User*>(newUser), Qt::UserRole);
    users->appendRow(newMenuItem);
    this->sortUsers();
    return newUser;
}

void Channel::sortUsers()
{
    users->setSortRole(User::UserDataSort);
    users->sort(0);
}

void Channel::removeUser(QString inUser) {
    User* user = getUser(inUser);
    if(user != NULL) {
        QStandardItem *menuItem = user->getMenuItem();
        int row = menuItem->row();
        users->removeRow(row);
        user->deleteLater();
    }
}

QStandardItem* Channel::getUserMenuItem(QString inUser)
{
    QModelIndex startIndex = users->index(0, 0);
    QModelIndexList foundUsers = users->match(startIndex, User::UserDataName, inUser, -1, Qt::MatchExactly);
    if(foundUsers.count() == 1) {
        QStandardItem *user = users->itemFromIndex(foundUsers.at(0));
        return user;
    }
    return NULL;
}

User* Channel::getUser(QString inUser)
{
    QStandardItem *user = getUserMenuItem(inUser);
    if(user != NULL) {
        QVariant data = user->data(Qt::UserRole);
        return data.value<User*>();
    }
    return NULL;
}

QStringList Channel::findUserName(QString searchStr)
{
    QStringList userList = QStringList();

    QModelIndex startIndex = users->index(0, 0);
    QModelIndexList foundUsers = users->match(startIndex, User::UserDataName, searchStr, -1, Qt::MatchStartsWith);
    for(QModelIndexList::Iterator iter = foundUsers.begin(); iter != foundUsers.end(); iter++) {
        QStandardItem *user = users->itemFromIndex(*iter);
        QString username = user->data(User::UserDataName).value<QString>();
        userList.append(username);
    }

    return userList;
}

Server* Channel::getServer() {
    return qobject_cast<Server *>(this->parent());
}

QStandardItem* Channel::getMenuItem()
{
    return menuItem;
}

bool Channel::isChannel(QString name)
{
    return (name.startsWith("&") || name.startsWith("#") ||
            name.startsWith("+") || name.startsWith("!"));
}
