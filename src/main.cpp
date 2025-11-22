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
#include <QDebug>
#include "DiscLabel.h"
#include "main.moc"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    QWidget window;
    window.setWindowTitle("mixplayer");
    window.setFixedSize(300, 450);

    QVBoxLayout *layout = new QVBoxLayout;

    QMediaPlayer *player = new QMediaPlayer(&window);
    QMediaPlayer *sfxplayer = new QMediaPlayer(&window);
    QAudioOutput *audioOutput = new QAudioOutput(&window);
    QAudioOutput *sfxAudioOutput = new QAudioOutput(&window);
    player->setAudioOutput(audioOutput);
    sfxplayer->setAudioOutput(sfxAudioOutput);
    audioOutput->setVolume(0.8);
    sfxAudioOutput->setVolume(0.9);

    DiscLabel *discLabel = new DiscLabel;
    QPixmap discPixmap(":/disc.png");
    discPixmap = discPixmap.scaled(150, 150, Qt::KeepAspectRatio, Qt::SmoothTransformation);
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
    QPushButton *sfx1Button = new QPushButton("SFX 1");
    QPushButton *sfx2Button = new QPushButton("SFX 2");
    QPushButton *sfx3Button = new QPushButton("SFX 3");
    QPushButton *sfx4Button = new QPushButton("SFX 4");
    QPushButton *openButton = new QPushButton("Open Music");
    layout->addWidget(playButton);
    layout->addWidget(stopButton);
    layout->addWidget(sfx1Button);
    layout->addWidget(sfx2Button);
    layout->addWidget(sfx3Button);
    layout->addWidget(sfx4Button);
    layout->addWidget(openButton);
    playButton->setEnabled(false);
    stopButton->setEnabled(false);

    QObject::connect(playButton, &QPushButton::clicked, [=]() mutable {
        if(player->playbackState() == QMediaPlayer::PlayingState) {
            player->pause();
            timer->stop();
            playButton->setText("Play");
        } else {
            player->play();
            timer->start(50);
            playButton->setText("Pause");
        }
    });

    QObject::connect(stopButton, &QPushButton::clicked, [=]() { player->stop(); timer->stop(); playButton->setText("Play"); });
    QObject::connect(sfx1Button, &QPushButton::clicked, [=]() {
        sfxplayer->setSource(QUrl::fromLocalFile("sfx1.wav"));
        sfxplayer->play();
    });
    QObject::connect(sfx2Button, &QPushButton::clicked, [=]() {
        sfxplayer->setSource(QUrl::fromLocalFile("sfx2.wav"));
        sfxplayer->play();
    });
    QObject::connect(sfx3Button, &QPushButton::clicked, [=]() {
        sfxplayer->setSource(QUrl::fromLocalFile("sfx3.wav"));
        sfxplayer->play();
    });
    QObject::connect(sfx4Button, &QPushButton::clicked, [=]() {
        sfxplayer->setSource(QUrl::fromLocalFile("sfx4.wav"));
        sfxplayer->play();
    });
    QObject::connect(openButton, &QPushButton::clicked, [&]() {
        QString file = QFileDialog::getOpenFileName(&window,"Select Music","","Audio Files (*.mp3 *.wav *.flac)");
        if(!file.isEmpty()) {
            player->setSource(QUrl::fromLocalFile(file));
            QFileInfo info(file);
            openButton->setText(info.fileName());
            playButton->setEnabled(true);
            stopButton->setEnabled(true);
            playButton->setText("Play");
        }
    });

    QObject::connect(player, &QMediaPlayer::errorOccurred, [&](QMediaPlayer::Error err, const QString &msg){
        Q_UNUSED(err)
        qDebug() << "player error:" << msg;
    });
    QObject::connect(sfxplayer, &QMediaPlayer::errorOccurred, [&](QMediaPlayer::Error err, const QString &msg){
        Q_UNUSED(err)
        qDebug() << "sfxplayer error:" << msg;
    });

    QObject::connect(player, &QMediaPlayer::playbackStateChanged, [&](QMediaPlayer::PlaybackState state){
        if(state == QMediaPlayer::PlayingState) playButton->setText("Pause");
        else playButton->setText("Play");
    });

    window.setLayout(layout);
    window.show();
    return app.exec();
}
