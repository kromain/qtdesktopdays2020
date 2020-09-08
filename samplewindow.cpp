#include "samplewindow.h"

#include <QDebug>
#include <QFile>
#include <QGridLayout>
#include <QListWidget>
#include <QPushButton>
#include <QScroller>
#include <QTextEdit>

#include <QTabletEvent>
#include <QWheelEvent>

#include <qpa/qwindowsysteminterface.h>

SampleWindow::SampleWindow(QWidget *parent)
    : QWidget(parent)
    , m_ignoreOSMouseEventsFromPen(false)
{
    QGridLayout* layout = new QGridLayout(this);

    QTextEdit* textEdit = new QTextEdit;
    layout->addWidget(textEdit, 0, 0, 2, 1);

    QFile sampleFile(":/sample.txt");
    if (sampleFile.open(QIODevice::ReadOnly)) {
        textEdit->setMarkdown(sampleFile.readAll());
        sampleFile.close();
    }
    textEdit->setLineWrapMode(QTextEdit::NoWrap);
    textEdit->setWordWrapMode(QTextOption::NoWrap);
    textEdit->zoomIn(2);

    QListWidget* listView = new QListWidget;
    layout->addWidget(listView, 0, 1, 1, 1);

    listView->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
    for (int i = 1; i <= 1000; ++i)
        listView->addItem(QStringLiteral("Item %1").arg(i));

    QPushButton* quitButton = new QPushButton(tr("&Close"));
    layout->addWidget(quitButton, 1, 1, 1, 1);

    connect(quitButton, &QPushButton::clicked, this, &SampleWindow::close);

    QScroller::grabGesture(textEdit, QScroller::MiddleMouseButtonGesture);
    QScroller::grabGesture(listView, QScroller::MiddleMouseButtonGesture);
}

bool SampleWindow::eventFilter(QObject* obj, QEvent* e)
{
    switch (e->type()) {
    case QEvent::MouseButtonPress:
    case QEvent::MouseButtonRelease:
    case QEvent::MouseButtonDblClick:
    case QEvent::MouseMove:
    {
        QMouseEvent* me = static_cast<QMouseEvent*>(e);

        // Ignore mouse events synthesized by the OS for touch/pen events
        if (me->source() == Qt::MouseEventSynthesizedBySystem || (me->source() == Qt::MouseEventNotSynthesized && m_ignoreOSMouseEventsFromPen))
            return true;
    }
        break;

    // Workaround for regression from QTBUG-76617 (to be fixed in 5.15.2)
    case QEvent::TabletEnterProximity:
        m_ignoreOSMouseEventsFromPen = true;
        break;
    case QEvent::TabletLeaveProximity:
        m_ignoreOSMouseEventsFromPen = false;
        break;

    case QEvent::TabletPress:
    case QEvent::TabletRelease:
    case QEvent::TabletMove:
    {
        QTabletEvent* te = static_cast<QTabletEvent*>(e);

        if (te->pointerType() == QTabletEvent::Eraser) {

            if (te->type() == QEvent::TabletPress) {
                m_lastPenPos = te->posF();
            }
            else if (te->type() == QEvent::TabletRelease) {
                m_lastPenPos = {};
            }
            else if (te->type() == QEvent::TabletMove && !m_lastPenPos.isNull()) {
                const QPoint scrollOffset = (te->posF() - m_lastPenPos).toPoint() * 2;
                if (!scrollOffset.isNull())
                    QWindowSystemInterface::handleWheelEvent(windowHandle(), mapFromGlobal(te->globalPos()), te->globalPos(), {}, scrollOffset);

                m_lastPenPos = te->posF();
            }

            // Filter out to prevent conversion to mouse event
            return true;
        }
    }
        break;

    default:
        break;
    }

    return QWidget::eventFilter(obj, e);
}

