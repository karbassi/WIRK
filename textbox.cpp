#include "textbox.h"
#include <QKeyEvent>
#include <QDebug>
#include <QLineEdit>
#include "messagehistory.h"

TextBox::TextBox(QWidget *parent) : QLineEdit(parent)
{
    messageHistory = new MessageHistory(this);
    searchingUsernames = QStringList();
    userSearchIndex = 0;
    channel = NULL;
    searchString = "";
}

void TextBox::setChannel(Channel &chan)
{
    channel = &chan;
    userSearchIndex = 0;
    searchingUsernames = QStringList();
}

TextBox::~TextBox()
{

}

void TextBox::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Up || event->key() == Qt::Key_Down)
    {
        bool willCycleUp = event->key() == Qt::Key_Up;

        QString lastSent = messageHistory->getLastSentMessage(willCycleUp);
        this->setText(lastSent);
    }
    else {
        if (event->key() == Qt::Key_Enter || event->key() == Qt::Key_Return)
        {
            messageHistory->insertNewMessage(this->text());
        }

        QLineEdit::keyPressEvent(event);
    }
}

bool TextBox::event(QEvent *e)
{
    if(e->type() == QEvent::KeyPress) {
        QKeyEvent *ke = static_cast<QKeyEvent *>(e);
        // We listen to the tab press here because
        // it's not sent all the way to keyPressEvent
        if(ke->key() == Qt::Key_Tab) {
            getLastArgument();
            return true;
        } else {
            lastWord = "";
        }

    }

    return QLineEdit::event(e);
}

void TextBox::getLastArgument()
{
    QStringList messageList = this->text().split(QRegExp("\\s+"));

    if(!messageList.isEmpty() && lastWord == "") {
        lastWord = messageList.last();
    }

    if(lastWord == NULL || lastWord.isEmpty()) {
        return;
    }

    if(searchString != lastWord) {
        searchString = lastWord;
        searchingUsernames = QStringList();
    }

    if(searchingUsernames.isEmpty()) {
        userSearchIndex = 0;
        QStringList usernames = channel->findUserName(lastWord);
        for(QStringList::Iterator iter = usernames.begin(); iter != usernames.end(); iter++) {
            searchingUsernames.append(*iter);
        }
    } else {
        userSearchIndex++;
    }

    QString foundName = "";
    if(!searchingUsernames.isEmpty()) {
        foundName = searchingUsernames.at(userSearchIndex % searchingUsernames.length());
    } else {
        return;
    }

    QString fullMessage = "";
    for(QStringList::Iterator iter = messageList.begin(); iter != messageList.end()-1; iter++) {
        if(iter != messageList.begin()) {
            fullMessage += " ";
        }
        fullMessage += *iter;
    }

    if(!foundName.isEmpty()) {
        if(!fullMessage.isEmpty()) {
            fullMessage += " ";
        }
        fullMessage += foundName + ":";
    }

    this->setText(fullMessage);
}
