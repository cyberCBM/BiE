#include "BimModel.h"

bool FSModel::hasChildren(const QModelIndex &parent) const
{
	if (parent.column() > 0)
		return false;

	if (!parent.isValid()) // drives
		return true;

	// return false if item cant have children
	if (parent.flags() &  Qt::ItemNeverHasChildren) {
		return false;
	}

	// return if at least one child exists
	return QDirIterator(filePath(parent), filter() | QDir::NoDotAndDotDot, QDirIterator::NoIteratorFlags).hasNext();
}

FSTree::FSTree(QWidget *parent) : QTreeView(parent)
{
	setAcceptDrops(true);
	setDragEnabled(true);
	setDragDropMode(QAbstractItemView::InternalMove);

	fsModel = new FSModel();
	fsModel->setRootPath("");
	setModelFlags();

	setModel(fsModel);

	for (int i = 1; i <= 3; ++i) {
		hideColumn(i);
	}
	setHeaderHidden(true);

	connect(this, SIGNAL(expanded(const QModelIndex &)),
								this, SLOT(resizeTreeColumn(const QModelIndex &)));
	connect(this, SIGNAL(collapsed(const QModelIndex &)),
								this, SLOT(resizeTreeColumn(const QModelIndex &)));
}

QModelIndex FSTree::getCurrentIndex()
{
	return selectedIndexes().first();
}

void FSTree::resizeTreeColumn(const QModelIndex &)
{
	resizeColumnToContents(0);
}

void FSTree::dragEnterEvent(QDragEnterEvent *event)
{
	QModelIndexList selectedDirs = selectionModel()->selectedRows();
	if (selectedDirs.size() > 0) {
		dndOrigSelection = selectedDirs[0];
		event->acceptProposedAction();
	}
}

void FSTree::dragMoveEvent(QDragMoveEvent *event)
{
	setCurrentIndex(indexAt(event->pos()));
}

void FSTree::dropEvent(QDropEvent *event)
{
	if (event->source())
	{
		QString fstreeStr="FSTree";
		bool dirOp = (event->source()->metaObject()->className() == fstreeStr);
		emit dropOp(event->keyboardModifiers(), dirOp, event->mimeData()->urls().at(0).toLocalFile());
		setCurrentIndex(dndOrigSelection);
	}
}

void FSTree::setModelFlags()
{
	fsModel->setFilter(QDir::AllDirs | QDir::NoDotAndDotDot);
    if (BimData::showHiddenFiles)
		fsModel->setFilter(fsModel->filter() | QDir::Hidden);
}

/////////////////////////////////////////////////////////////////

DirCompleter::DirCompleter(QObject *parent) : QCompleter(parent)
{
    QDirModel *model = new QDirModel;
    model->setFilter(QDir::AllDirs | QDir::NoDotAndDotDot);
    model->setLazyChildCount(true);
    setModel(model);
}

QString DirCompleter::pathFromIndex(const QModelIndex &index) const
{
    return QCompleter::pathFromIndex(index) + "/";
}

QStringList DirCompleter::splitPath(const QString &path) const
{
    if (path.startsWith("~")) {
        return QCompleter::splitPath(QString(path).replace(0, 1, QDir::homePath()));
    }
    return QCompleter::splitPath(path);
}

/////////////////////////////////////////////////////////////////

namespace BimData
{
    QSettings *appSettings;
    unsigned int layoutMode;
    unsigned int zoomInFlags;
    unsigned int zoomOutFlags;
    QColor backgroundColor;
    QColor thumbsBackgroundColor;
    QColor thumbsTextColor;
    unsigned int thumbsLayout;
    unsigned int thumbSpacing;
    unsigned int thumbPagesReadahead;
    bool exitInsteadOfClose;
    bool wrapImageList;
    bool enableAnimations;
    float imageZoomFactor;
    bool keepZoomFactor;
    int rotation;
    bool keepTransform;
    bool flipH;
    bool flipV;
    int scaledWidth;
    int scaledHeight;
    int defaultSaveQuality;
    int cropLeft;
    int cropTop;
    int cropWidth;
    int cropHeight;
    int cropLeftPercent;
    int cropTopPercent;
    int cropWidthPercent;
    int cropHeightPercent;
    bool noEnlargeSmallThumb;
    int slideShowDelay;
    bool slideShowRandom;
    bool slideShowActive;
    QMap<QString, QAction *> actionKeys;
    int hueVal;
    int saturationVal;
    int lightnessVal;
    int contrastVal;
    int brightVal;
    int redVal;
    int greenVal;
    int blueVal;
    bool colorsActive;
    bool colorizeEnabled;
    bool hueRedChannel;
    bool hueGreenChannel;
    bool hueBlueChannel;
    bool exifRotationEnabled;
    bool exifThumbRotationEnabled;
    bool includeSubFolders;
    bool showHiddenFiles;
    bool imageToolbarFullScreen;
    QMap<QString, QString> externalApps;
    QSet<QString> bookmarkPaths;
    bool reverseMouseBehavior;
    bool deleteConfirm;
    QModelIndexList copyCutIdxList;
    bool copyOp;
    QStringList copyCutFileList;
    bool isFullScreen;
    int dialogLastX;
    int dialogLastY;
    StartupDir startupDir;
    QString specifiedStartDir;
    bool enableImageInfoFS;
    bool showLabels;
    bool smallIcons;
    bool LockDocks;
    bool fsDockVisible;
    bool bmDockVisible;
    bool iiDockVisible;
    bool pvDockVisible;
    bool ivDockVisible;
    QString thumbsBackImage;
}
