
#ifndef _H_BookMarksTree_H_
#define _H_BookMarksTree_H_

#include <QtWidgets>
#include "BimModel.h"

class BookMarksTree : public QTreeWidget
{
	Q_OBJECT

public:
    BookMarksTree(QWidget *parent);
	void reloadBookmarks();

public slots:
	void removeBookmark();

private:
	QModelIndex dndOrigSelection;

private slots:
	void resizeTreeColumn(const QModelIndex &);

protected:
	void dragEnterEvent(QDragEnterEvent *event);
	void dragMoveEvent(QDragMoveEvent *event);
	void dropEvent(QDropEvent *event);

signals:
	void dropOp(Qt::KeyboardModifiers keyMods, bool dirOp, QString cpMvDirPath);
};

#endif // _H_BookMarksTree_H_

