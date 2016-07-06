#include "BookmarksTree.h"

BookMarksTree::BookMarksTree(QWidget *parent) : QTreeWidget(parent)
{
	setAcceptDrops(true);
	setDragEnabled(false);
	setDragDropMode(QAbstractItemView::DropOnly);

	connect(this, SIGNAL(expanded(const QModelIndex &)),
								this, SLOT(resizeTreeColumn(const QModelIndex &)));
	connect(this, SIGNAL(collapsed(const QModelIndex &)),
								this, SLOT(resizeTreeColumn(const QModelIndex &)));

	setColumnCount(1);
	setHeaderHidden(true);
	reloadBookmarks();
}

void BookMarksTree::reloadBookmarks()
{
	clear();
	QSetIterator<QString> it(BimData::bookmarkPaths);
	while (it.hasNext()) {
		QString itemPath = it.next();
	    QTreeWidgetItem *item = new QTreeWidgetItem(this);
	    item->setText(0, QFileInfo(itemPath).fileName());
	   	item->setIcon(0, QIcon(":/images/bookmarks.png"));
	   	item->setToolTip(0, itemPath);
	    insertTopLevelItem(0, item);
	}
}

void BookMarksTree::resizeTreeColumn(const QModelIndex &)
{
	resizeColumnToContents(0);
}

void BookMarksTree::removeBookmark()
{
	if (selectedItems().size() == 1) {
		BimData::bookmarkPaths.remove(selectedItems().at(0)->toolTip(0));
		reloadBookmarks();
	}
}

void BookMarksTree::dragEnterEvent(QDragEnterEvent *event)
{
	QModelIndexList selectedDirs = selectionModel()->selectedRows();

	if (selectedDirs.size() > 0) {
		dndOrigSelection = selectedDirs[0];
	}
	event->acceptProposedAction();
}

void BookMarksTree::dragMoveEvent(QDragMoveEvent *event)
{
	setCurrentIndex(indexAt(event->pos()));
}

void BookMarksTree::dropEvent(QDropEvent *event)
{
	if (event->source()) {
		QString fstreeStr("FSTree");
		bool dirOp = (event->source()->metaObject()->className() == fstreeStr);
		emit dropOp(event->keyboardModifiers(), dirOp, event->mimeData()->urls().at(0).toLocalFile());
	}
}

