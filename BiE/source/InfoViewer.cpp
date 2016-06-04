#include "InfoViewer.h"

InfoViewer::InfoViewer(QWidget *parent) : QTableView(parent)
{
	setSelectionBehavior(QAbstractItemView::SelectItems);
	setSelectionMode(QAbstractItemView::ExtendedSelection);
	verticalHeader()->setVisible(false);
	verticalHeader()->setDefaultSectionSize(verticalHeader()->minimumSectionSize());
	horizontalHeader()->setVisible(false);
	horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
	setShowGrid(false);

	setEditTriggers(QAbstractItemView::NoEditTriggers);
	setSelectionBehavior(QAbstractItemView::SelectRows);
	setTabKeyNavigation(false);

	infoModel = new QStandardItemModel(this);
	setModel(infoModel);

	// InfoView menu
	infoMenu = new QMenu("");
	copyAction = new QAction(tr("Copy"), this);
	connect(copyAction, SIGNAL(triggered()), this, SLOT(copyEntry()));
	infoMenu->addAction(copyAction);
	setContextMenuPolicy(Qt::CustomContextMenu);
	connect(this, SIGNAL(customContextMenuRequested(QPoint)), SLOT(showInfoViewMenu(QPoint)));
}

void InfoViewer::showInfoViewMenu(QPoint pt)
{
    selectedEntry = indexAt(pt);
	if (selectedEntry.isValid())
    	infoMenu->popup(viewport()->mapToGlobal(pt));
}
 
void InfoViewer::clear()
{
	infoModel->clear();
}

void InfoViewer::addEntry(QString &key, QString &value)
{
	int atRow = infoModel->rowCount();
	QStandardItem *itemKey = new QStandardItem(key);
	infoModel->insertRow(atRow, itemKey);
	if (!value.isEmpty()) {
		QStandardItem *itemVal = new QStandardItem(value);
		itemVal->setToolTip(value);
		infoModel->setItem(atRow, 1, itemVal);
	}
}

void InfoViewer::addTitleEntry(QString title)
{
	int atRow = infoModel->rowCount();
	QStandardItem *itemKey = new QStandardItem(title);
	infoModel->insertRow(atRow, itemKey);

	QFont boldFont;
	boldFont.setBold(true);
    itemKey->setData(boldFont, Qt::FontRole);
}

void InfoViewer::copyEntry()
{
	if (selectedEntry.isValid())
		QApplication::clipboard()->setText(infoModel->itemFromIndex(selectedEntry)->toolTip());
}

