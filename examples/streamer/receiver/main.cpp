#include <QApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    const char *port = argc > 1 ? argv[1] : "5201";
    QQmlApplicationEngine engine;
    engine.rootContext()->setContextProperty("PORT", port);
    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));

    return app.exec();
}
