#include "BiManagerWindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication QApp(argc, argv);
   	if (QCoreApplication::arguments().size() > 2)
   	{
        qDebug() << QObject::tr("Usage: bim [FILE or DIRECTORY]...");
   		return -1;
	}

    QTranslator qtTranslator;

    qtTranslator.load("qt_" + QLocale::system().name(),
                    QLibraryInfo::location(QLibraryInfo::TranslationsPath));
    QApp.installTranslator(&qtTranslator);

    QTranslator bimTranslator;
    bimTranslator.load(":/translations/bim_" + QLocale::system().name());
    QApp.installTranslator(&bimTranslator);

    BiManagerWindow bim;
    bim.show();
    return QApp.exec();
}

