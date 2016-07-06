#ifndef THUMBVIEW_H
#define THUMBVIEW_H

#include <QtWidgets>
#include <exiv2/exiv2.hpp>
#include "BimModel.h"
#include "BookmarksTree.h"
#include "InfoViewer.h"
#include "ImageViewer.h"

struct DuplicateImage
{
	QString filePath;
	int duplicates;
};

class ThumbnailView : public QListView
{
	Q_OBJECT

public:
    InfoViewer *infoView;
	QDir *thumbsDir;
	QStringList *fileFilters;
	QList<QStandardItem*> *thumbList;
	QStandardItemModel *thumbViewModel;
	QString currentViewDir;
	QString recentThumb;
	QDir::SortFlags thumbsSortFlags;
	int thumbSize;
	int thumbWidth;
	int thumbHeight;
	QString filterStr;
	bool busy;

	enum UserRoles
	{
		FileNameRole = Qt::UserRole + 1,
		SortRole,
		LoadedRole
	};

	enum ThumbnailLayouts
	{
		Classic,
		Compact,
		Squares
	};

    ThumbnailView(QWidget *parent);
	void loadPrepare();
	void load();
	void loadDuplicates();
	void setNeedScroll(bool needScroll)
	{
		isNeedScroll = needScroll;
	}
	void setThumbColors();
	bool setCurrentIndexByName(QString &FileName);
	bool setCurrentIndexByRow(int row);
	void selectCurrentIndex();
	int getNextRow();
	int getPrevRow();
	int getLastRow();
	int getRandomRow();
	int getCurrentRow();
	void setCurrentRow(int row);
	void setImageviewWindowTitle();
	QString getSingleSelectionFilename();
	void addThumb(QString &imageFullPath);
	void abort();

protected:
	void startDrag(Qt::DropActions);
	void wheelEvent(QWheelEvent *event);
	void mousePressEvent(QMouseEvent *event);
	
private:
	QFileInfo thumbFileInfo;
	QFileInfoList thumbFileInfoList;
	QImage emptyImg;
	QModelIndex currentIndex;
	QImageReader imageInfoReader;
	QWidget *mainWindow;
	QMap<QString, DuplicateImage> dupImageHashes;
	
	bool abortOp;
	int newIndex;
	bool isNeedScroll;
	int currentRow;
	bool scrolledForward;
	int thumbsRangeFirst;
	int thumbsRangeLast;

	void initThumbs();
	void findDupes(bool resetCounters);
	void updateFoundDupesState(int duplicates, int filesScanned, int originalImages);
	int getFirstVisibleThumb();
	int getLastVisibleThumb();
	bool isThumbVisible(QModelIndex idx);
	void updateThumbsCount();
	void updateThumbsSelection();
	void updateExifInfo(QString imageFullPath);

signals:
	void setStatus(QString state);
	void showBusy(bool busy);

public slots:
	void loadVisibleThumbs(int scrollBarValue = 0);
	void handleSelectionChanged(const QItemSelection& selection);
	void invertSelection();

private slots:
	void loadThumbsRange();
};

#endif // THUMBVIEW_H

