#include "animationviewmodel.h"
#include <QPixmap>
#include <QBuffer>
#include <QMovie>
#include <QTextDocument>

AnimationViewModel::AnimationViewModel(QByteArray bytes, QUrl url, QTextDocument *document, QObject *parent) : QThread(parent)
{
    buffer.open(QBuffer::ReadWrite);
    buffer.write(bytes);
    buffer.seek(0);
    movie = new QMovie(&buffer);
    QPixmap map;
    map.loadFromData(bytes);
    if(!map.isNull()) {
        movie->setScaledSize(QSize((150 * map.size().width()) / map.size().height(), 150));
    }
    movieUrl = url;
    textDocument = document;
    connect(movie, SIGNAL(frameChanged(int)), this, SLOT(movieAnimated(int)));
    movie->start();
}

void AnimationViewModel::movieAnimated(int frame)
{
    QPixmap map = movie->currentPixmap();
    textDocument->addResource(QTextDocument::ImageResource, movieUrl, map);
    emit(movieAnimated());
}
