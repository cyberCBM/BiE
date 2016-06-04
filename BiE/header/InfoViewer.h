#ifndef INFOVIEW_H
#define INFOVIEW_H

#include <QtWidgets>

class InfoViewer : public QTableView
{
	Q_OBJECT

public:
    InfoViewer(QWidget *parent);
	void clear();
	void addEntry(QString &key, QString &value);
	void addTitleEntry(QString title);

public slots:
	void showInfoViewMenu(QPoint pt);
	void copyEntry();

private:
	QStandardItemModel *infoModel;
	QModelIndex selectedEntry;
	QMenu *infoMenu;
	QAction *copyAction;

};

#endif // INFOVIEW_H
