#include <QtWidgets>
#include <QCompleter>
#include <QSettings>
#include <QModelIndexList>
#include <QStringList>
#include <QColor>
#include <QAction>
#include <QSet>

#ifndef _H_BimModel_H_
#define _H_BimModel_H_

class FSModel : public QFileSystemModel
{
public:
	bool hasChildren(const QModelIndex &parent) const;
};

class FSTree : public QTreeView
{
	Q_OBJECT

public:
	FSTree(QWidget *parent);
	FSModel *fsModel;
	QModelIndex getCurrentIndex();
	void setModelFlags();

protected:
	void dragEnterEvent(QDragEnterEvent *event);
	void dragMoveEvent(QDragMoveEvent *event);
	void dropEvent(QDropEvent *event);

signals:
	void dropOp(Qt::KeyboardModifiers keyMods, bool dirOp, QString cpMvDirPath);

private:
	QModelIndex dndOrigSelection;

private slots:
	void resizeTreeColumn(const QModelIndex &);
};



class DirCompleter : public QCompleter
{
    Q_OBJECT
public:
    DirCompleter(QObject *parent = 0);
    QString pathFromIndex(const QModelIndex &index) const;

public slots:
    QStringList splitPath(const QString &path) const;
};


namespace BimData
{
    enum StartupDir
    {
        defaultDir = 0,
        rememberLastDir,
        specifiedDir
    };

    extern QSettings *appSettings;
    extern unsigned int layoutMode;
    extern unsigned int zoomInFlags;
    extern unsigned int zoomOutFlags;
    extern QColor backgroundColor;
    extern QColor thumbsBackgroundColor;
    extern QColor thumbsTextColor;
    extern unsigned int thumbsLayout;
    extern unsigned int thumbSpacing;
    extern unsigned int thumbPagesReadahead;
    extern bool exitInsteadOfClose;
    extern bool wrapImageList;
    extern bool enableAnimations;
    extern float imageZoomFactor;
    extern bool keepZoomFactor;
    extern int rotation;
    extern bool keepTransform;
    extern bool flipH;
    extern bool flipV;
    extern int scaledWidth;
    extern int scaledHeight;
    extern int defaultSaveQuality;
    extern int cropLeft;
    extern int cropTop;
    extern int cropWidth;
    extern int cropHeight;
    extern int cropLeftPercent;
    extern int cropTopPercent;
    extern int cropWidthPercent;
    extern int cropHeightPercent;
    extern bool noEnlargeSmallThumb;
    extern int slideShowDelay;
    extern bool slideShowRandom;
    extern bool slideShowActive;
    extern QMap<QString, QAction *> actionKeys;
    extern bool hueSatEnabled;
    extern bool brightContrastEnabled;
    extern int hueVal;
    extern int saturationVal;
    extern int lightnessVal;
    extern int contrastVal;
    extern int brightVal;
    extern int redVal;
    extern int greenVal;
    extern int blueVal;
    extern bool colorsActive;
    extern bool colorizeEnabled;
    extern bool hueRedChannel;
    extern bool hueGreenChannel;
    extern bool hueBlueChannel;
    extern bool exifRotationEnabled;
    extern bool exifThumbRotationEnabled;
    extern bool includeSubFolders;
    extern bool showHiddenFiles;
    extern bool imageToolbarFullScreen;
    extern QMap<QString, QString> externalApps;
    extern QSet<QString> bookmarkPaths;
    extern bool reverseMouseBehavior;
    extern bool deleteConfirm;
    extern QModelIndexList copyCutIdxList;
    extern bool copyOp;
    extern QStringList copyCutFileList;
    extern bool isFullScreen;
    extern int dialogLastX;
    extern int dialogLastY;
    extern StartupDir startupDir;
    extern QString specifiedStartDir;
    extern bool enableImageInfoFS;
    extern bool showLabels;
    extern bool smallIcons;
    extern bool LockDocks;
    extern bool fsDockVisible;
    extern bool bmDockVisible;
    extern bool iiDockVisible;
    extern bool pvDockVisible;
    extern bool ivDockVisible;
    extern QString thumbsBackImage;
}

#endif _H_BimModel_H_

