#pragma once
#include <QLabel>
#include <QMouseEvent>
#include <QtMath>

class DiscLabel : public QLabel {
    Q_OBJECT
public:
    explicit DiscLabel(QWidget *parent = nullptr);

signals:
    void spinAdjusted(double delta);

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;

private:
    QPoint lastPos;
};
