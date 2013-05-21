#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QStandardItemModel>
#include <QStandardItem>
#include <QMapIterator>
#include "irc_channel.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // TODO:  Get this from settings
    irc_server *server = new irc_server(this);
    server->setHost("chat.freenode.net");
    server->setPort(7070);
    server->setUsername("testing1234567");
    server->setNickname("testing1234567");
    server->setRealname("Testing 1234567");
    connect(server, SIGNAL(textChanged(parsed_message*)), this, SLOT(displayMessage(parsed_message*)));
    connect(server, SIGNAL(channelChanged()), this, SLOT(channelChanged()));
    server->setSSL(true);
    server->createConnection();
    m_servers.append(server);

    // Hook up the sending text box
    connect(ui->sendText, SIGNAL(returnPressed()), this, SLOT(sendMessage()));

    // Setup Tree
    ui->treeView->setHeaderHidden(true);
    ui->treeView->setModel(this->generateTree());
}

MainWindow::~MainWindow()
{    
    delete ui;
}

QStandardItemModel* MainWindow::generateTree() {
    QStandardItemModel *treeModel = new QStandardItemModel();
    for(int i = 0; i < m_servers.count(); i++) {
        irc_server *server = m_servers[i];
        QStandardItem *server_node = new QStandardItem();
        server_node->setText(server->getHost());
        //server_node->setData(server, Qt::UserRole); TODO:  Why doesn't this work?
        treeModel->setItem(i, server_node);
        QMapIterator<QString, irc_channel*> j(server->getChannels());
        int index = 0;
        while (j.hasNext()) {
            j.next();
            irc_channel *channel = j.value();
            QStandardItem *channel_node = new QStandardItem();
            channel_node->setText(channel->getName());
            //channel_node->setData(channel, Qt::UserRole); TODO:  Why doesn't this work?
            server_node->setChild(index, channel_node);
            index++;
        }
    }
    return treeModel;
}

void MainWindow::channelChanged()
{
    // TODO:  Maybe do incremental updates?
    ui->treeView->setModel(this->generateTree());
}

void MainWindow::displayMessage(parsed_message *message)
{
    // TODO:  Only display text from currently selected tree item
    //        Highlight the item in the tree if it's not currently selected
    //        Maybe this object should have: server, channel, message?
    ui->mainText->setHtml(message->getMessage());

    // This scrolls the main text to the bottom
    QTextCursor c = ui->mainText->textCursor();
    c.movePosition(QTextCursor::End);
    ui->mainText->setTextCursor(c);
}

void MainWindow::sendMessage()
{
    m_servers[0]->sendMessage(ui->sendText->text()); // TODO: Send to selected server in tree
    ui->sendText->setText("");
}
