#include "samplewindow.h"

#include <QApplication>

#include <qpa/qwindowsysteminterface_p.h>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    app.setAttribute(Qt::AA_SynthesizeMouseForUnhandledTouchEvents, false);
    app.setAttribute(Qt::AA_SynthesizeMouseForUnhandledTabletEvents, true);

    QWindowSystemInterfacePrivate::TabletEvent::setPlatformSynthesizesMouse(false);

    SampleWindow window;
    window.resize(960,540);
    window.show();

    app.installEventFilter(&window);

    return app.exec();
}
