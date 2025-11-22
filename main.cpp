#include <QApplication>
#include <QPushButton>
#include <QMediaPlayer>
#include <QAudioOutput>
#include <QFileDialog>
#include <QFileInfo>
#include <QVBoxLayout>
#include <QWidget>
#include <QLabel>
#include <QTimer>
#include <QPixmap>
#include <QPainter>
#include <QTransform>
#include <QMouseEvent>
#include <QtMath>
#include "DiscLabel.h"
#include "main.moc"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    QWidget window;
    window.setWindowTitle("mixplayer");
    window.setFixedSize(300, 450);

    QVBoxLayout *layout = new QVBoxLayout;

    QMediaPlayer *player = new QMediaPlayer;
    QAudioOutput *audioOutput = new QAudioOutput;
    player->setAudioOutput(audioOutput);
    audioOutput->setVolume(0.8);

    DiscLabel *discLabel = new DiscLabel;
    QPixmap discPixmap("disc.png");
    discPixmap = discPixmap.scaled(150,150,Qt::KeepAspectRatio, Qt::SmoothTransformation);
    discLabel->setPixmap(discPixmap);
    layout->addWidget(discLabel, 0, Qt::AlignHCenter);

    double angle = 0;
    double spinSpeed = 2.0;
    QTimer *timer = new QTimer;

    QObject::connect(timer, &QTimer::timeout, [=,&angle,&spinSpeed,&discPixmap,&discLabel,&player](){
        QPixmap canvas(150,150);
        canvas.fill(Qt::transparent);
        QPainter painter(&canvas);
        painter.setRenderHint(QPainter::SmoothPixmapTransform);
        QTransform transform;
        transform.translate(canvas.width()/2.0, canvas.height()/2.0);
        transform.rotate(angle);
        transform.translate(-discPixmap.width()/2.0, -discPixmap.height()/2.0);
        painter.setTransform(transform);
        painter.drawPixmap(0,0,discPixmap);
        discLabel->setPixmap(canvas);

        angle += spinSpeed;
        if(angle >= 360) angle -= 360;

        double rate = player->playbackRate();
        player->setPlaybackRate(rate + (1.0 - rate) * 0.1);
    });

    QObject::connect(discLabel, &DiscLabel::spinAdjusted, [&](double deltaAngle){
        double factor = deltaAngle * 1.0;
        angle += deltaAngle * 180.0 / M_PI;

        if(player->playbackState() == QMediaPlayer::PlayingState) {
            static double currentRate = 1.0;
            double targetRate = player->playbackRate() + factor;
            targetRate = qBound(0.1, targetRate, 3.0);
            currentRate += (targetRate - currentRate) * 0.2;
            player->setPlaybackRate(currentRate);

        } else {
            qint64 newPos = player->position() + factor * 1;
            if(newPos < 0) newPos = 0;
            if(newPos > player->duration()) newPos = player->duration();
            player->setPosition(newPos);
        }
    });

    QPushButton *playButton = new QPushButton("Play");
    QPushButton *stopButton = new QPushButton("Stop");
    QPushButton *openButton = new QPushButton("Open Music");
    layout->addWidget(playButton);
    layout->addWidget(stopButton);
    layout->addWidget(openButton);

    QObject::connect(playButton, &QPushButton::clicked, [=]() { player->play(); timer->start(50); });
    QObject::connect(stopButton, &QPushButton::clicked, [=]() { player->stop(); timer->stop(); });
    QObject::connect(openButton, &QPushButton::clicked, [&]() {
        QString file = QFileDialog::getOpenFileName(&window,"Select Music","","Audio Files (*.mp3 *.wav *.flac)");
        if(!file.isEmpty()) {
            player->setSource(QUrl::fromLocalFile(file));
            QFileInfo info(file);
            openButton->setText(info.fileName());
        }
    });

    window.setLayout(layout);
    window.show();
    return app.exec();
}
