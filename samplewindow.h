#pragma once

#include <QWidget>

class SampleWindow : public QWidget
{
    Q_OBJECT

public:
    explicit SampleWindow(QWidget *parent = nullptr);

protected:
    bool eventFilter(QObject*, QEvent*) override;

private:
    bool m_ignoreOSMouseEventsFromPen;
    QPointF m_lastPenPos;
};
