#include "DiscLabel.h"

DiscLabel::DiscLabel(QWidget *parent) : QLabel(parent) {
    setFixedSize(150, 150);
}

void DiscLabel::mousePressEvent(QMouseEvent *event) {
    lastPos = event->pos();
}

void DiscLabel::mouseMoveEvent(QMouseEvent *event) {
    QPointF center(width()/2.0, height()/2.0);
    QPointF lastVec = lastPos - center;
    QPointF currentVec = event->pos() - center;

    double lastAngle = std::atan2(lastVec.y(), lastVec.x());
    double currentAngle = std::atan2(currentVec.y(), currentVec.x());
    double deltaAngle = currentAngle - lastAngle;

    if(deltaAngle > M_PI) deltaAngle -= 2*M_PI;
    if(deltaAngle < -M_PI) deltaAngle += 2*M_PI;

    emit spinAdjusted(deltaAngle);
    lastPos = event->pos();
}
