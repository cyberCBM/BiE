#include "BimModel.h"
#include "BiManagerWindow.h"

#define THUMB_SIZE_MIN	50
#define THUMB_SIZE_MAX	300

BiManagerWindow::BiManagerWindow(QWidget *parent) : QMainWindow(parent)
{
    BimData::appSettings = new QSettings("Bim", "bim_103");
	readSettings();
	createThumbView();
	createActions();
	createMenus();
	createToolBars();
	createStatusBar();
	createFSTree();
	createBookmarks();
	createImageView();
	updateExternalApps();
	loadShortcuts();
	setupDocks();

	connect(qApp, SIGNAL(focusChanged(QWidget*, QWidget*)), 
				this, SLOT(updateActions()));

    restoreGeometry(BimData::appSettings->value("Geometry").toByteArray());
    restoreState(BimData::appSettings->value("WindowState").toByteArray());
    setWindowIcon(QIcon(":/images/bim.png"));

	mainLayout = new QHBoxLayout;
	mainLayout->setContentsMargins(0, 0, 0, 0);
	mainLayout->setSpacing(0);
	mainLayout->addWidget(thumbView);
	QWidget *centralWidget = new QWidget;
	centralWidget->setLayout(mainLayout);
	setCentralWidget(centralWidget);

	handleStartupArgs();

	copyMoveToDialog = 0;
	colorsDialog = 0;
	cropDialog = 0;
	initComplete = true;
	thumbView->busy = false;
	currentHistoryIdx = -1;
	needHistoryRecord = true;
	interfaceDisabled = false;

	refreshThumbs(true);
    if (BimData::layoutMode == thumbViewIdx)
		thumbView->setFocus(Qt::OtherFocusReason);
	if (!cliImageLoaded)
		QTimer::singleShot(100, this, SLOT(selectRecentThumb()));
	else 
		QTimer::singleShot(100, this, SLOT(updateIndexByViewerImage()));
}

void BiManagerWindow::handleStartupArgs()
{
	cliImageLoaded = false;
	if (QCoreApplication::arguments().size() == 2)
	{
		QFileInfo cliArg(QCoreApplication::arguments().at(1));
		if (cliArg.isDir())
		{
			thumbView->currentViewDir = QCoreApplication::arguments().at(1);
			cliImageLoaded = false;
		}
		else
		{
			thumbView->currentViewDir = cliArg.absolutePath();
			cliFileName = thumbView->currentViewDir + QDir::separator() + cliArg.fileName();
			cliImageLoaded = true;
			loadImagefromCli();
		}
	}
	else
	{
        if (BimData::startupDir == BimData::specifiedDir)
            thumbView->currentViewDir = BimData::specifiedStartDir;
        else if (BimData::startupDir == BimData::rememberLastDir)
            thumbView->currentViewDir = BimData::appSettings->value("lastDir").toString();
	}
	selectCurrentViewDir();
}

bool BiManagerWindow::event(QEvent *event)
{
	if (event->type() == QEvent::ActivationChange ||
            (BimData::layoutMode == thumbViewIdx && event->type() == QEvent::MouseButtonRelease)) {
		thumbView->loadVisibleThumbs();
	}
	
	return QMainWindow::event(event);
}

void BiManagerWindow::createThumbView()
{
    thumbView = new ThumbnailView(this);
    thumbView->thumbsSortFlags = (QDir::SortFlags)BimData::appSettings->value("thumbsSortFlags").toInt();
	thumbView->thumbsSortFlags |= QDir::IgnoreCase;

	connect(thumbView, SIGNAL(setStatus(QString)), this, SLOT(setStatus(QString)));
	connect(thumbView, SIGNAL(showBusy(bool)), this, SLOT(showBusyStatus(bool)));
	connect(thumbView->selectionModel(), SIGNAL(selectionChanged(QItemSelection, QItemSelection)), 
				this, SLOT(updateActions()));

	iiDock = new QDockWidget(tr("Image Info"), this);
	iiDock->setObjectName("Image Info");
	iiDock->setWidget(thumbView->infoView);
	connect(iiDock->toggleViewAction(), SIGNAL(triggered()), this, SLOT(setIiDockVisibility()));	
	connect(iiDock, SIGNAL(visibilityChanged(bool)), this, SLOT(setIiDockVisibility()));	
}

void BiManagerWindow::addMenuSeparator(QWidget *widget)
{
	QAction *separator = new QAction(this);
	separator->setSeparator(true);
	widget->addAction(separator);
}

void BiManagerWindow::createImageView()
{
    imageView = new ImageViewer(this);
	connect(saveAction, SIGNAL(triggered()), imageView, SLOT(saveImage()));
	connect(saveAsAction, SIGNAL(triggered()), imageView, SLOT(saveImageAs()));
	connect(copyImageAction, SIGNAL(triggered()), imageView, SLOT(copyImage()));
	connect(pasteImageAction, SIGNAL(triggered()), imageView, SLOT(pasteImage()));
	connect(cropToSelectionAct, SIGNAL(triggered()), imageView, SLOT(cropToSelection()));
	imageView->ImagePopUpMenu = new QMenu();

	// Widget actions
	imageView->addAction(slideShowAction);
	imageView->addAction(nextImageAction);
	imageView->addAction(prevImageAction);
	imageView->addAction(firstImageAction);
	imageView->addAction(lastImageAction);
	imageView->addAction(randomImageAction);
	imageView->addAction(zoomInAct);
	imageView->addAction(zoomOutAct);
	imageView->addAction(origZoomAct);
	imageView->addAction(resetZoomAct);
	imageView->addAction(rotateRightAct);
	imageView->addAction(rotateLeftAct);
	imageView->addAction(freeRotateRightAct);
	imageView->addAction(freeRotateLeftAct);
	imageView->addAction(flipHAct);
	imageView->addAction(flipVAct);
	imageView->addAction(cropAct);
	imageView->addAction(cropToSelectionAct);
	imageView->addAction(resizeAct);
	imageView->addAction(saveAction);
	imageView->addAction(saveAsAction);
	imageView->addAction(copyImageAction);
	imageView->addAction(pasteImageAction);
	imageView->addAction(deleteAction);
	imageView->addAction(renameAction);
	imageView->addAction(closeImageAct);
	imageView->addAction(fullScreenAct);
	imageView->addAction(settingsAction);
	imageView->addAction(mirrorDisabledAct);
	imageView->addAction(mirrorDualAct);
	imageView->addAction(mirrorTripleAct);
	imageView->addAction(mirrorVDualAct);
	imageView->addAction(mirrorQuadAct);
	imageView->addAction(keepTransformAct);
	imageView->addAction(keepZoomAct);
	imageView->addAction(refreshAction);
	imageView->addAction(colorsAct);
	imageView->addAction(moveRightAct);
	imageView->addAction(moveLeftAct);
	imageView->addAction(moveUpAct);
	imageView->addAction(moveDownAct);
	imageView->addAction(showClipboardAction);
	imageView->addAction(copyToAction);
	imageView->addAction(moveToAction);
	imageView->addAction(resizeAct);
	imageView->addAction(openAction);
	imageView->addAction(exitAction);

	// Actions
	addMenuSeparator(imageView->ImagePopUpMenu);
	imageView->ImagePopUpMenu->addAction(nextImageAction);
	imageView->ImagePopUpMenu->addAction(prevImageAction);
	imageView->ImagePopUpMenu->addAction(firstImageAction);
	imageView->ImagePopUpMenu->addAction(lastImageAction);
	imageView->ImagePopUpMenu->addAction(randomImageAction);
	imageView->ImagePopUpMenu->addAction(slideShowAction);

	addMenuSeparator(imageView->ImagePopUpMenu);
	zoomSubMenu = new QMenu(tr("Zoom"));
	zoomSubMenuAct = new QAction(tr("Zoom"), this);
	zoomSubMenuAct->setIcon(QIcon::fromTheme("edit-find", QIcon(":/images/zoom.png")));
	zoomSubMenuAct->setMenu(zoomSubMenu);
	imageView->ImagePopUpMenu->addAction(zoomSubMenuAct);
	zoomSubMenu->addAction(zoomInAct);
	zoomSubMenu->addAction(zoomOutAct);
	zoomSubMenu->addAction(origZoomAct);
	zoomSubMenu->addAction(resetZoomAct);
	addMenuSeparator(zoomSubMenu);
	zoomSubMenu->addAction(keepZoomAct);

	MirroringSubMenu = new QMenu(tr("Mirroring"));
	mirrorSubMenuAct = new QAction(tr("Mirroring"), this);
	mirrorSubMenuAct->setMenu(MirroringSubMenu);
	mirroringGroup = new QActionGroup(this);
	mirroringGroup->addAction(mirrorDisabledAct);
	mirroringGroup->addAction(mirrorDualAct);
	mirroringGroup->addAction(mirrorTripleAct);
	mirroringGroup->addAction(mirrorVDualAct);
	mirroringGroup->addAction(mirrorQuadAct);
	MirroringSubMenu->addActions(mirroringGroup->actions());

	transformSubMenu = new QMenu(tr("Transform"));
	transformSubMenuAct = new QAction(tr("Transform"), this);
	transformSubMenuAct->setMenu(transformSubMenu);
	imageView->ImagePopUpMenu->addAction(resizeAct);
	imageView->ImagePopUpMenu->addAction(cropToSelectionAct);
	imageView->ImagePopUpMenu->addAction(transformSubMenuAct);
	transformSubMenu->addAction(colorsAct);
	transformSubMenu->addAction(rotateRightAct);
	transformSubMenu->addAction(rotateLeftAct);
	transformSubMenu->addAction(freeRotateRightAct);
	transformSubMenu->addAction(freeRotateLeftAct);
	transformSubMenu->addAction(flipHAct);
	transformSubMenu->addAction(flipVAct);
	transformSubMenu->addAction(cropAct);

	addMenuSeparator(transformSubMenu);
	transformSubMenu->addAction(keepTransformAct);
	imageView->ImagePopUpMenu->addAction(mirrorSubMenuAct);

	addMenuSeparator(imageView->ImagePopUpMenu);
	imageView->ImagePopUpMenu->addAction(copyToAction);
	imageView->ImagePopUpMenu->addAction(moveToAction);
	imageView->ImagePopUpMenu->addAction(saveAction);
	imageView->ImagePopUpMenu->addAction(saveAsAction);
	imageView->ImagePopUpMenu->addAction(renameAction);
	imageView->ImagePopUpMenu->addAction(deleteAction);
	imageView->ImagePopUpMenu->addAction(openWithMenuAct);

	addMenuSeparator(imageView->ImagePopUpMenu);
	viewSubMenu = new QMenu(tr("View"));
	viewSubMenuAct = new QAction(tr("View"), this);
	viewSubMenuAct->setMenu(viewSubMenu);
	imageView->ImagePopUpMenu->addAction(viewSubMenuAct);
	viewSubMenu->addAction(fullScreenAct);
	viewSubMenu->addAction(showClipboardAction);
	viewSubMenu->addAction(actShowViewerToolbars);
	viewSubMenu->addAction(refreshAction);
	imageView->ImagePopUpMenu->addAction(copyImageAction);
	imageView->ImagePopUpMenu->addAction(pasteImageAction);
	imageView->ImagePopUpMenu->addAction(closeImageAct);
	imageView->ImagePopUpMenu->addAction(exitAction);

	addMenuSeparator(imageView->ImagePopUpMenu);
	imageView->ImagePopUpMenu->addAction(settingsAction);

	imageView->setContextMenuPolicy(Qt::DefaultContextMenu);
    BimData::isFullScreen = BimData::appSettings->value("isFullScreen").toBool();
    fullScreenAct->setChecked(BimData::isFullScreen);
}

void BiManagerWindow::createActions()
{
	thumbsGoTopAct = new QAction(tr("Top"), this);
	thumbsGoTopAct->setObjectName("thumbsGoTop");
	thumbsGoTopAct->setIcon(QIcon::fromTheme("go-top", QIcon(":/images/top.png")));
	connect(thumbsGoTopAct, SIGNAL(triggered()), this, SLOT(goTop()));

	thumbsGoBottomAct = new QAction(tr("Bottom"), this);
	thumbsGoBottomAct->setObjectName("thumbsGoBottom");
	thumbsGoBottomAct->setIcon(QIcon::fromTheme("go-bottom", QIcon(":/images/bottom.png")));
	connect(thumbsGoBottomAct, SIGNAL(triggered()), this, SLOT(goBottom()));

	closeImageAct = new QAction(tr("Close Image"), this);
	closeImageAct->setObjectName("closeImage");
	connect(closeImageAct, SIGNAL(triggered()), this, SLOT(hideViewer()));

	fullScreenAct = new QAction(tr("Full Screen"), this);
	fullScreenAct->setObjectName("fullScreen");
	fullScreenAct->setCheckable(true);
	connect(fullScreenAct, SIGNAL(triggered()), this, SLOT(toggleFullScreen()));
	
	settingsAction = new QAction(tr("Preferences"), this);
	settingsAction->setObjectName("settings");
	settingsAction->setIcon(QIcon::fromTheme("preferences-other", QIcon(":/images/settings.png")));
	connect(settingsAction, SIGNAL(triggered()), this, SLOT(showSettings()));

	exitAction = new QAction(tr("Exit"), this);
	exitAction->setObjectName("exit");
	connect(exitAction, SIGNAL(triggered()), this, SLOT(close()));

	thumbsZoomInAct = new QAction(tr("Enlarge Thumbnails"), this);
	thumbsZoomInAct->setObjectName("thumbsZoomIn");
	connect(thumbsZoomInAct, SIGNAL(triggered()), this, SLOT(thumbsZoomIn()));
	thumbsZoomInAct->setIcon(QIcon::fromTheme("zoom-in", QIcon(":/images/zoom_in.png")));
	if (thumbView->thumbSize == THUMB_SIZE_MAX)
		thumbsZoomInAct->setEnabled(false);

	thumbsZoomOutAct = new QAction(tr("Shrink Thumbnails"), this);
	thumbsZoomOutAct->setObjectName("thumbsZoomOut");
	connect(thumbsZoomOutAct, SIGNAL(triggered()), this, SLOT(thumbsZoomOut()));
	thumbsZoomOutAct->setIcon(QIcon::fromTheme("zoom-out", QIcon(":/images/zoom_out.png")));
	if (thumbView->thumbSize == THUMB_SIZE_MIN)
		thumbsZoomOutAct->setEnabled(false);

	cutAction = new QAction(tr("Cut"), this);
	cutAction->setObjectName("cut");
	cutAction->setIcon(QIcon::fromTheme("edit-cut", QIcon(":/images/cut.png")));
	connect(cutAction, SIGNAL(triggered()), this, SLOT(cutThumbs()));
	cutAction->setEnabled(false);

	copyAction = new QAction(tr("Copy"), this);
	copyAction->setObjectName("copy");
	copyAction->setIcon(QIcon::fromTheme("edit-copy", QIcon(":/images/copy.png")));
	connect(copyAction, SIGNAL(triggered()), this, SLOT(copyThumbs()));
	copyAction->setEnabled(false);

	copyToAction = new QAction(tr("Copy to..."), this);
	copyToAction->setObjectName("copyTo");
	connect(copyToAction, SIGNAL(triggered()), this, SLOT(copyImagesTo()));

	moveToAction = new QAction(tr("Move to..."), this);
	moveToAction->setObjectName("moveTo");
	connect(moveToAction, SIGNAL(triggered()), this, SLOT(moveImagesTo()));
	
	deleteAction = new QAction(tr("Delete"), this);
	deleteAction->setObjectName("delete");
	deleteAction->setIcon(QIcon::fromTheme("edit-delete", QIcon(":/images/delete.png")));
	connect(deleteAction, SIGNAL(triggered()), this, SLOT(deleteOp()));

	saveAction = new QAction(tr("Save"), this);
	saveAction->setObjectName("save");
	saveAction->setIcon(QIcon::fromTheme("document-save", QIcon(":/images/save.png")));

	saveAsAction = new QAction(tr("Save As"), this);
	saveAsAction->setObjectName("saveAs");
	saveAsAction->setIcon(QIcon::fromTheme("document-save-as", QIcon(":/images/save_as.png")));
	
	copyImageAction = new QAction(tr("Copy Image Data"), this);
	copyImageAction->setObjectName("copyImage");
	pasteImageAction = new QAction(tr("Paste Image Data"), this);
	pasteImageAction->setObjectName("pasteImage");

	renameAction = new QAction(tr("Rename"), this);
	renameAction->setObjectName("rename");
	connect(renameAction, SIGNAL(triggered()), this, SLOT(rename()));

	selectAllAction = new QAction(tr("Select All"), this);
	selectAllAction->setObjectName("selectAll");
	connect(selectAllAction, SIGNAL(triggered()), this, SLOT(selectAllThumbs()));

	aboutAction = new QAction(tr("About"), this);
	aboutAction->setObjectName("about");
	aboutAction->setIcon(QIcon::fromTheme("help-about", QIcon(":/images/about.png")));
	connect(aboutAction, SIGNAL(triggered()), this, SLOT(about()));

	// Sort actions
	actName = new QAction(tr("Name"), this);
	actName->setObjectName("name");
	actTime = new QAction(tr("Time"), this);
	actTime->setObjectName("time");
	actSize = new QAction(tr("Size"), this);
	actSize->setObjectName("size");
	actType = new QAction(tr("Type"), this);
	actType->setObjectName("type");
	actReverse = new QAction(tr("Reverse"), this);
	actReverse->setObjectName("reverse");
	actName->setCheckable(true);
	actTime->setCheckable(true);
	actSize->setCheckable(true);
	actType->setCheckable(true);
	actReverse->setCheckable(true);
	connect(actName, SIGNAL(triggered()), this, SLOT(sortThumbnains()));
	connect(actTime, SIGNAL(triggered()), this, SLOT(sortThumbnains()));
	connect(actSize, SIGNAL(triggered()), this, SLOT(sortThumbnains()));
	connect(actType, SIGNAL(triggered()), this, SLOT(sortThumbnains()));
	connect(actReverse, SIGNAL(triggered()), this, SLOT(sortThumbnains()));

	if (thumbView->thumbsSortFlags & QDir::Time)
		actTime->setChecked(true);	
	else if (thumbView->thumbsSortFlags & QDir::Size)
		actSize->setChecked(true);	
	else if (thumbView->thumbsSortFlags & QDir::Type)
		actType->setChecked(true);	
	else
		actName->setChecked(true);	
	actReverse->setChecked(thumbView->thumbsSortFlags & QDir::Reversed); 

	actShowHidden = new QAction(tr("Show Hidden Files"), this);
	actShowHidden->setObjectName("showHidden");
	actShowHidden->setCheckable(true);
    actShowHidden->setChecked(BimData::showHiddenFiles);
	connect(actShowHidden, SIGNAL(triggered()), this, SLOT(showHiddenFiles()));

	actShowLabels = new QAction(tr("Show Labels"), this);
	actShowLabels->setObjectName("showLabels");
	actShowLabels->setCheckable(true);
    actShowLabels->setChecked(BimData::showLabels);
	connect(actShowLabels, SIGNAL(triggered()), this, SLOT(showLabels()));
    actShowLabels->setEnabled(BimData::thumbsLayout != ThumbnailView::Squares)	;

	actSmallIcons = new QAction(tr("Small Icons"), this);
	actSmallIcons->setObjectName("smallIcons");
	actSmallIcons->setCheckable(true);
    actSmallIcons->setChecked(BimData::smallIcons);
	connect(actSmallIcons, SIGNAL(triggered()), this, SLOT(setToolbarIconSize()));

	actLockDocks = new QAction(tr("Hide Docks Title Bar"), this);
	actLockDocks->setObjectName("lockDocks");
	actLockDocks->setCheckable(true);
    actLockDocks->setChecked(BimData::LockDocks);
	connect(actLockDocks, SIGNAL(triggered()), this, SLOT(lockDocks()));

	actShowViewerToolbars = new QAction(tr("Show Toolbar"), this);
	actShowViewerToolbars->setObjectName("showViewerToolbars");
	actShowViewerToolbars->setCheckable(true);
    actShowViewerToolbars->setChecked(BimData::imageToolbarFullScreen);
	connect(actShowViewerToolbars, SIGNAL(triggered()), this, SLOT(toggleImageToolbar()));

	actClassic = new QAction(tr("Classic Thumbs"), this);
	actClassic->setObjectName("classic");
	actCompact = new QAction(tr("Compact"), this);
	actCompact->setObjectName("compact");
	actSquarish = new QAction(tr("Squarish"), this);
	actSquarish->setObjectName("squarish");
	connect(actClassic, SIGNAL(triggered()), this, SLOT(setClassicThumbs()));
	connect(actCompact, SIGNAL(triggered()), this, SLOT(setCompactThumbs()));
	connect(actSquarish, SIGNAL(triggered()), this, SLOT(setSquarishThumbs()));
	actClassic->setCheckable(true);
	actCompact->setCheckable(true);
	actSquarish->setCheckable(true);
    actClassic->setChecked(BimData::thumbsLayout == ThumbnailView::Classic);
    actCompact->setChecked(BimData::thumbsLayout == ThumbnailView::Compact);
    actSquarish->setChecked(BimData::thumbsLayout == ThumbnailView::Squares);

	refreshAction = new QAction(tr("Reload"), this);
	refreshAction->setObjectName("refresh");
	refreshAction->setIcon(QIcon::fromTheme("view-refresh", QIcon(":/images/refresh.png")));
	connect(refreshAction, SIGNAL(triggered()), this, SLOT(reload()));

	subFoldersAction = new QAction(tr("Include Sub-folders"), this);
	subFoldersAction->setObjectName("subFolders");
	subFoldersAction->setIcon(QIcon(":/images/tree.png"));
	subFoldersAction->setCheckable(true);
	connect(subFoldersAction, SIGNAL(triggered()), this, SLOT(setIncludeSubFolders()));

	pasteAction = new QAction(tr("Paste Here"), this);
	pasteAction->setObjectName("paste");
	pasteAction->setIcon(QIcon::fromTheme("edit-paste", QIcon(":/images/paste.png")));
	connect(pasteAction, SIGNAL(triggered()), this, SLOT(pasteThumbs()));
	pasteAction->setEnabled(false);
	
	createDirAction = new QAction(tr("New Folder"), this);
	createDirAction->setObjectName("createDir");
	connect(createDirAction, SIGNAL(triggered()), this, SLOT(createSubDirectory()));
	createDirAction->setIcon(QIcon::fromTheme("folder-new", QIcon(":/images/new_folder.png")));
	
	goBackAction = new QAction(tr("Back"), this);
	goBackAction->setObjectName("goBack");
	goBackAction->setIcon(QIcon::fromTheme("go-previous", QIcon(":/images/back.png")));
	connect(goBackAction, SIGNAL(triggered()), this, SLOT(goBack()));
	goBackAction->setEnabled(false);

	goFrwdAction = new QAction(tr("Forward"), this);
	goFrwdAction->setObjectName("goFrwd");
	goFrwdAction->setIcon(QIcon::fromTheme("go-next", QIcon(":/images/next.png")));
	connect(goFrwdAction, SIGNAL(triggered()), this, SLOT(goForward()));
	goFrwdAction->setEnabled(false);

	goUpAction = new QAction(tr("Go Up"), this);
	goUpAction->setObjectName("up");
	goUpAction->setIcon(QIcon::fromTheme("go-up", QIcon(":/images/up.png")));
	connect(goUpAction, SIGNAL(triggered()), this, SLOT(goUp()));

	goHomeAction = new QAction(tr("Home"), this);
	goHomeAction->setObjectName("home");
	connect(goHomeAction, SIGNAL(triggered()), this, SLOT(goHome()));	
	goHomeAction->setIcon(QIcon::fromTheme("go-home", QIcon(":/images/home.png")));

	slideShowAction = new QAction(tr("Slide Show"), this);
	slideShowAction->setObjectName("slideShow");
	connect(slideShowAction, SIGNAL(triggered()), this, SLOT(slideShow()));
	slideShowAction->setIcon(QIcon::fromTheme("media-playback-start", QIcon(":/images/play.png")));

	nextImageAction = new QAction(tr("Next"), this);
	nextImageAction->setObjectName("nextImage");
	nextImageAction->setIcon(QIcon::fromTheme("go-next", QIcon(":/images/next.png")));
	connect(nextImageAction, SIGNAL(triggered()), this, SLOT(loadNextImage()));
	
	prevImageAction = new QAction(tr("Previous"), this);
	prevImageAction->setObjectName("prevImage");
	prevImageAction->setIcon(QIcon::fromTheme("go-previous", QIcon(":/images/back.png")));
	connect(prevImageAction, SIGNAL(triggered()), this, SLOT(loadPrevImage()));

	firstImageAction = new QAction(tr("First"), this);
	firstImageAction->setObjectName("firstImage");
	firstImageAction->setIcon(QIcon::fromTheme("go-first", QIcon(":/images/first.png")));
	connect(firstImageAction, SIGNAL(triggered()), this, SLOT(loadFirstImage()));

	lastImageAction = new QAction(tr("Last"), this);
	lastImageAction->setObjectName("lastImage");
	lastImageAction->setIcon(QIcon::fromTheme("go-last", QIcon(":/images/last.png")));
	connect(lastImageAction, SIGNAL(triggered()), this, SLOT(loadLastImage()));

	randomImageAction = new QAction(tr("Random"), this);
	randomImageAction->setObjectName("randomImage");
	connect(randomImageAction, SIGNAL(triggered()), this, SLOT(loadRandomImage()));

	openAction = new QAction(tr("Open"), this);
	openAction->setObjectName("open");
	openAction->setIcon(QIcon::fromTheme("document-open", QIcon(":/images/open.png")));
	connect(openAction, SIGNAL(triggered()), this, SLOT(openOp()));

	showClipboardAction = new QAction(tr("Load Clipboard"), this);
	showClipboardAction->setObjectName("showClipboard");
	showClipboardAction->setIcon(QIcon::fromTheme("insert-image", QIcon(":/images/new.png")));
	connect(showClipboardAction, SIGNAL(triggered()), this, SLOT(newImage()));

	openWithSubMenu = new QMenu(tr("Open With..."));
	openWithMenuAct = new QAction(tr("Open With..."), this);
	openWithMenuAct->setObjectName("openWithMenu");
	openWithMenuAct->setMenu(openWithSubMenu);
	chooseAppAct = new QAction(tr("Manage External Applications"), this);
	chooseAppAct->setObjectName("chooseApp");
	connect(chooseAppAct, SIGNAL(triggered()), this, SLOT(chooseExternalApp()));

	addBookmarkAction = new QAction(tr("Add Bookmark"), this);
	addBookmarkAction->setObjectName("addBookmark");
	addBookmarkAction->setIcon(QIcon(":/images/new_bookmark.png"));
	connect(addBookmarkAction, SIGNAL(triggered()), this, SLOT(addNewBookmark()));

	removeBookmarkAction = new QAction(tr("Remove Bookmark"), this);
	removeBookmarkAction->setObjectName("removeBookmark");
	removeBookmarkAction->setIcon(QIcon::fromTheme("edit-delete", QIcon(":/images/delete.png")));

	zoomOutAct = new QAction(tr("Zoom Out"), this);
	zoomOutAct->setObjectName("zoomOut");
	connect(zoomOutAct, SIGNAL(triggered()), this, SLOT(zoomOut()));
	zoomOutAct->setIcon(QIcon::fromTheme("zoom-out", QIcon(":/images/zoom_out.png")));

	zoomInAct = new QAction(tr("Zoom In"), this);
	zoomInAct->setObjectName("zoomIn");
	connect(zoomInAct, SIGNAL(triggered()), this, SLOT(zoomIn()));
	zoomInAct->setIcon(QIcon::fromTheme("zoom-in", QIcon(":/images/zoom_out.png")));

	resetZoomAct = new QAction(tr("Reset Zoom"), this);
	resetZoomAct->setObjectName("resetZoom");
	resetZoomAct->setIcon(QIcon::fromTheme("zoom-fit-best", QIcon(":/images/zoom.png")));
	connect(resetZoomAct, SIGNAL(triggered()), this, SLOT(resetZoom()));

	origZoomAct = new QAction(tr("Original Size"), this);
	origZoomAct->setObjectName("origZoom");
	origZoomAct->setIcon(QIcon::fromTheme("zoom-original", QIcon(":/images/zoom1.png")));
	connect(origZoomAct, SIGNAL(triggered()), this, SLOT(origZoom()));

	keepZoomAct = new QAction(tr("Keep Zoom"), this);
	keepZoomAct->setObjectName("keepZoom");
	keepZoomAct->setCheckable(true);
	connect(keepZoomAct, SIGNAL(triggered()), this, SLOT(keepZoom()));

	rotateLeftAct = new QAction(tr("Rotate 90 degree CCW"), this);
	rotateLeftAct->setObjectName("rotateLeft");
	rotateLeftAct->setIcon(QIcon::fromTheme("object-rotate-left", QIcon(":/images/rotate_left.png")));
	connect(rotateLeftAct, SIGNAL(triggered()), this, SLOT(rotateLeft()));

	rotateRightAct = new QAction(tr("Rotate 90 degree CW"), this);
	rotateRightAct->setObjectName("rotateRight");
	rotateRightAct->setIcon(QIcon::fromTheme("object-rotate-right", QIcon(":/images/rotate_right.png")));
	connect(rotateRightAct, SIGNAL(triggered()), this, SLOT(rotateRight()));

	flipHAct = new QAction(tr("Flip Horizontally"), this);
	flipHAct->setObjectName("flipH");
	flipHAct->setIcon(QIcon::fromTheme("object-flip-horizontal", QIcon(":/images/flipH.png")));
	connect(flipHAct, SIGNAL(triggered()), this, SLOT(flipHoriz()));

	flipVAct = new QAction(tr("Flip Vertically"), this);
	flipVAct->setObjectName("flipV");
	flipVAct->setIcon(QIcon::fromTheme("object-flip-vertical", QIcon(":/images/flipV.png")));
	connect(flipVAct, SIGNAL(triggered()), this, SLOT(flipVert()));

	cropAct = new QAction(tr("Cropping"), this);
	cropAct->setObjectName("crop");
	cropAct->setIcon(QIcon(":/images/crop.png"));
	connect(cropAct, SIGNAL(triggered()), this, SLOT(cropImage()));

	cropToSelectionAct = new QAction(tr("Crop to Selection"), this);
	cropToSelectionAct->setObjectName("cropToSelection");
	cropToSelectionAct->setIcon(QIcon(":/images/crop.png"));

	resizeAct = new QAction(tr("Scale Image"), this);
	resizeAct->setObjectName("resize");
	resizeAct->setIcon(QIcon::fromTheme("transform-scale", QIcon(":/images/scale.png")));
	connect(resizeAct, SIGNAL(triggered()), this, SLOT(scaleImage()));

	freeRotateLeftAct = new QAction(tr("Rotate 1 degree CCW"), this);
	freeRotateLeftAct->setObjectName("freeRotateLeft");
	connect(freeRotateLeftAct, SIGNAL(triggered()), this, SLOT(freeRotateLeft()));

	freeRotateRightAct = new QAction(tr("Rotate 1 degree CW"), this);
	freeRotateRightAct->setObjectName("freeRotateRight");
	connect(freeRotateRightAct, SIGNAL(triggered()), this, SLOT(freeRotateRight()));

	colorsAct = new QAction(tr("Colors"), this);
	colorsAct->setObjectName("colors");
	connect(colorsAct, SIGNAL(triggered()), this, SLOT(showColorsDialog()));
	colorsAct->setIcon(QIcon(":/images/colors.png"));

	findDupesAction = new QAction(tr("Find Duplicate Images"), this);
	findDupesAction->setObjectName("findDupes");
	findDupesAction->setIcon(QIcon(":/images/duplicates.png"));
	findDupesAction->setCheckable(true);
	connect(findDupesAction, SIGNAL(triggered()), this, SLOT(findDuplicateImages()));

	mirrorDisabledAct = new QAction(tr("Disable"), this);
	mirrorDisabledAct->setObjectName("mirrorDisabled");
	mirrorDualAct = new QAction(tr("Dual"), this);
	mirrorDualAct->setObjectName("mirrorDual");
	mirrorTripleAct = new QAction(tr("Triple"), this);
	mirrorTripleAct->setObjectName("mirrorTriple");
	mirrorVDualAct = new QAction(tr("Dual Vertical"), this);
	mirrorVDualAct->setObjectName("mirrorVDual");
	mirrorQuadAct = new QAction(tr("Quad"), this);
	mirrorQuadAct->setObjectName("mirrorQuad");

	mirrorDisabledAct->setCheckable(true);
	mirrorDualAct->setCheckable(true);
	mirrorTripleAct->setCheckable(true);
	mirrorVDualAct->setCheckable(true);
	mirrorQuadAct->setCheckable(true);
	connect(mirrorDisabledAct, SIGNAL(triggered()), this, SLOT(setMirrorDisabled()));
	connect(mirrorDualAct, SIGNAL(triggered()), this, SLOT(setMirrorDual()));
	connect(mirrorTripleAct, SIGNAL(triggered()), this, SLOT(setMirrorTriple()));
	connect(mirrorVDualAct, SIGNAL(triggered()), this, SLOT(setMirrorVDual()));
	connect(mirrorQuadAct, SIGNAL(triggered()), this, SLOT(setMirrorQuad()));
	mirrorDisabledAct->setChecked(true); 

	keepTransformAct = new QAction(tr("Lock Transformations"), this);
	keepTransformAct->setObjectName("keepTransform");
	keepTransformAct->setCheckable(true);
	connect(keepTransformAct, SIGNAL(triggered()), this, SLOT(keepTransformClicked()));

	moveLeftAct = new QAction(tr("Move Left"), this);
	moveLeftAct->setObjectName("moveLeft");
	connect(moveLeftAct, SIGNAL(triggered()), this, SLOT(moveLeft()));
	moveRightAct = new QAction(tr("Move Right"), this);
	moveRightAct->setObjectName("moveRight");
	connect(moveRightAct, SIGNAL(triggered()), this, SLOT(moveRight()));
	moveUpAct = new QAction(tr("Move Up"), this);
	moveUpAct->setObjectName("moveUp");
	connect(moveUpAct, SIGNAL(triggered()), this, SLOT(moveUp()));
	moveDownAct = new QAction(tr("Move Down"), this);
	moveDownAct->setObjectName("moveDown");
	connect(moveDownAct, SIGNAL(triggered()), this, SLOT(moveDown()));

	invertSelectionAct = new QAction(tr("Invert Selection"), this);
	invertSelectionAct->setObjectName("invertSelection");
	connect(invertSelectionAct, SIGNAL(triggered()), thumbView, SLOT(invertSelection()));

	filterImagesFocusAct = new QAction(tr("Filter by Name"), this);
	filterImagesFocusAct->setObjectName("filterImagesFocus");
	connect(filterImagesFocusAct, SIGNAL(triggered()), this, SLOT(filterImagesFocus()));
	setPathFocusAct = new QAction(tr("Set Path"), this);
	setPathFocusAct->setObjectName("setPathFocus");
	connect(setPathFocusAct, SIGNAL(triggered()), this, SLOT(setPathFocus()));
}

void BiManagerWindow::createMenus()
{
	fileMenu = menuBar()->addMenu(tr("&File"));
	fileMenu->addAction(subFoldersAction);
	fileMenu->addAction(createDirAction);
	fileMenu->addAction(showClipboardAction);
	fileMenu->addAction(addBookmarkAction);
	fileMenu->addSeparator();
	fileMenu->addAction(exitAction);

	editMenu = menuBar()->addMenu(tr("&Edit"));
	editMenu->addAction(cutAction);
	editMenu->addAction(copyAction);
	editMenu->addAction(copyToAction);
	editMenu->addAction(moveToAction);
	editMenu->addAction(pasteAction);
	editMenu->addAction(renameAction);
	editMenu->addAction(deleteAction);
	editMenu->addSeparator();
	editMenu->addAction(selectAllAction);
	editMenu->addAction(invertSelectionAct);
	addAction(filterImagesFocusAct);
	addAction(setPathFocusAct);
	editMenu->addSeparator();
	editMenu->addAction(settingsAction);

	goMenu = menuBar()->addMenu(tr("&Go"));
	goMenu->addAction(goBackAction);
	goMenu->addAction(goFrwdAction);
	goMenu->addAction(goUpAction);
	goMenu->addAction(goHomeAction);
	goMenu->addSeparator();
	goMenu->addAction(thumbsGoTopAct);
	goMenu->addAction(thumbsGoBottomAct);

	viewMenu = menuBar()->addMenu(tr("&View"));
	viewMenu->addAction(slideShowAction);
	viewMenu->addSeparator();
	
	viewMenu->addAction(thumbsZoomInAct);
	viewMenu->addAction(thumbsZoomOutAct);
	sortMenu = viewMenu->addMenu(tr("Sort By"));
	sortTypesGroup = new QActionGroup(this);
	sortTypesGroup->addAction(actName);
	sortTypesGroup->addAction(actTime);
	sortTypesGroup->addAction(actSize);
	sortTypesGroup->addAction(actType);
	sortMenu->addActions(sortTypesGroup->actions());
	sortMenu->addSeparator();
	sortMenu->addAction(actReverse);
	viewMenu->addSeparator();

	thumbLayoutsGroup = new QActionGroup(this);
	thumbLayoutsGroup->addAction(actClassic);
	thumbLayoutsGroup->addAction(actCompact);
	thumbLayoutsGroup->addAction(actSquarish);
	viewMenu->addActions(thumbLayoutsGroup->actions());
	viewMenu->addSeparator();

	viewMenu->addAction(actShowLabels);
	viewMenu->addAction(actShowHidden);
	viewMenu->addSeparator();
	viewMenu->addAction(refreshAction);

	toolsMenu = menuBar()->addMenu(tr("&Tools"));
	toolsMenu->addAction(findDupesAction);

	menuBar()->addSeparator();
	helpMenu = menuBar()->addMenu(tr("&Help"));
	helpMenu->addAction(aboutAction);

	// thumbview context menu
	thumbView->addAction(openAction);
	thumbView->addAction(openWithMenuAct);
	thumbView->addAction(cutAction);
	thumbView->addAction(copyAction);
	thumbView->addAction(pasteAction);
	addMenuSeparator(thumbView);
	thumbView->addAction(copyToAction);
	thumbView->addAction(moveToAction);
	thumbView->addAction(renameAction);
	thumbView->addAction(deleteAction);
	addMenuSeparator(thumbView);
	thumbView->addAction(selectAllAction);
	thumbView->addAction(invertSelectionAct);
	thumbView->setContextMenuPolicy(Qt::ActionsContextMenu);
	menuBar()->setVisible(true);
}

void BiManagerWindow::createToolBars()
{
	/* Edit */
	editToolBar = addToolBar(tr("Edit"));
	editToolBar->setObjectName("Edit");
	editToolBar->addAction(cutAction);
	editToolBar->addAction(copyAction);
	editToolBar->addAction(pasteAction);
	editToolBar->addAction(deleteAction);
	editToolBar->addAction(showClipboardAction);
	connect(editToolBar->toggleViewAction(), SIGNAL(triggered()), this, SLOT(setEditToolBarVisibility()));

	/* Navigation */
	goToolBar = addToolBar(tr("Navigation"));
	goToolBar->setObjectName("Navigation");
	goToolBar->addAction(goBackAction);
	goToolBar->addAction(goFrwdAction);
	goToolBar->addAction(goUpAction);
	goToolBar->addAction(goHomeAction);
	goToolBar->addAction(refreshAction);

	/* path bar */
	pathBar = new QLineEdit;
	pathBar->setCompleter(new DirCompleter(pathBar));
	pathBar->setMinimumWidth(200);
	pathBar->setMaximumWidth(300);
	connect(pathBar, SIGNAL(returnPressed()), this, SLOT(goPathBarDir()));
	goToolBar->addWidget(pathBar);
	goToolBar->addAction(subFoldersAction);
	goToolBar->addAction(findDupesAction);
	connect(goToolBar->toggleViewAction(), SIGNAL(triggered()), this, SLOT(setGoToolBarVisibility()));

	/* View */
	viewToolBar = addToolBar(tr("View"));
	viewToolBar->setObjectName("View");
	viewToolBar->addAction(slideShowAction);
	viewToolBar->addAction(thumbsZoomInAct);
	viewToolBar->addAction(thumbsZoomOutAct);

	/* filter bar */ 
	QAction *filterAct = new QAction(tr("Filter"), this);
	filterAct->setIcon(QIcon::fromTheme("edit-find", QIcon(":/images/zoom.png")));
	connect(filterAct, SIGNAL(triggered()), this, SLOT(setThumbsFilter()));
	filterBar = new QLineEdit;
	filterBar->setMinimumWidth(100);
	filterBar->setMaximumWidth(200);
	connect(filterBar, SIGNAL(returnPressed()), this, SLOT(setThumbsFilter()));
	connect(filterBar, SIGNAL(textChanged(const QString&)), this, SLOT(clearThumbsFilter()));
	filterBar->setClearButtonEnabled(true);
	filterBar->addAction(filterAct, QLineEdit::LeadingPosition);

	viewToolBar->addSeparator();
	viewToolBar->addWidget(filterBar);
	viewToolBar->addAction(settingsAction);
	connect(viewToolBar->toggleViewAction(), SIGNAL(triggered()), this, SLOT(setViewToolBarVisibility()));	

	/* image */
	imageToolBar = addToolBar(tr("Image"));
	imageToolBar->setObjectName("Image");
	imageToolBar->addAction(prevImageAction);
	imageToolBar->addAction(nextImageAction);
	imageToolBar->addAction(firstImageAction);
	imageToolBar->addAction(lastImageAction);
	imageToolBar->addAction(slideShowAction);
	imageToolBar->addSeparator();
	imageToolBar->addAction(saveAction);
	imageToolBar->addAction(saveAsAction);
	imageToolBar->addAction(deleteAction);
	imageToolBar->addSeparator();
	imageToolBar->addAction(zoomInAct);
	imageToolBar->addAction(zoomOutAct);
	imageToolBar->addAction(resetZoomAct);
	imageToolBar->addAction(origZoomAct);
	imageToolBar->addSeparator();
	imageToolBar->addAction(resizeAct);
	imageToolBar->addAction(rotateRightAct);
	imageToolBar->addAction(rotateLeftAct);
	imageToolBar->addAction(flipHAct);
	imageToolBar->addAction(flipVAct);
	imageToolBar->addAction(cropAct);
	imageToolBar->addAction(colorsAct);
	imageToolBar->setVisible(false);
	connect(imageToolBar->toggleViewAction(), SIGNAL(triggered()), this, SLOT(setImageToolBarVisibility()));

	setToolbarIconSize();
}

void BiManagerWindow::setToolbarIconSize()
{
	int iconSize;
	if (initComplete)
        BimData::smallIcons = actSmallIcons->isChecked();
    iconSize = BimData::smallIcons? 16 : 24;
	QSize iconQSize(iconSize, iconSize);

	editToolBar->setIconSize(iconQSize);
	goToolBar->setIconSize(iconQSize);
	viewToolBar->setIconSize(iconQSize);
	imageToolBar->setIconSize(iconQSize);
}

void BiManagerWindow::createStatusBar()
{
	stateLabel = new QLabel(tr("Initializing..."));
	statusBar()->addWidget(stateLabel);

	busyMovie = new QMovie(":/images/busy.gif");
	busyLabel = new QLabel(this);
	busyLabel->setMovie(busyMovie);
	statusBar()->addWidget(busyLabel);
	busyLabel->setVisible(false);

	statusBar()->setStyleSheet("QStatusBar::item { border: 0px solid black }; ");
}

void BiManagerWindow::createFSTree()
{
	fsDock = new QDockWidget(tr("File System"), this);
	fsDock->setObjectName("File System");

	fsTree = new FSTree(fsDock);
	fsDock->setWidget(fsTree);
	connect(fsDock->toggleViewAction(), SIGNAL(triggered()), this, SLOT(setFsDockVisibility()));	
	connect(fsDock, SIGNAL(visibilityChanged(bool)), this, SLOT(setFsDockVisibility()));	
	addDockWidget(Qt::LeftDockWidgetArea, fsDock);

	// Context menu
	fsTree->addAction(openAction);
	fsTree->addAction(createDirAction);
	fsTree->addAction(renameAction);
	fsTree->addAction(deleteAction);
	addMenuSeparator(fsTree);
	fsTree->addAction(pasteAction);
	addMenuSeparator(fsTree);
	fsTree->addAction(openWithMenuAct);
	fsTree->addAction(addBookmarkAction);
	fsTree->setContextMenuPolicy(Qt::ActionsContextMenu);

	connect(fsTree, SIGNAL(clicked(const QModelIndex&)),
				this, SLOT(goSelectedDir(const QModelIndex &)));

	connect(fsTree->fsModel, SIGNAL(rowsRemoved(const QModelIndex &, int, int)),
				this, SLOT(checkDirState(const QModelIndex &, int, int)));

	connect(fsTree, SIGNAL(dropOp(Qt::KeyboardModifiers, bool, QString)),
				this, SLOT(dropOp(Qt::KeyboardModifiers, bool, QString)));

	fsTree->setCurrentIndex(fsTree->fsModel->index(QDir::currentPath()));

	connect(fsTree->selectionModel(), SIGNAL(selectionChanged(QItemSelection, QItemSelection)),
				this, SLOT(updateActions()));
}

void BiManagerWindow::createBookmarks()
{
	bmDock = new QDockWidget(tr("Bookmarks"), this);
	bmDock->setObjectName("Bookmarks");
    bookmarks = new BookMarksTree(bmDock);
	bmDock->setWidget(bookmarks);
	
	connect(bmDock->toggleViewAction(), SIGNAL(triggered()), this, SLOT(setBmDockVisibility()));	
	connect(bmDock, SIGNAL(visibilityChanged(bool)), this, SLOT(setBmDockVisibility()));	
	connect(bookmarks, SIGNAL(itemClicked(QTreeWidgetItem *, int)),
					this, SLOT(bookmarkClicked(QTreeWidgetItem *, int)));
	connect(removeBookmarkAction, SIGNAL(triggered()), bookmarks, SLOT(removeBookmark()));
	connect(bookmarks, SIGNAL(dropOp(Qt::KeyboardModifiers, bool, QString)),
				this, SLOT(dropOp(Qt::KeyboardModifiers, bool, QString)));
	connect(bookmarks, SIGNAL(itemSelectionChanged()),
				this, SLOT(updateActions()));
	addDockWidget(Qt::LeftDockWidgetArea, bmDock);

	bookmarks->addAction(pasteAction);
	bookmarks->addAction(removeBookmarkAction);
	bookmarks->setContextMenuPolicy(Qt::ActionsContextMenu);
}

void BiManagerWindow::sortThumbnains()
{
	thumbView->thumbsSortFlags = QDir::IgnoreCase;

	if (actName->isChecked())
		thumbView->thumbsSortFlags |= QDir::Name;
	else if (actTime->isChecked())
		thumbView->thumbsSortFlags |= QDir::Time;
	else if (actSize->isChecked())
		thumbView->thumbsSortFlags |= QDir::Size;
	else if (actType->isChecked())
		thumbView->thumbsSortFlags |= QDir::Type;

	if (actReverse->isChecked())
		thumbView->thumbsSortFlags |= QDir::Reversed;

	refreshThumbs(false);
}

void BiManagerWindow::reload()
{
	findDupesAction->setChecked(false);
    if (BimData::layoutMode == thumbViewIdx) {
		refreshThumbs(false);
	} else {
		imageView->reload();
	}
}

void BiManagerWindow::setIncludeSubFolders()
{
	findDupesAction->setChecked(false);
    BimData::includeSubFolders = subFoldersAction->isChecked();
	refreshThumbs(false);
}

void BiManagerWindow::refreshThumbs(bool scrollToTop)
{
	thumbView->setNeedScroll(scrollToTop);
	QTimer::singleShot(0, this, SLOT(reloadThumbsSlot()));
	QTimer::singleShot(100, this, SLOT(selectRecentThumb()));
}

void BiManagerWindow::setClassicThumbs()
{
    BimData::thumbsLayout = ThumbnailView::Classic;
	actShowLabels->setEnabled(true);
	refreshThumbs(false);
}

void BiManagerWindow::setCompactThumbs()
{
    BimData::thumbsLayout = ThumbnailView::Compact;
	actShowLabels->setEnabled(true);
	refreshThumbs(false);
}

void BiManagerWindow::setSquarishThumbs()
{
    BimData::thumbsLayout = ThumbnailView::Squares;
	actShowLabels->setEnabled(false);
	refreshThumbs(false);
}

void BiManagerWindow::showHiddenFiles()
{
    BimData::showHiddenFiles = actShowHidden->isChecked();
	fsTree->setModelFlags();
	refreshThumbs(false);
}

void BiManagerWindow::toggleImageToolbar()
{
	imageToolBar->setVisible(actShowViewerToolbars->isChecked());
    BimData::imageToolbarFullScreen = actShowViewerToolbars->isChecked();
}

void BiManagerWindow::showLabels()
{
    BimData::showLabels = actShowLabels->isChecked();
	refreshThumbs(false);
}

void BiManagerWindow::about()
{
    QString aboutString = "<h2>Bulk Image Manager v0.0.1</h2>"
		+ tr("<p>Image viewer and organizer</p>")
		+ "Qt v" + QT_VERSION_STR
        ;

    QMessageBox::about(this, tr("About") + " Bim", aboutString);
}

void BiManagerWindow::filterImagesFocus()
{
    if (BimData::layoutMode == thumbViewIdx)
	{
		if (!viewToolBar->isVisible())
			viewToolBar->setVisible(true);
		setViewToolBarVisibility();
		filterBar->setFocus(Qt::OtherFocusReason);
		filterBar->selectAll();
	}
}

void BiManagerWindow::setPathFocus()
{
    if (BimData::layoutMode == thumbViewIdx)
	{
		if (!goToolBar->isVisible())
			goToolBar->setVisible(true);
		setGoToolBarVisibility();
		pathBar->setFocus(Qt::OtherFocusReason);
		pathBar->selectAll();
	}
}

void BiManagerWindow::cleanupSender()
{
	delete QObject::sender();
}

void BiManagerWindow::externalAppError()
{
	QMessageBox msgBox;
	msgBox.critical(this, tr("Error"), tr("Failed to start external application."));
}

void BiManagerWindow::runExternalApp()
{
	QString execCommand;
	QString selectedFileNames("");
    execCommand = BimData::externalApps[((QAction*) sender())->text()];

    if (BimData::layoutMode == imageViewIdx) {
		if (imageView->isNewImage()) 	{
			showNewImageWarning(this);
			return;
		}

		execCommand += " \"" + imageView->currentImageFullPath + "\"";
	} else {
		if (QApplication::focusWidget() == fsTree) {
			selectedFileNames += " \"" + getSelectedPath() + "\"";
		} else {

			QModelIndexList selectedIdxList = thumbView->selectionModel()->selectedIndexes();
			if (selectedIdxList.size() < 1)
			{
				setStatus(tr("Invalid selection."));
				return;
			}

			selectedFileNames += " ";
			for (int tn = selectedIdxList.size() - 1; tn >= 0 ; --tn)
			{
				selectedFileNames += "\"" +
					thumbView->thumbViewModel->item(selectedIdxList[tn].row())->data(thumbView->FileNameRole).toString();
				if (tn) 
					selectedFileNames += "\" ";
			}
		}
		
		execCommand += selectedFileNames;
	}

	QProcess *externalProcess = new QProcess();
	connect(externalProcess, SIGNAL(finished(int, QProcess::ExitStatus)), this, SLOT(cleanupSender()));
	connect(externalProcess, SIGNAL(error(QProcess::ProcessError)), this, SLOT(externalAppError()));
	externalProcess->start(execCommand);
}

void BiManagerWindow::updateExternalApps()
{
	int actionNum = 0;
    QMapIterator<QString, QString> eaIter(BimData::externalApps);

    QList<QAction*> actionList = openWithSubMenu->actions();
    if (!actionList.empty()) {

    	for (int i = 0; i < actionList.size(); ++i)
    	{
      		QAction *action = actionList.at(i);
      		if (action->isSeparator())
      			break;
			openWithSubMenu->removeAction(action);
			imageView->removeAction(action);
			delete action;
   		}

      	openWithSubMenu->clear();
    }

	while (eaIter.hasNext())
	{
		++actionNum;
		eaIter.next();
		QAction *extAppAct = new QAction(eaIter.key(), this);
		if (actionNum < 10)
			extAppAct->setShortcut(QKeySequence("Alt+" + QString::number(actionNum)));
		extAppAct->setIcon(QIcon::fromTheme(eaIter.key()));
		connect(extAppAct, SIGNAL(triggered()), this, SLOT(runExternalApp()));
		openWithSubMenu->addAction(extAppAct);
		imageView->addAction(extAppAct);
	}

	openWithSubMenu->addSeparator();
	openWithSubMenu->addAction(chooseAppAct);
}

void BiManagerWindow::chooseExternalApp()
{
	AppMgmtDialog *dialog = new AppMgmtDialog(this);

    if (BimData::slideShowActive)
		slideShow();
	imageView->setCursorHiding(false);

	dialog->exec();
	updateExternalApps();
	delete(dialog);

	if (isFullScreen()) {
		imageView->setCursorHiding(true);
	}
}

void BiManagerWindow::showSettings()
{
    if (BimData::slideShowActive)
		slideShow();
	imageView->setCursorHiding(false);
	
	SettingsDialog *dialog = new SettingsDialog(this);
	if (dialog->exec())
	{
        imageView->setPalette(QPalette(BimData::backgroundColor));
		thumbView->setThumbColors();
        BimData::imageZoomFactor = 1.0;
        imageView->infoLabel->setVisible(BimData::enableImageInfoFS && isFullScreen());

        if (BimData::layoutMode == imageViewIdx)
		{
			imageView->reload();
			needThumbsRefresh = true;
		}
		else
			refreshThumbs(false);
	}

	if (isFullScreen())
		imageView->setCursorHiding(true);
	delete dialog;
}

void BiManagerWindow::toggleFullScreen()
{
	if (fullScreenAct->isChecked())
	{
		shouldMaximize = isMaximized();
		showFullScreen();
        BimData::isFullScreen = true;
		imageView->setCursorHiding(true);
        imageView->infoLabel->setVisible(BimData::enableImageInfoFS);
	}
	else
	{
		showNormal();
		if (shouldMaximize)
			showMaximized();
		imageView->setCursorHiding(false);
        BimData::isFullScreen = false;
		imageView->infoLabel->setVisible(false);
	}
}

void BiManagerWindow::selectAllThumbs()
{
	thumbView->selectAll();
}

void BiManagerWindow::copyOrCutThumbs(bool copy)
{
    BimData::copyCutIdxList = thumbView->selectionModel()->selectedIndexes();
    copyCutCount = BimData::copyCutIdxList.size();

    BimData::copyCutFileList.clear();
	for (int tn = 0; tn < copyCutCount; ++tn)
	{
        BimData::copyCutFileList.append(thumbView->thumbViewModel->item(BimData::copyCutIdxList[tn].
										row())->data(thumbView->FileNameRole).toString());
	}

    BimData::copyOp = copy;
	pasteAction->setEnabled(true);
}

void BiManagerWindow::cutThumbs()
{
	copyOrCutThumbs(false);
}

void BiManagerWindow::copyThumbs()
{
	copyOrCutThumbs(true);
}

void BiManagerWindow::copyImagesTo()
{
	copyMoveImages(false);
}

void BiManagerWindow::moveImagesTo()
{
	copyMoveImages(true);
}

void BiManagerWindow::copyMoveImages(bool move)
{
    if (BimData::slideShowActive)
		slideShow();
	imageView->setCursorHiding(false);

	copyMoveToDialog = new CopyMoveToDialog(this, getSelectedPath(), move);
	if (copyMoveToDialog->exec()) {
        if (BimData::layoutMode == thumbViewIdx) {
			if (copyMoveToDialog->copyOp)
				copyThumbs();
			else
				cutThumbs();

			pasteThumbs();
		} else {
			if (imageView->isNewImage()) 	{
				showNewImageWarning(this);
				if (isFullScreen()) {
					imageView->setCursorHiding(true);
				}

				return;
			}
		
			QFileInfo fileInfo = QFileInfo(imageView->currentImageFullPath);
			QString fileName = fileInfo.fileName();
			QString destFile = copyMoveToDialog->selectedPath + QDir::separator() + fileInfo.fileName();
			
			int res = cpMvFile(copyMoveToDialog->copyOp, fileName, imageView->currentImageFullPath,
				 									destFile, copyMoveToDialog->selectedPath);

			if (!res) {
				QMessageBox msgBox;
				msgBox.critical(this, tr("Error"), tr("Failed to copy or move image."));
			} else {
				if (!copyMoveToDialog->copyOp) {
					int currentRow = thumbView->getCurrentRow();
					thumbView->thumbViewModel->removeRow(currentRow);
					updateCurrentImage(currentRow);
				}
			}
		}
	}

	bookmarks->reloadBookmarks();
	delete(copyMoveToDialog);
	copyMoveToDialog = 0;

	if (isFullScreen()) {
		imageView->setCursorHiding(true);
	}
}

void BiManagerWindow::thumbsZoomIn()
{
	if (thumbView->thumbSize < THUMB_SIZE_MAX)
	{
		thumbView->thumbSize += 50;
		thumbsZoomOutAct->setEnabled(true);
		if (thumbView->thumbSize == THUMB_SIZE_MAX)
			thumbsZoomInAct->setEnabled(false);
		refreshThumbs(false);
	}
}

void BiManagerWindow::thumbsZoomOut()
{
	if (thumbView->thumbSize > THUMB_SIZE_MIN)
	{
		thumbView->thumbSize -= 50;
		thumbsZoomInAct->setEnabled(true);
		if (thumbView->thumbSize == THUMB_SIZE_MIN)
			thumbsZoomOutAct->setEnabled(false);
		refreshThumbs(false);
	}
}

void BiManagerWindow::zoomOut()
{
    BimData::imageZoomFactor -= (BimData::imageZoomFactor <= 0.25)? 0 : 0.25;
	imageView->tempDisableResize = false;
	imageView->resizeImage();
    imageView->setFeedback(tr("Zoom %1%").arg(QString::number(BimData::imageZoomFactor * 100)));
}

void BiManagerWindow::zoomIn()
{
    BimData::imageZoomFactor += (BimData::imageZoomFactor >= 3.50)? 0 : 0.25;
	imageView->tempDisableResize = false;
	imageView->resizeImage();
    imageView->setFeedback(tr("Zoom %1%").arg(QString::number(BimData::imageZoomFactor * 100)));
}

void BiManagerWindow::resetZoom()
{
    BimData::imageZoomFactor = 1.0;
	imageView->tempDisableResize = false;
	imageView->resizeImage();
	imageView->setFeedback(tr("Zoom Reset"));
}

void BiManagerWindow::origZoom()
{
    BimData::imageZoomFactor = 1.0;
	imageView->tempDisableResize = true;
	imageView->resizeImage();
	imageView->setFeedback(tr("Original Size"));
}

void BiManagerWindow::keepZoom()
{
    BimData::keepZoomFactor = keepZoomAct->isChecked();
    if (BimData::keepZoomFactor)
		imageView->setFeedback(tr("Zoom Locked"));
	else
		imageView->setFeedback(tr("Zoom Unlocked"));
}

void BiManagerWindow::keepTransformClicked()
{
    BimData::keepTransform = keepTransformAct->isChecked();

    if (BimData::keepTransform) {
		imageView->setFeedback(tr("Transformations Locked"));
		if (cropDialog)
			cropDialog->applyCrop(0);
	} else {
        BimData::cropLeftPercent = BimData::cropTopPercent = BimData::cropWidthPercent = BimData::cropHeightPercent = 0;
		imageView->setFeedback(tr("Transformations Unlocked"));
	}

	imageView->refresh();
}

void BiManagerWindow::rotateLeft()
{
    BimData::rotation -= 90;
    if (BimData::rotation < 0)
        BimData::rotation = 270;
	imageView->refresh();
    imageView->setFeedback(tr("Rotation %1°").arg(QString::number(BimData::rotation)));
}

void BiManagerWindow::rotateRight()
{
    BimData::rotation += 90;
    if (BimData::rotation > 270)
        BimData::rotation = 0;
	imageView->refresh();
    imageView->setFeedback(tr("Rotation %1°").arg(QString::number(BimData::rotation)));
}

void BiManagerWindow::flipVert()
{
    BimData::flipV = !BimData::flipV;
	imageView->refresh();
    imageView->setFeedback(BimData::flipV? tr("Flipped Vertically") : tr("Unflipped Vertically"));
}

void BiManagerWindow::flipHoriz()
{
    BimData::flipH = !BimData::flipH;
	imageView->refresh();
    imageView->setFeedback(BimData::flipH? tr("Flipped Horizontally") : tr("Unflipped Horizontally"));
}

void BiManagerWindow::cropImage()
{
    if (BimData::slideShowActive)
		slideShow();

	if (!cropDialog) {
		cropDialog = new CropDialog(this, imageView);
		connect(cropDialog, SIGNAL(accepted()), this, SLOT(cleanupCropDialog()));
		connect(cropDialog, SIGNAL(rejected()), this, SLOT(cleanupCropDialog()));
	}

	cropDialog->show();
	setInterfaceEnabled(false);
	cropDialog->applyCrop(0);
}

void BiManagerWindow::scaleImage()
{
    if (BimData::slideShowActive)
		slideShow();

    if (BimData::layoutMode == thumbViewIdx && thumbView->selectionModel()->selectedIndexes().size() < 1) {
		setStatus(tr("No selection"));
		return;
	}

	resizeDialog = new ResizeDialog(this, imageView);
	connect(resizeDialog, SIGNAL(accepted()), this, SLOT(cleanupScaleDialog()));
	connect(resizeDialog, SIGNAL(rejected()), this, SLOT(cleanupScaleDialog()));

	resizeDialog->show();
	setInterfaceEnabled(false);
}

void BiManagerWindow::freeRotateLeft()
{
    --BimData::rotation;
    if (BimData::rotation < 0)
        BimData::rotation = 359;
	imageView->refresh();
    imageView->setFeedback(tr("Rotation %1°").arg(QString::number(BimData::rotation)));
}

void BiManagerWindow::freeRotateRight()
{
    ++BimData::rotation;
    if (BimData::rotation > 360)
        BimData::rotation = 1;
	imageView->refresh();
    imageView->setFeedback(tr("Rotation %1°").arg(QString::number(BimData::rotation)));
}

void BiManagerWindow::showColorsDialog()
{
    if (BimData::slideShowActive)
		slideShow();

	if (!colorsDialog) {
		colorsDialog = new ColorsDialog(this, imageView);
		connect(colorsDialog, SIGNAL(accepted()), this, SLOT(cleanupColorsDialog()));
		connect(colorsDialog, SIGNAL(rejected()), this, SLOT(cleanupColorsDialog()));
	}

    BimData::colorsActive = true;
	colorsDialog->show();
	colorsDialog->applyColors(0);
	setInterfaceEnabled(false);
}

void BiManagerWindow::moveRight()
{
    imageView->keyMoveEvent(ImageViewer::MoveRight);
}

void BiManagerWindow::moveLeft()
{
    imageView->keyMoveEvent(ImageViewer::MoveLeft);
}

void BiManagerWindow::moveUp()
{
    imageView->keyMoveEvent(ImageViewer::MoveUp);
}

void BiManagerWindow::moveDown()
{
    imageView->keyMoveEvent(ImageViewer::MoveDown);
}

void BiManagerWindow::setMirrorDisabled()
{
    imageView->mirrorLayout = ImageViewer::LayNone;
	imageView->refresh();
	imageView->setFeedback(tr("Mirroring Disabled"));
}

void BiManagerWindow::setMirrorDual()
{
    imageView->mirrorLayout = ImageViewer::LayDual;
	imageView->refresh();
	imageView->setFeedback(tr("Mirroring: Dual"));
}

void BiManagerWindow::setMirrorTriple()
{
    imageView->mirrorLayout = ImageViewer::LayTriple;
	imageView->refresh();
	imageView->setFeedback(tr("Mirroring: Triple"));
}

void BiManagerWindow::setMirrorVDual()
{
    imageView->mirrorLayout = ImageViewer::LayVDual;
	imageView->refresh();
	imageView->setFeedback(tr("Mirroring: Dual Vertical"));
}

void BiManagerWindow::setMirrorQuad()
{
    imageView->mirrorLayout = ImageViewer::LayQuad;
	imageView->refresh();
	imageView->setFeedback(tr("Mirroring: Quad"));
}

bool BiManagerWindow::isValidPath(QString &path)
{
	QDir checkPath(path);
	if (!checkPath.exists() || !checkPath.isReadable()) {
		return false;
	}
	return true;
}

void BiManagerWindow::pasteThumbs()
{
	if (!copyCutCount)
		return;

	QString destDir;
	if (copyMoveToDialog)
		destDir = copyMoveToDialog->selectedPath;
	else {
		if (QApplication::focusWidget() == bookmarks) {
			if (bookmarks->currentItem()) {
				destDir = bookmarks->currentItem()->toolTip(0);
			}
		} else {
			destDir = getSelectedPath();
		}
	}
	
	if (!isValidPath(destDir)) {
		QMessageBox msgBox;
		msgBox.critical(this, tr("Error"), tr("Can not copy or move to ") + destDir);
		selectCurrentViewDir();
		return;
	}

	bool pasteInCurrDir = (thumbView->currentViewDir == destDir);

	QFileInfo fileInfo;
    if (!BimData::copyOp && pasteInCurrDir) {
        for (int tn = 0; tn < BimData::copyCutFileList.size(); ++tn) {
            fileInfo = QFileInfo(BimData::copyCutFileList[tn]);
			if (fileInfo.absolutePath() == destDir) {
				QMessageBox msgBox;
				msgBox.critical(this, tr("Error"), tr("Can not copy or move to the same folder"));
				return;
			}
		}
	}

	CpMvDialog *dialog = new CpMvDialog(this);
	dialog->exec(thumbView, destDir, pasteInCurrDir);
	if (pasteInCurrDir) {
        for (int tn = 0; tn < BimData::copyCutFileList.size(); ++tn) {
            thumbView->addThumb(BimData::copyCutFileList[tn]);
		}
	}

    QString state = QString((BimData::copyOp? tr("Copied") : tr("Moved")) + " " +
								tr("%n image(s)", "", dialog->nfiles));
	setStatus(state);
	delete(dialog);
	selectCurrentViewDir();

	copyCutCount = 0;
    BimData::copyCutIdxList.clear();
    BimData::copyCutFileList.clear();
	pasteAction->setEnabled(false);

	thumbView->loadVisibleThumbs();
}

void BiManagerWindow::updateCurrentImage(int currentRow)
{
    bool wrapImageListTmp = BimData::wrapImageList;
    BimData::wrapImageList = false;

	if (currentRow == thumbView->thumbViewModel->rowCount())
	{
		thumbView->setCurrentRow(currentRow - 1);
	}

	if (thumbView->getNextRow() < 0 && currentRow > 0)
	{
		imageView->loadImage(thumbView->thumbViewModel->item(currentRow - 1)->
											data(thumbView->FileNameRole).toString());
	}
	else
	{
		if (thumbView->thumbViewModel->rowCount() == 0)
		{
			hideViewer();
			refreshThumbs(true);
			return;
		}
		imageView->loadImage(thumbView->thumbViewModel->item(currentRow)->
											data(thumbView->FileNameRole).toString());
	}
		
    BimData::wrapImageList = wrapImageListTmp;
	thumbView->setImageviewWindowTitle();
}

void BiManagerWindow::deleteViewerImage()
{
	if (imageView->isNewImage()) {
		showNewImageWarning(this);
		return;
	}

    if (BimData::slideShowActive)
		slideShow();

	imageView->setCursorHiding(false);

	bool ok;
	QFileInfo fileInfo = QFileInfo(imageView->currentImageFullPath);
	QString fileName = fileInfo.fileName();

	bool deleteConfirmed = true;
    if (BimData::deleteConfirm) {
		QMessageBox msgBox;
		msgBox.setText(tr("Permanently delete") + " " + fileName);
		msgBox.setWindowTitle(tr("Delete image"));
		msgBox.setIcon(QMessageBox::Warning);
		msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::Cancel);
		msgBox.setDefaultButton(QMessageBox::Yes);
		msgBox.setButtonText(QMessageBox::Yes, tr("Yes"));  
		msgBox.setButtonText(QMessageBox::Cancel, tr("Cancel"));  

		if (msgBox.exec() != QMessageBox::Yes) {
			deleteConfirmed = false;
		}
	}

	if (deleteConfirmed)
	{
		int currentRow = thumbView->getCurrentRow();

		ok = QFile::remove(imageView->currentImageFullPath);
		if (ok)
		{
			 thumbView->thumbViewModel->removeRow(currentRow);
		}
		else
		{
			QMessageBox msgBox;
			msgBox.critical(this, tr("Error"), tr("Failed to delete image"));
			if (isFullScreen()) {
				imageView->setCursorHiding(true);
			}
			return;
		}

		updateCurrentImage(currentRow);
	}
	if (isFullScreen()) {
		imageView->setCursorHiding(true);
	}
}

void BiManagerWindow::deleteOp()
{
	if (QApplication::focusWidget() == bookmarks) {
		bookmarks->removeBookmark();
		return;
	}

	if (QApplication::focusWidget() == fsTree) {
		deleteDir();
		return;
	}

    if (BimData::layoutMode == imageViewIdx) {
		deleteViewerImage();
		return;
	}

	if (thumbView->selectionModel()->selectedIndexes().size() < 1) {
		setStatus(tr("No selection"));
		return;
	}

	// deleting from thumbnail viewer
	bool deleteConfirmed = true;
    if (BimData::deleteConfirm) {
		QMessageBox msgBox;
		msgBox.setText(tr("Permanently delete selected images?"));
		msgBox.setWindowTitle(tr("Delete images"));
		msgBox.setIcon(QMessageBox::Warning);
		msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::Cancel);
		msgBox.setDefaultButton(QMessageBox::Yes);
		msgBox.setButtonText(QMessageBox::Yes, tr("Yes"));  
		msgBox.setButtonText(QMessageBox::Cancel, tr("Cancel"));  

		if (msgBox.exec() != QMessageBox::Yes) {
			deleteConfirmed = false;
		}
	}

	if (deleteConfirmed) {
		QModelIndexList indexesList;
		int nfiles = 0;
		bool ok;
	
		while((indexesList = thumbView->selectionModel()->selectedIndexes()).size()) {
			ok = QFile::remove(thumbView->thumbViewModel->item(
								indexesList.first().row())->data(thumbView->FileNameRole).toString());

			++nfiles;
			if (ok) {
				thumbView->thumbViewModel->removeRow(indexesList.first().row());
			} else {
				QMessageBox msgBox;
				msgBox.critical(this, tr("Error"), tr("Failed to delete image."));
				return;
			}
			if (thumbView->thumbViewModel->rowCount() > 0) {
				thumbView->setRowHidden(0 , false);
			}
		}
		
		QString state = QString(tr("Deleted") + " " + tr("%n image(s)", "", nfiles));
		setStatus(state);

		thumbView->loadVisibleThumbs();
	}
}

void BiManagerWindow::goTo(QString path)
{
	findDupesAction->setChecked(false);
	thumbView->setNeedScroll(true);
	fsTree->setCurrentIndex(fsTree->fsModel->index(path));
	thumbView->currentViewDir = path;
	refreshThumbs(true);
}

void BiManagerWindow::goSelectedDir(const QModelIndex &idx)
{
	findDupesAction->setChecked(false);
	thumbView->setNeedScroll(true);
	thumbView->currentViewDir = getSelectedPath();
	refreshThumbs(true);
	fsTree->expand(idx);
}

void BiManagerWindow::goPathBarDir()
{
	findDupesAction->setChecked(false);
	thumbView->setNeedScroll(true);

	QDir checkPath(pathBar->text());
	if (!checkPath.exists() || !checkPath.isReadable())
	{
		QMessageBox msgBox;
		msgBox.critical(this, tr("Error"), tr("Invalid Path:") + " " + pathBar->text());
		pathBar->setText(thumbView->currentViewDir);
		return;
	}
	
	thumbView->currentViewDir = pathBar->text();
	refreshThumbs(true);
	selectCurrentViewDir();
}

void BiManagerWindow::bookmarkClicked(QTreeWidgetItem *item, int col)
{
	goTo(item->toolTip(col));
}

void BiManagerWindow::setThumbsFilter()
{
	thumbView->filterStr = filterBar->text();
	refreshThumbs(true);
}

void BiManagerWindow::clearThumbsFilter()
{
	if (filterBar->text() == "")
	{
		thumbView->filterStr = filterBar->text();
		refreshThumbs(true);
	}
}

void BiManagerWindow::goBack()
{
	if (currentHistoryIdx > 0)
	{
		needHistoryRecord = false;
		goTo(pathHistory.at(--currentHistoryIdx));
		goFrwdAction->setEnabled(true);
		if (currentHistoryIdx == 0)
			goBackAction->setEnabled(false);
	}
}

void BiManagerWindow::goForward()
{

	if (currentHistoryIdx < pathHistory.size() - 1)
	{
		needHistoryRecord = false;
		goTo(pathHistory.at(++currentHistoryIdx));
		if (currentHistoryIdx == (pathHistory.size() - 1))
			goFrwdAction->setEnabled(false);
	}
}

void BiManagerWindow::goUp()
{
	QFileInfo fileInfo = QFileInfo(thumbView->currentViewDir);
	goTo(fileInfo.dir().absolutePath());
}

void BiManagerWindow::goHome()
{
	goTo(QDir::homePath());
}

void BiManagerWindow::setCopyCutActions(bool setEnabled)
{
	cutAction->setEnabled(setEnabled);
	copyAction->setEnabled(setEnabled);
}

void BiManagerWindow::setDeleteAction(bool setEnabled)
{
	deleteAction->setEnabled(setEnabled);
}

void BiManagerWindow::updateActions()
{
	if (QApplication::focusWidget() == thumbView) {
		setCopyCutActions(thumbView->selectionModel()->selectedIndexes().size());
		setDeleteAction(thumbView->selectionModel()->selectedIndexes().size());
	} else if (QApplication::focusWidget() == bookmarks) {
		setCopyCutActions(false);
		setDeleteAction(bookmarks->selectionModel()->selectedIndexes().size());
	} else if (QApplication::focusWidget() == fsTree) {
		setCopyCutActions(false);
		setDeleteAction(fsTree->selectionModel()->selectedIndexes().size());
    } else if (BimData::layoutMode == imageViewIdx) {
		setCopyCutActions(false);
		setDeleteAction(true);
	} else {
		setCopyCutActions(false);
		setDeleteAction(false);
	}

    if (BimData::layoutMode == imageViewIdx && !interfaceDisabled) {
		setViewerKeyEventsEnabled(true);
		fullScreenAct->setEnabled(true);
		closeImageAct->setEnabled(true);
	} else {
		if (QApplication::focusWidget() == imageView->scrlArea) {
			setViewerKeyEventsEnabled(true);
			fullScreenAct->setEnabled(false);
			closeImageAct->setEnabled(false);
		} else {
			setViewerKeyEventsEnabled(false);
			fullScreenAct->setEnabled(false);
			closeImageAct->setEnabled(false);
		}
	}
}

void BiManagerWindow::writeSettings()
{
    if (BimData::layoutMode == thumbViewIdx) {
        BimData::appSettings->setValue("Geometry", saveGeometry());
        BimData::appSettings->setValue("WindowState", saveState());
	}

    BimData::appSettings->setValue("thumbsSortFlags", (int)thumbView->thumbsSortFlags);
    BimData::appSettings->setValue("thumbsZoomVal", (int)thumbView->thumbSize);
    BimData::appSettings->setValue("isFullScreen", (bool)BimData::isFullScreen);
    BimData::appSettings->setValue("backgroundColor", BimData::backgroundColor);
    BimData::appSettings->setValue("backgroundThumbColor", BimData::thumbsBackgroundColor);
    BimData::appSettings->setValue("textThumbColor", BimData::thumbsTextColor);
    BimData::appSettings->setValue("thumbSpacing", (int)BimData::thumbSpacing);
    BimData::appSettings->setValue("thumbPagesReadahead", (int)BimData::thumbPagesReadahead);
    BimData::appSettings->setValue("thumbLayout", (int)BimData::thumbsLayout);
    BimData::appSettings->setValue("exitInsteadOfClose", (int)BimData::exitInsteadOfClose);
    BimData::appSettings->setValue("enableAnimations", (bool)BimData::enableAnimations);
    BimData::appSettings->setValue("exifRotationEnabled", (bool)BimData::exifRotationEnabled);
    BimData::appSettings->setValue("exifThumbRotationEnabled", (bool)BimData::exifThumbRotationEnabled);
    BimData::appSettings->setValue("reverseMouseBehavior", (bool)BimData::reverseMouseBehavior);
    BimData::appSettings->setValue("deleteConfirm", (bool)BimData::deleteConfirm);
    BimData::appSettings->setValue("showHiddenFiles", (bool)BimData::showHiddenFiles);
    BimData::appSettings->setValue("wrapImageList", (bool)BimData::wrapImageList);
    BimData::appSettings->setValue("imageZoomFactor", (float)BimData::imageZoomFactor);
    BimData::appSettings->setValue("shouldMaximize", (bool)isMaximized());
    BimData::appSettings->setValue("defaultSaveQuality", (int)BimData::defaultSaveQuality);
    BimData::appSettings->setValue("noEnlargeSmallThumb", (bool)BimData::noEnlargeSmallThumb);
    BimData::appSettings->setValue("slideShowDelay", (int)BimData::slideShowDelay);
    BimData::appSettings->setValue("slideShowRandom", (bool)BimData::slideShowRandom);
    BimData::appSettings->setValue("editToolBarVisible", (bool)editToolBarVisible);
    BimData::appSettings->setValue("goToolBarVisible", (bool)goToolBarVisible);
    BimData::appSettings->setValue("viewToolBarVisible", (bool)viewToolBarVisible);
    BimData::appSettings->setValue("imageToolBarVisible", (bool)imageToolBarVisible);
    BimData::appSettings->setValue("fsDockVisible", (bool)BimData::fsDockVisible);
    BimData::appSettings->setValue("iiDockVisible", (bool)BimData::iiDockVisible);
    BimData::appSettings->setValue("bmDockVisible", (bool)BimData::bmDockVisible);
    BimData::appSettings->setValue("pvDockVisible", (bool)BimData::pvDockVisible);
    BimData::appSettings->setValue("startupDir", (int)BimData::startupDir);
    BimData::appSettings->setValue("specifiedStartDir", BimData::specifiedStartDir);
    BimData::appSettings->setValue("thumbsBackImage", BimData::thumbsBackImage);
    BimData::appSettings->setValue("lastDir", BimData::startupDir == BimData::rememberLastDir?
																		thumbView->currentViewDir: "");
    BimData::appSettings->setValue("enableImageInfoFS", (bool)BimData::enableImageInfoFS);
    BimData::appSettings->setValue("showLabels", (bool)BimData::showLabels);
    BimData::appSettings->setValue("smallIcons", (bool)BimData::smallIcons);
    BimData::appSettings->setValue("LockDocks", (bool)BimData::LockDocks);
    BimData::appSettings->setValue("imageToolbarFullScreen", (bool)BimData::imageToolbarFullScreen);

	/* Action shortcuts */
    BimData::appSettings->beginGroup("Shortcuts");
    QMapIterator<QString, QAction *> scIter(BimData::actionKeys);
	while (scIter.hasNext()) {
		scIter.next();
        BimData::appSettings->setValue(scIter.key(), scIter.value()->shortcut().toString());
	}
    BimData::appSettings->endGroup();

	/* External apps */
    BimData::appSettings->beginGroup("ExternalApps");
    BimData::appSettings->remove("");
    QMapIterator<QString, QString> eaIter(BimData::externalApps);
	while (eaIter.hasNext()) {
		eaIter.next();
        BimData::appSettings->setValue(eaIter.key(), eaIter.value());
	}
    BimData::appSettings->endGroup();

	/* save bookmarks */
	int idx = 0;
    BimData::appSettings->beginGroup("CopyMoveToPaths");
    BimData::appSettings->remove("");
    QSetIterator<QString> pathsIter(BimData::bookmarkPaths);
	while (pathsIter.hasNext()) {
        BimData::appSettings->setValue("path" + QString::number(++idx), pathsIter.next());
	}
    BimData::appSettings->endGroup();
}

void BiManagerWindow::readSettings()
{
	initComplete = false;
	needThumbsRefresh = false;

    if (!BimData::appSettings->contains("thumbsZoomVal")) {
		resize(800, 600);
        BimData::appSettings->setValue("thumbsSortFlags", (int)0);
        BimData::appSettings->setValue("thumbsZoomVal", (int)150);
        BimData::appSettings->setValue("isFullScreen", (bool)false);
        BimData::appSettings->setValue("backgroundColor", QColor(25, 25, 25));
        BimData::appSettings->setValue("backgroundThumbColor", QColor(200, 200, 200));
        BimData::appSettings->setValue("textThumbColor", QColor(25, 25, 25));
        BimData::appSettings->setValue("thumbSpacing", (int)10);
        BimData::appSettings->setValue("thumbPagesReadahead", (int)2);
        BimData::appSettings->setValue("thumbLayout", (int)BimData::thumbsLayout);
        BimData::appSettings->setValue("zoomOutFlags", (int)1);
        BimData::appSettings->setValue("zoomInFlags", (int)0);
        BimData::appSettings->setValue("wrapImageList", (bool)false);
        BimData::appSettings->setValue("exitInsteadOfClose", (int)0);
        BimData::appSettings->setValue("imageZoomFactor", (float)1.0);
        BimData::appSettings->setValue("defaultSaveQuality", (int)90);
        BimData::appSettings->setValue("noEnlargeSmallThumb", (bool)true);
        BimData::appSettings->setValue("enableAnimations", (bool)true);
        BimData::appSettings->setValue("exifRotationEnabled", (bool)true);
        BimData::appSettings->setValue("exifThumbRotationEnabled", (bool)false);
        BimData::appSettings->setValue("reverseMouseBehavior", (bool)false);
        BimData::appSettings->setValue("deleteConfirm", (bool)true);
        BimData::appSettings->setValue("showHiddenFiles", (bool)false);
        BimData::appSettings->setValue("slideShowDelay", (int)5);
        BimData::appSettings->setValue("slideShowRandom", (bool)false);
        BimData::appSettings->setValue("editToolBarVisible", (bool)true);
        BimData::appSettings->setValue("goToolBarVisible", (bool)true);
        BimData::appSettings->setValue("viewToolBarVisible", (bool)true);
        BimData::appSettings->setValue("imageToolBarVisible", (bool)false);
        BimData::appSettings->setValue("fsDockVisible", (bool)true);
        BimData::appSettings->setValue("bmDockVisible", (bool)true);
        BimData::appSettings->setValue("iiDockVisible", (bool)true);
        BimData::appSettings->setValue("pvDockVisible", (bool)true);
        BimData::appSettings->setValue("enableImageInfoFS", (bool)false);
        BimData::appSettings->setValue("showLabels", (bool)true);
        BimData::appSettings->setValue("smallIcons", (bool)false);
        BimData::appSettings->setValue("LockDocks", (bool)true);
        BimData::appSettings->setValue("imageToolbarFullScreen", (bool)false);
        BimData::bookmarkPaths.insert(QDir::homePath());
	}

    BimData::backgroundColor = BimData::appSettings->value("backgroundColor").value<QColor>();
    BimData::exitInsteadOfClose = BimData::appSettings->value("exitInsteadOfClose").toBool();
    BimData::enableAnimations = BimData::appSettings->value("enableAnimations").toBool();
    BimData::exifRotationEnabled = BimData::appSettings->value("exifRotationEnabled").toBool();
    BimData::exifThumbRotationEnabled = BimData::appSettings->value("exifThumbRotationEnabled").toBool();
    BimData::reverseMouseBehavior = BimData::appSettings->value("reverseMouseBehavior").toBool();
    BimData::deleteConfirm = BimData::appSettings->value("deleteConfirm").toBool();
    BimData::showHiddenFiles = BimData::appSettings->value("showHiddenFiles").toBool();
    BimData::wrapImageList = BimData::appSettings->value("wrapImageList").toBool();
    BimData::imageZoomFactor = BimData::appSettings->value("imageZoomFactor").toFloat();
    BimData::zoomOutFlags = BimData::appSettings->value("zoomOutFlags").toInt();
    BimData::zoomInFlags = BimData::appSettings->value("zoomInFlags").toInt();
    BimData::rotation = 0;
    BimData::keepTransform = false;
    shouldMaximize = BimData::appSettings->value("shouldMaximize").toBool();
    BimData::flipH = false;
    BimData::flipV = false;
    BimData::defaultSaveQuality = BimData::appSettings->value("defaultSaveQuality").toInt();
    BimData::noEnlargeSmallThumb = BimData::appSettings->value("noEnlargeSmallThumb").toBool();
    BimData::slideShowDelay = BimData::appSettings->value("slideShowDelay").toInt();
    BimData::slideShowRandom = BimData::appSettings->value("slideShowRandom").toBool();
    BimData::slideShowActive = false;
    editToolBarVisible = BimData::appSettings->value("editToolBarVisible").toBool();
    goToolBarVisible = BimData::appSettings->value("goToolBarVisible").toBool();
    viewToolBarVisible = BimData::appSettings->value("viewToolBarVisible").toBool();
    imageToolBarVisible = BimData::appSettings->value("imageToolBarVisible").toBool();
    BimData::fsDockVisible = BimData::appSettings->value("fsDockVisible").toBool();
    BimData::bmDockVisible = BimData::appSettings->value("bmDockVisible").toBool();
    BimData::iiDockVisible = BimData::appSettings->value("iiDockVisible").toBool();
    BimData::pvDockVisible = BimData::appSettings->value("pvDockVisible").toBool();
    BimData::startupDir = (BimData::StartupDir)BimData::appSettings->value("startupDir").toInt();
    BimData::specifiedStartDir = BimData::appSettings->value("specifiedStartDir").toString();
    BimData::thumbsBackImage = BimData::appSettings->value("thumbsBackImage").toString();
    BimData::enableImageInfoFS = BimData::appSettings->value("enableImageInfoFS").toBool();
    BimData::showLabels = BimData::appSettings->value("showLabels").toBool();
    BimData::smallIcons = BimData::appSettings->value("smallIcons").toBool();
    BimData::LockDocks = BimData::appSettings->value("LockDocks").toBool();
    BimData::imageToolbarFullScreen = BimData::appSettings->value("imageToolbarFullScreen").toBool();

    BimData::appSettings->beginGroup("ExternalApps");
    QStringList extApps = BimData::appSettings->childKeys();
	for (int i = 0; i < extApps.size(); ++i)
	{
        BimData::externalApps[extApps.at(i)] = BimData::appSettings->value(extApps.at(i)).toString();
	}
    BimData::appSettings->endGroup();

    BimData::appSettings->beginGroup("CopyMoveToPaths");
    QStringList paths = BimData::appSettings->childKeys();
	for (int i = 0; i < paths.size(); ++i)
	{
        BimData::bookmarkPaths.insert(BimData::appSettings->value(paths.at(i)).toString());
	}
    BimData::appSettings->endGroup();
}

void BiManagerWindow::setupDocks()
{
	pvDock = new QDockWidget(tr("Viewer"), this);
	pvDock->setObjectName("Viewer");

	imageViewContainer = new QVBoxLayout;
	imageViewContainer->setContentsMargins(0, 0, 0, 0);
	imageViewContainer->addWidget(imageView);
	QWidget *imageViewContainerWidget = new QWidget;
	imageViewContainerWidget->setLayout(imageViewContainer);
	
	pvDock->setWidget(imageViewContainerWidget);
	connect(pvDock->toggleViewAction(), SIGNAL(triggered()), this, SLOT(setPvDockVisibility()));	
	connect(pvDock, SIGNAL(visibilityChanged(bool)), this, SLOT(setPvDockVisibility()));	
	addDockWidget(Qt::RightDockWidgetArea, pvDock);
	addDockWidget(Qt::RightDockWidgetArea, iiDock);

	QAction *docksNToolbarsAct = viewMenu->insertMenu(refreshAction, createPopupMenu());
	docksNToolbarsAct->setText(tr("Docks and Toolbars"));

	fsDockOrigWidget = fsDock->titleBarWidget();
	bmDockOrigWidget = bmDock->titleBarWidget();
	iiDockOrigWidget = iiDock->titleBarWidget();
	pvDockOrigWidget = pvDock->titleBarWidget();
	fsDockEmptyWidget = new QWidget;
	bmDockEmptyWidget = new QWidget;
	iiDockEmptyWidget = new QWidget;
	pvDockEmptyWidget = new QWidget;
	lockDocks();

	setDockOptions(QMainWindow::AllowNestedDocks);
}

void BiManagerWindow::lockDocks()
{
	if (initComplete)
        BimData::LockDocks = actLockDocks->isChecked();

    if (BimData::LockDocks) {
		fsDock->setTitleBarWidget(fsDockEmptyWidget);
		bmDock->setTitleBarWidget(bmDockEmptyWidget);
		iiDock->setTitleBarWidget(iiDockEmptyWidget);
		pvDock->setTitleBarWidget(pvDockEmptyWidget);
	} else {
		fsDock->setTitleBarWidget(fsDockOrigWidget);
		bmDock->setTitleBarWidget(bmDockOrigWidget);
		iiDock->setTitleBarWidget(iiDockOrigWidget);
		pvDock->setTitleBarWidget(pvDockOrigWidget);
	}
}

QMenu *BiManagerWindow::createPopupMenu()
{
	QMenu *testMenu = QMainWindow::createPopupMenu();
	testMenu->addSeparator();
	testMenu->addAction(actSmallIcons);
	testMenu->addAction(actLockDocks);
	return testMenu;
}

void BiManagerWindow::loadShortcuts()
{
	// Add customizable key shortcut actions
    BimData::actionKeys[thumbsGoTopAct->objectName()] = thumbsGoTopAct;
    BimData::actionKeys[thumbsGoBottomAct->objectName()] = thumbsGoBottomAct;
    BimData::actionKeys[closeImageAct->objectName()] = closeImageAct;
    BimData::actionKeys[fullScreenAct->objectName()] = fullScreenAct;
    BimData::actionKeys[settingsAction->objectName()] = settingsAction;
    BimData::actionKeys[exitAction->objectName()] = exitAction;
    BimData::actionKeys[thumbsZoomInAct->objectName()] = thumbsZoomInAct;
    BimData::actionKeys[thumbsZoomOutAct->objectName()] = thumbsZoomOutAct;
    BimData::actionKeys[cutAction->objectName()] = cutAction;
    BimData::actionKeys[copyAction->objectName()] = copyAction;
    BimData::actionKeys[nextImageAction->objectName()] = nextImageAction;
    BimData::actionKeys[prevImageAction->objectName()] = prevImageAction;
    BimData::actionKeys[deleteAction->objectName()] = deleteAction;
    BimData::actionKeys[saveAction->objectName()] = saveAction;
    BimData::actionKeys[saveAsAction->objectName()] = saveAsAction;
    BimData::actionKeys[keepTransformAct->objectName()] = keepTransformAct;
    BimData::actionKeys[keepZoomAct->objectName()] = keepZoomAct;
    BimData::actionKeys[showClipboardAction->objectName()] = showClipboardAction;
    BimData::actionKeys[copyImageAction->objectName()] = copyImageAction;
    BimData::actionKeys[pasteImageAction->objectName()] = pasteImageAction;
    BimData::actionKeys[renameAction->objectName()] = renameAction;
    BimData::actionKeys[refreshAction->objectName()] = refreshAction;
    BimData::actionKeys[pasteAction->objectName()] = pasteAction;
    BimData::actionKeys[goBackAction->objectName()] = goBackAction;
    BimData::actionKeys[goFrwdAction->objectName()] = goFrwdAction;
    BimData::actionKeys[slideShowAction->objectName()] = slideShowAction;
    BimData::actionKeys[firstImageAction->objectName()] = firstImageAction;
    BimData::actionKeys[lastImageAction->objectName()] = lastImageAction;
    BimData::actionKeys[randomImageAction->objectName()] = randomImageAction;
    BimData::actionKeys[openAction->objectName()] = openAction;
    BimData::actionKeys[zoomOutAct->objectName()] = zoomOutAct;
    BimData::actionKeys[zoomInAct->objectName()] = zoomInAct;
    BimData::actionKeys[resetZoomAct->objectName()] = resetZoomAct;
    BimData::actionKeys[origZoomAct->objectName()] = origZoomAct;
    BimData::actionKeys[rotateLeftAct->objectName()] = rotateLeftAct;
    BimData::actionKeys[rotateRightAct->objectName()] = rotateRightAct;
    BimData::actionKeys[freeRotateLeftAct->objectName()] = freeRotateLeftAct;
    BimData::actionKeys[freeRotateRightAct->objectName()] = freeRotateRightAct;
    BimData::actionKeys[flipHAct->objectName()] = flipHAct;
    BimData::actionKeys[flipVAct->objectName()] = flipVAct;
    BimData::actionKeys[cropAct->objectName()] = cropAct;
    BimData::actionKeys[cropToSelectionAct->objectName()] = cropToSelectionAct;
    BimData::actionKeys[colorsAct->objectName()] = colorsAct;
    BimData::actionKeys[mirrorDisabledAct->objectName()] = mirrorDisabledAct;
    BimData::actionKeys[mirrorDualAct->objectName()] = mirrorDualAct;
    BimData::actionKeys[mirrorTripleAct->objectName()] = mirrorTripleAct;
    BimData::actionKeys[mirrorVDualAct->objectName()] = mirrorVDualAct;
    BimData::actionKeys[mirrorQuadAct->objectName()] = mirrorQuadAct;
    BimData::actionKeys[moveDownAct->objectName()] = moveDownAct;
    BimData::actionKeys[moveUpAct->objectName()] = moveUpAct;
    BimData::actionKeys[moveRightAct->objectName()] = moveRightAct;
    BimData::actionKeys[moveLeftAct->objectName()] = moveLeftAct;
    BimData::actionKeys[copyToAction->objectName()] = copyToAction;
    BimData::actionKeys[moveToAction->objectName()] = moveToAction;
    BimData::actionKeys[goUpAction->objectName()] = goUpAction;
    BimData::actionKeys[resizeAct->objectName()] = resizeAct;
    BimData::actionKeys[filterImagesFocusAct->objectName()] = filterImagesFocusAct;
    BimData::actionKeys[setPathFocusAct->objectName()] = setPathFocusAct;
    BimData::actionKeys[keepTransformAct->objectName()] = keepTransformAct;
	
    BimData::appSettings->beginGroup("Shortcuts");
    QStringList groupKeys = BimData::appSettings->childKeys();

	if (groupKeys.size())
	{
		if (groupKeys.contains(thumbsGoTopAct->text()))
		{
            QMapIterator<QString, QAction *> key(BimData::actionKeys);
			while (key.hasNext()) {
				key.next();
				if (groupKeys.contains(key.value()->text()))
				{
                    key.value()->setShortcut(BimData::appSettings->value(key.value()->text()).toString());
                    BimData::appSettings->remove(key.value()->text());
                    BimData::appSettings->setValue(key.key(), key.value()->shortcut().toString());
				}
			}
		}
		else
		{
			for (int i = 0; i < groupKeys.size(); ++i)
			{
                if (BimData::actionKeys.value(groupKeys.at(i)))
                    BimData::actionKeys.value(groupKeys.at(i))->setShortcut
                                    (BimData::appSettings->value(groupKeys.at(i)).toString());
			}
		}
	}
	else
	{
		thumbsGoTopAct->setShortcut(QKeySequence("Ctrl+Home"));
		thumbsGoBottomAct->setShortcut(QKeySequence("Ctrl+End"));
		closeImageAct->setShortcut(Qt::Key_Escape);
		fullScreenAct->setShortcut(QKeySequence("F"));
		settingsAction->setShortcut(QKeySequence("P"));
		exitAction->setShortcut(QKeySequence("Ctrl+Q"));
		cutAction->setShortcut(QKeySequence("Ctrl+X"));
		copyAction->setShortcut(QKeySequence("Ctrl+C"));
		deleteAction->setShortcut(QKeySequence("Del"));
		saveAction->setShortcut(QKeySequence("Ctrl+S"));
		copyImageAction->setShortcut(QKeySequence("Ctrl+Shift+C"));
		pasteImageAction->setShortcut(QKeySequence("Ctrl+Shift+V"));
		renameAction->setShortcut(QKeySequence("F2"));
		refreshAction->setShortcut(QKeySequence("F5"));
		pasteAction->setShortcut(QKeySequence("Ctrl+V"));
		goBackAction->setShortcut(QKeySequence("Alt+Left"));
		goFrwdAction->setShortcut(QKeySequence("Alt+Right"));
		goUpAction->setShortcut(QKeySequence("Alt+Up"));
		slideShowAction->setShortcut(QKeySequence("W"));
		nextImageAction->setShortcut(QKeySequence("PgDown"));
		prevImageAction->setShortcut(QKeySequence("PgUp"));
		firstImageAction->setShortcut(QKeySequence("Home"));
		lastImageAction->setShortcut(QKeySequence("End"));
		randomImageAction->setShortcut(QKeySequence("R"));
		openAction->setShortcut(QKeySequence("Return"));
		zoomOutAct->setShortcut(QKeySequence("Alt+Z"));
		zoomInAct->setShortcut(QKeySequence("Z"));
		resetZoomAct->setShortcut(QKeySequence("Ctrl+Z"));
		origZoomAct->setShortcut(QKeySequence("Shift+Z"));
		rotateLeftAct->setShortcut(QKeySequence("Ctrl+Left"));
		rotateRightAct->setShortcut(QKeySequence("Ctrl+Right"));
		freeRotateLeftAct->setShortcut(QKeySequence("Ctrl+Shift+Left"));
		freeRotateRightAct->setShortcut(QKeySequence("Ctrl+Shift+Right"));
		flipHAct->setShortcut(QKeySequence("Ctrl+Down"));
		flipVAct->setShortcut(QKeySequence("Ctrl+Up"));
		cropAct->setShortcut(QKeySequence("Ctrl+G"));
		cropToSelectionAct->setShortcut(QKeySequence("Ctrl+R"));
		colorsAct->setShortcut(QKeySequence("C"));
		mirrorDisabledAct->setShortcut(QKeySequence("Ctrl+1"));
		mirrorDualAct->setShortcut(QKeySequence("Ctrl+2"));
		mirrorTripleAct->setShortcut(QKeySequence("Ctrl+3"));
		mirrorVDualAct->setShortcut(QKeySequence("Ctrl+4"));
		mirrorQuadAct->setShortcut(QKeySequence("Ctrl+5"));
		moveDownAct->setShortcut(QKeySequence("Down"));
		moveUpAct->setShortcut(QKeySequence("Up"));
		moveLeftAct->setShortcut(QKeySequence("Left"));
		moveRightAct->setShortcut(QKeySequence("Right"));
		copyToAction->setShortcut(QKeySequence("Ctrl+Y"));
		moveToAction->setShortcut(QKeySequence("Ctrl+M"));
		resizeAct->setShortcut(QKeySequence("Ctrl+I"));
		filterImagesFocusAct->setShortcut(QKeySequence("Ctrl+F"));
		setPathFocusAct->setShortcut(QKeySequence("Ctrl+L"));
		keepTransformAct->setShortcut(QKeySequence("Ctrl+K"));
	}
		
    BimData::appSettings->endGroup();
}

void BiManagerWindow::closeEvent(QCloseEvent *event)
{
	thumbView->abort();
	writeSettings();
	if (!QApplication::clipboard()->image().isNull())
		QApplication::clipboard()->clear();
	event->accept();
}

void BiManagerWindow::setStatus(QString state)
{
	stateLabel->setText("    " + state + "    ");
}

void BiManagerWindow::mouseDoubleClickEvent(QMouseEvent *event)
{
	if (interfaceDisabled)
		return;

	if (event->button() == Qt::LeftButton) {
        if (BimData::layoutMode == imageViewIdx) {
            if (BimData::reverseMouseBehavior)
			{
				fullScreenAct->setChecked(!(fullScreenAct->isChecked()));
				toggleFullScreen();
				event->accept();
			}
			else if (closeImageAct->isEnabled())
			{
				hideViewer();
				event->accept();
			}
		} else {
			if (QApplication::focusWidget() == imageView->scrlArea) {
				openOp();
			}
		}
	}
}

void BiManagerWindow::mousePressEvent(QMouseEvent *event)
{
	if (interfaceDisabled)
		return;

    if (BimData::layoutMode == imageViewIdx) {
		if (event->button() == Qt::MiddleButton) {

			if (event->modifiers() == Qt::ShiftModifier) {
				origZoom();
				event->accept();
				return;
			}
			if (event->modifiers() == Qt::ControlModifier) {
				resetZoom();
				event->accept();
				return;
			}

            if (BimData::reverseMouseBehavior && closeImageAct->isEnabled()) {
				hideViewer();
				event->accept();
			} else {
				fullScreenAct->setChecked(!(fullScreenAct->isChecked()));
				toggleFullScreen();
				event->accept();
			}
		}
	} else {
		if (QApplication::focusWidget() == imageView->scrlArea) {
            if (event->button() == Qt::MiddleButton && BimData::reverseMouseBehavior) {
				openOp();
			}
		}
	}
}

void BiManagerWindow::newImage()
{
    if (BimData::layoutMode == thumbViewIdx)
		showViewer();

	imageView->loadImage("");
}

void BiManagerWindow::setDocksVisibility(bool visible)
{
	if (!visible) {
		fsDock->setMaximumHeight(fsDock->height());
		bmDock->setMaximumHeight(bmDock->height());
		iiDock->setMaximumHeight(iiDock->height());
		pvDock->setMaximumHeight(pvDock->height());
		fsDock->setMaximumWidth(fsDock->width());
		bmDock->setMaximumWidth(bmDock->width());
		iiDock->setMaximumWidth(iiDock->width());
		pvDock->setMaximumWidth(pvDock->width());
	}

    fsDock->setVisible(visible? BimData::fsDockVisible : false);
    bmDock->setVisible(visible? BimData::bmDockVisible : false);
    iiDock->setVisible(visible? BimData::iiDockVisible : false);
    pvDock->setVisible(visible? BimData::pvDockVisible : false);

	menuBar()->setVisible(visible);
	menuBar()->setDisabled(!visible);
	statusBar()->setVisible(visible);

	editToolBar->setVisible(visible? editToolBarVisible : false);
	goToolBar->setVisible(visible? goToolBarVisible : false);
	viewToolBar->setVisible(visible? viewToolBarVisible : false);
    imageToolBar->setVisible(visible? imageToolBarVisible : BimData::imageToolbarFullScreen);

	setContextMenuPolicy(Qt::PreventContextMenu);
}

void BiManagerWindow::openOp()
{
    if (BimData::layoutMode == imageViewIdx)
	{
		hideViewer();
		return;
	}
	
	if (QApplication::focusWidget() == fsTree)
	{
		goSelectedDir(fsTree->getCurrentIndex());
		return;
	}
	else if (QApplication::focusWidget() == thumbView 
				|| QApplication::focusWidget() == imageView->scrlArea)
	{
		QModelIndex idx;
		QModelIndexList indexesList = thumbView->selectionModel()->selectedIndexes();
		if (indexesList.size() > 0)
			idx = indexesList.first();
		else
		{
			if (thumbView->thumbViewModel->rowCount() == 0)
			{
				setStatus(tr("No images"));
				return;
			}

			idx = thumbView->thumbViewModel->indexFromItem(thumbView->thumbViewModel->item(0));
			thumbView->selectionModel()->select(idx, QItemSelectionModel::Toggle);
			thumbView->setCurrentRow(0);
		}

		loadImagefromThumb(idx);
		return;
	}
	else if (QApplication::focusWidget() == filterBar)
	{
		setThumbsFilter();
		return;
	}
	else if (QApplication::focusWidget() == pathBar)
	{
		goPathBarDir();
		return;
	}
}

void BiManagerWindow::setEditToolBarVisibility()
{
	editToolBarVisible = editToolBar->isVisible();
}

void BiManagerWindow::setGoToolBarVisibility()
{
	goToolBarVisible = goToolBar->isVisible();
}

void BiManagerWindow::setViewToolBarVisibility()
{
	viewToolBarVisible = viewToolBar->isVisible();
}

void BiManagerWindow::setImageToolBarVisibility()
{
	imageToolBarVisible = imageToolBar->isVisible();
}

void BiManagerWindow::setFsDockVisibility()
{
    if (BimData::layoutMode == imageViewIdx)
		return;

    BimData::fsDockVisible = fsDock->isVisible();
}

void BiManagerWindow::setBmDockVisibility()
{
    if (BimData::layoutMode == imageViewIdx)
		return;

    BimData::bmDockVisible = bmDock->isVisible();
}

void BiManagerWindow::setIiDockVisibility()
{
    if (BimData::layoutMode == imageViewIdx)
		return;

    BimData::iiDockVisible = iiDock->isVisible();
}

void BiManagerWindow::setPvDockVisibility()
{
    if (BimData::layoutMode == imageViewIdx)
		return;

    BimData::pvDockVisible = pvDock->isVisible();
}

void BiManagerWindow::showViewer()
{
    if (BimData::layoutMode == thumbViewIdx) {
        BimData::layoutMode = imageViewIdx;
        BimData::appSettings->setValue("Geometry", saveGeometry());
        BimData::appSettings->setValue("WindowState", saveState());

		imageViewContainer->removeWidget(imageView);
		mainLayout->addWidget(imageView);
		imageView->setVisible(true);
		thumbView->setVisible(false);
		setDocksVisibility(false);
        if (BimData::isFullScreen == true) {
			shouldMaximize = isMaximized();
			showFullScreen();
			imageView->setCursorHiding(true);
            imageView->infoLabel->setVisible(BimData::enableImageInfoFS);
		}
		imageView->adjustSize();

		updateActions();
	}
}

void BiManagerWindow::showBusyStatus(bool busy)
{
	static int busyStatus = 0;

	if (busy)
		++busyStatus;
	else
		--busyStatus;

	if (busyStatus > 0) {
		busyMovie->start();
		busyLabel->setVisible(true);
	} else {
		busyLabel->setVisible(false);
		busyMovie->stop();
		busyStatus = 0;
	}
}

void BiManagerWindow::loadImagefromThumb(const QModelIndex &idx)
{
	thumbView->setCurrentRow(idx.row());
	showViewer();
	imageView->loadImage(thumbView->thumbViewModel->item(idx.row())->data(thumbView->FileNameRole).toString());
	thumbView->setImageviewWindowTitle();
}

void BiManagerWindow::updateViewerImageBySelection(const QItemSelection&)
{
    if (!pvDock->isVisible() || BimData::layoutMode == imageViewIdx)
		return;
			
	QModelIndexList indexesList = thumbView->selectionModel()->selectedIndexes();
	if (indexesList.size() == 1) {
		QString ImagePath = thumbView->thumbViewModel->item(indexesList.first().row())->data
																(thumbView->FileNameRole).toString();
		imageView->loadImage(ImagePath);
		thumbView->setCurrentRow(indexesList.first().row());
	} else {
		QString ImagePath(":/images/no_image.png");
		imageView->loadImage(ImagePath);
	}
}

void BiManagerWindow::loadImagefromCli()
{
	QFile imageFile(cliFileName);
	if(!imageFile.exists()) 
	{
		QMessageBox msgBox;
		msgBox.critical(this, tr("Error"), tr("Failed to open file \"%1\": file not found.").arg(cliFileName));
		cliFileName = "";
		return;
	}

	showViewer();
	imageView->loadImage(cliFileName);
    setWindowTitle(cliFileName + " - Bim");
}

void BiManagerWindow::slideShow()
{
    if (BimData::slideShowActive)
	{
        BimData::slideShowActive = false;
		slideShowAction->setText(tr("Slide Show"));
		imageView->setFeedback(tr("Slide show stopped"));

		SlideShowTimer->stop();
		delete SlideShowTimer;
		slideShowAction->setIcon(QIcon::fromTheme("media-playback-start", QIcon(":/images/play.png")));
	}
	else
	{
		if (thumbView->thumbViewModel->rowCount() <= 0)
			return;
	
        if (BimData::layoutMode == thumbViewIdx)
		{
			QModelIndexList indexesList = thumbView->selectionModel()->selectedIndexes();
			if (indexesList.size() != 1)
				thumbView->setCurrentRow(0);
			else
				thumbView->setCurrentRow(indexesList.first().row());

			showViewer();
		}
	
        BimData::slideShowActive = true;

		SlideShowTimer = new QTimer(this);
		connect(SlideShowTimer, SIGNAL(timeout()), this, SLOT(slideShowHandler()));
        SlideShowTimer->start(BimData::slideShowDelay * 1000);

		slideShowAction->setText(tr("Stop Slide Show"));
		imageView->setFeedback(tr("Slide show started"));
		slideShowAction->setIcon(QIcon::fromTheme("media-playback-stop", QIcon(":/images/stop.png")));

		slideShowHandler();
	}
}

void BiManagerWindow::slideShowHandler()
{
    if (BimData::slideShowActive)
	{
        if (BimData::slideShowRandom)
		{
			loadRandomImage();
		}
		else
		{
			int currentRow = thumbView->getCurrentRow();
			imageView->loadImage(thumbView->thumbViewModel->item(currentRow)->data(thumbView->FileNameRole).toString());
			thumbView->setImageviewWindowTitle();

			if (thumbView->getNextRow() > 0)
				thumbView->setCurrentRow(thumbView->getNextRow());
			else 
			{
                if (BimData::wrapImageList)
					thumbView->setCurrentRow(0);
				else
					slideShow();
			}
		}
	}
}

void BiManagerWindow::selectThumbByRow(int row)
{
	thumbView->setCurrentIndexByRow(row);
	thumbView->selectCurrentIndex();
}

void BiManagerWindow::loadNextImage()
{
	if (thumbView->thumbViewModel->rowCount() <= 0)
		return;

	int nextRow = thumbView->getNextRow();
	if (nextRow < 0) 
	{
        if (BimData::wrapImageList)
			nextRow = 0;
		else
			return;
	}

	imageView->loadImage(thumbView->thumbViewModel->item(nextRow)->data(thumbView->FileNameRole).toString());
	thumbView->setCurrentRow(nextRow);
	thumbView->setImageviewWindowTitle();

    if (BimData::layoutMode == thumbViewIdx)
		selectThumbByRow(nextRow);
}

void BiManagerWindow::loadPrevImage()
{
	if (thumbView->thumbViewModel->rowCount() <= 0)
		return;

	int prevRow = thumbView->getPrevRow();
	if (prevRow < 0) 
	{
        if (BimData::wrapImageList)
			prevRow = thumbView->getLastRow();
		else
			return;
	}
	
	imageView->loadImage(thumbView->thumbViewModel->item(prevRow)->data(thumbView->FileNameRole).toString());
	thumbView->setCurrentRow(prevRow);
	thumbView->setImageviewWindowTitle();

    if (BimData::layoutMode == thumbViewIdx)
		selectThumbByRow(prevRow);
}

void BiManagerWindow::loadFirstImage()
{
	if (thumbView->thumbViewModel->rowCount() <= 0)
		return;

	imageView->loadImage(thumbView->thumbViewModel->item(0)->data(thumbView->FileNameRole).toString());
	thumbView->setCurrentRow(0);
	thumbView->setImageviewWindowTitle();

    if (BimData::layoutMode == thumbViewIdx)
		selectThumbByRow(0);
}

void BiManagerWindow::loadLastImage()
{
	if (thumbView->thumbViewModel->rowCount() <= 0)
		return;

	int lastRow = thumbView->getLastRow();
	imageView->loadImage(thumbView->thumbViewModel->item(lastRow)->data(thumbView->FileNameRole).toString());
	thumbView->setCurrentRow(lastRow);
	thumbView->setImageviewWindowTitle();

    if (BimData::layoutMode == thumbViewIdx)
		selectThumbByRow(lastRow);
}

void BiManagerWindow::loadRandomImage()
{
	if (thumbView->thumbViewModel->rowCount() <= 0)
		return;

	int randomRow = thumbView->getRandomRow();
	imageView->loadImage(thumbView->thumbViewModel->item(randomRow)->data(thumbView->FileNameRole).toString());
	thumbView->setCurrentRow(randomRow);
	thumbView->setImageviewWindowTitle();

    if (BimData::layoutMode == thumbViewIdx)
		selectThumbByRow(randomRow);
}

void BiManagerWindow::selectRecentThumb()
{
	if (thumbView->thumbViewModel->rowCount() > 0) 
	{
		if (thumbView->setCurrentIndexByName(thumbView->recentThumb))
			thumbView->selectCurrentIndex();
		else
		{
			if (thumbView->setCurrentIndexByRow(0))
				thumbView->selectCurrentIndex();
		}
	}
}

void BiManagerWindow::setViewerKeyEventsEnabled(bool enabled)
{
	moveLeftAct->setEnabled(enabled);
	moveRightAct->setEnabled(enabled);
	moveUpAct->setEnabled(enabled);
	moveDownAct->setEnabled(enabled);
}

void BiManagerWindow::updateIndexByViewerImage()
{
	if (thumbView->thumbViewModel->rowCount() > 0) {
		if (thumbView->setCurrentIndexByName(imageView->currentImageFullPath))
			thumbView->selectCurrentIndex();
	}
}

void BiManagerWindow::hideViewer()
{
    if (cliImageLoaded && BimData::exitInsteadOfClose) {
		close();
		return;
	}

	showBusyStatus(true);

	if (isFullScreen())	{
		showNormal();
		if (shouldMaximize)
			showMaximized();
		imageView->setCursorHiding(false);
		imageView->infoLabel->setVisible(false);
	}

	QApplication::processEvents();

    BimData::layoutMode = thumbViewIdx;
	mainLayout->removeWidget(imageView);
	
	imageViewContainer->addWidget(imageView);
	setDocksVisibility(true);
	while (QApplication::overrideCursor()) {
		QApplication::restoreOverrideCursor();
	}

    if (BimData::slideShowActive) {
		slideShow();
	}

	thumbView->setResizeMode(QListView::Fixed);
	thumbView->setVisible(true);
	for (int i = 0; i <= 100 && qApp->hasPendingEvents(); ++i) {
		QApplication::processEvents();
	}

	setThumbviewWindowTitle();

	fsDock->setMaximumHeight(QWIDGETSIZE_MAX);
	bmDock->setMaximumHeight(QWIDGETSIZE_MAX);
	iiDock->setMaximumHeight(QWIDGETSIZE_MAX);
	pvDock->setMaximumHeight(QWIDGETSIZE_MAX);
	fsDock->setMaximumWidth(QWIDGETSIZE_MAX);
	bmDock->setMaximumWidth(QWIDGETSIZE_MAX);
	iiDock->setMaximumWidth(QWIDGETSIZE_MAX);
	pvDock->setMaximumWidth(QWIDGETSIZE_MAX);

	if (!cliFileName.isEmpty()) {
		cliFileName = "";
		if (!shouldMaximize) {
            restoreGeometry(BimData::appSettings->value("Geometry").toByteArray());
		}
        restoreState(BimData::appSettings->value("WindowState").toByteArray());
	}

	if (thumbView->thumbViewModel->rowCount() > 0) {
		if (thumbView->setCurrentIndexByName(imageView->currentImageFullPath))
			thumbView->selectCurrentIndex();
	}
	thumbView->setResizeMode(QListView::Adjust);

	if (needThumbsRefresh) {
		needThumbsRefresh = false;
		refreshThumbs(true);
	} else {
		thumbView->loadVisibleThumbs();
	}

	thumbView->setFocus(Qt::OtherFocusReason);
	showBusyStatus(false);
	setContextMenuPolicy(Qt::DefaultContextMenu);

	updateActions();
}

void BiManagerWindow::goBottom()
{
	thumbView->scrollToBottom();
}

void BiManagerWindow::goTop()
{
	thumbView->scrollToTop();
}

void BiManagerWindow::dropOp(Qt::KeyboardModifiers keyMods, bool dirOp, QString cpMvDirPath)
{
	QApplication::restoreOverrideCursor();
    BimData::copyOp = (keyMods == Qt::ControlModifier);
	QMessageBox msgBox;
	QString destDir;

	if (QObject::sender() == fsTree) {
		destDir = getSelectedPath();
	} else if (QObject::sender() == bookmarks) {
		if (bookmarks->currentItem()) {
			destDir = bookmarks->currentItem()->toolTip(0);
		} else {
			addBookmark(cpMvDirPath);
			return;
		}
	} else {
		// Unknown sender
		return;		
	}
	
	if (!isValidPath(destDir)) {
		msgBox.critical(this, tr("Error"), tr("Can not move or copy images to this folder."));
		selectCurrentViewDir();
		return;
	}
	
	if (destDir == 	thumbView->currentViewDir) {
		msgBox.critical(this, tr("Error"), tr("Destination folder is same as source."));
		return;
	}

	if (dirOp) {
		QString dirOnly = 
			cpMvDirPath.right(cpMvDirPath.size() - cpMvDirPath.lastIndexOf(QDir::separator()) - 1);

		QString question = tr("Move \"%1\" to \"%2\"?").arg(dirOnly).arg(destDir);
		int ret = QMessageBox::question(this, tr("Move folder"), question,
							QMessageBox::Yes | QMessageBox::Cancel, QMessageBox::Cancel);

		if (ret == QMessageBox::Yes) {
			QFile dir(cpMvDirPath);
			bool ok = dir.rename(destDir + QDir::separator() + dirOnly);
			if (!ok) {
				QMessageBox msgBox;
				msgBox.critical(this, tr("Error"), tr("Failed to move folder."));
			}
			setStatus(tr("Folder moved"));
		}
	} else {
		CpMvDialog *cpMvdialog = new CpMvDialog(this);
        BimData::copyCutIdxList = thumbView->selectionModel()->selectedIndexes();
		cpMvdialog->exec(thumbView, destDir, false);
        QString state = QString((BimData::copyOp? tr("Copied") : tr("Moved")) + " " +
							tr("%n image(s)", "", cpMvdialog->nfiles));
		setStatus(state);
		delete(cpMvdialog);
	}

	thumbView->loadVisibleThumbs();
}

void BiManagerWindow::selectCurrentViewDir()
{
	QModelIndex idx = fsTree->fsModel->index(thumbView->currentViewDir); 
	if (idx.isValid())
	{
		fsTree->expand(idx);
		fsTree->setCurrentIndex(idx);
	}
}

void BiManagerWindow::checkDirState(const QModelIndex &, int, int)
{
	if (!initComplete) {
		return;
	}

	if (thumbView->busy)
	{
		thumbView->abort();
	}

	if (!QDir().exists(thumbView->currentViewDir))
	{
		thumbView->currentViewDir = "";
		QTimer::singleShot(0, this, SLOT(reloadThumbsSlot()));
	}
}

void BiManagerWindow::recordHistory(QString dir)
{
	if (!needHistoryRecord)
	{
		needHistoryRecord = true;
		return;
	}

	if (pathHistory.size() && dir == pathHistory.at(currentHistoryIdx))
		return;
		
	pathHistory.insert(++currentHistoryIdx, dir);

	// Need to clear irrelevant items from list
	if (currentHistoryIdx != pathHistory.size() - 1)
	{	
		goFrwdAction->setEnabled(false);
		for (int i = pathHistory.size() - 1; i > currentHistoryIdx ; --i)
		{
			pathHistory.removeAt(i);
		}
	}
}

void BiManagerWindow::reloadThumbsSlot()
{
	if (thumbView->busy || !initComplete)
	{	
		thumbView->abort();
		QTimer::singleShot(0, this, SLOT(reloadThumbsSlot()));
		return;
	}

	if (thumbView->currentViewDir == "")
	{
		thumbView->currentViewDir = getSelectedPath();
		if (thumbView->currentViewDir == "")
			return;
	}

	QDir checkPath(thumbView->currentViewDir);
	if (!checkPath.exists() || !checkPath.isReadable())
	{
		QMessageBox msgBox;
		msgBox.critical(this, tr("Error"), tr("Failed to open folder:") + " " + thumbView->currentViewDir);
		return;
	}

	thumbView->infoView->clear();
    if (BimData::layoutMode == thumbViewIdx && pvDock->isVisible()) {
		QString ImagePath(":/images/no_image.png");
		imageView->loadImage(ImagePath);
	}

	pathBar->setText(thumbView->currentViewDir);
	recordHistory(thumbView->currentViewDir);
	if (currentHistoryIdx > 0)
		goBackAction->setEnabled(true);

    if (BimData::layoutMode == thumbViewIdx)
	{
		setThumbviewWindowTitle();
	}

	thumbView->busy = true;

	if (findDupesAction->isChecked()) {
		thumbView->loadDuplicates();
	} else {
		thumbView->load();
	}
}

void BiManagerWindow::setThumbviewWindowTitle()
{
	if (findDupesAction->isChecked())
        setWindowTitle(tr("Duplicate images in %1").arg(thumbView->currentViewDir) + " - Bim");
	else
        setWindowTitle(thumbView->currentViewDir + " - Bim");
}

void BiManagerWindow::renameDir()
{
	QModelIndexList selectedDirs = fsTree->selectionModel()->selectedRows();
	QFileInfo dirInfo = QFileInfo(fsTree->fsModel->filePath(selectedDirs[0]));

	bool ok;
	QString title = tr("Rename") + " " + dirInfo.completeBaseName();
	QString newDirName = QInputDialog::getText(this, title, 
							tr("New name:"), QLineEdit::Normal, dirInfo.completeBaseName(), &ok);

	if (!ok)
	{
		selectCurrentViewDir();
		return;
	}

	if(newDirName.isEmpty())
	{
		QMessageBox msgBox;
		msgBox.critical(this, tr("Error"), tr("Invalid name entered."));
		selectCurrentViewDir();
		return;
	}

	QFile dir(dirInfo.absoluteFilePath());
	QString newFullPathName = dirInfo.absolutePath() + QDir::separator() + newDirName;
	ok = dir.rename(newFullPathName);
	if (!ok)
	{
		QMessageBox msgBox;
		msgBox.critical(this, tr("Error"), tr("Failed to rename folder."));
		selectCurrentViewDir();
		return;
	}

	if (thumbView->currentViewDir == dirInfo.absoluteFilePath()) 
		fsTree->setCurrentIndex(fsTree->fsModel->index(newFullPathName));
	else
		selectCurrentViewDir();
}

void BiManagerWindow::rename()
{
	if (QApplication::focusWidget() == fsTree) {
		renameDir();
		return;
	}

    if (BimData::layoutMode == imageViewIdx) {

		if (imageView->isNewImage()) 	{
			showNewImageWarning(this);
			return;
		}
	
		if (thumbView->thumbViewModel->rowCount() > 0) {
			if (thumbView->setCurrentIndexByName(imageView->currentImageFullPath))
				thumbView->selectCurrentIndex();
		}
	}

	QString selectedImageFileName = thumbView->getSingleSelectionFilename();
	if (selectedImageFileName.isEmpty()) {
		setStatus(tr("Invalid selection"));
		return;
	}

    if (BimData::slideShowActive)
		slideShow();
	imageView->setCursorHiding(false);

	QString currnetFilePath = selectedImageFileName;
	QFile currentFile(currnetFilePath);
	QString newImageFullPath = thumbView->currentViewDir;
	bool ok;
	QString title = tr("Rename Image");
	QString newImageName = QInputDialog::getText(this,
									title, tr("Enter a new name for \"%1\":")
									.arg(QFileInfo(selectedImageFileName).fileName())
									+ "\t\t\t",
									QLineEdit::Normal,
									QFileInfo(selectedImageFileName).completeBaseName(),
									&ok);

	if (!ok) {
		goto cleanUp;
	}

	if(newImageName.isEmpty()) {
		QMessageBox msgBox;
		msgBox.critical(this, tr("Error"), tr("No name entered."));
		goto cleanUp;
	}

	newImageName += "." + QFileInfo(selectedImageFileName).suffix();
	if (newImageFullPath.right(1) == QDir::separator()) {
		newImageFullPath +=  newImageName;
	} else {
		newImageFullPath += QDir::separator() + newImageName;
	}
	
	ok = currentFile.rename(newImageFullPath);
	if (ok) {
		QModelIndexList indexesList = thumbView->selectionModel()->selectedIndexes();
		thumbView->thumbViewModel->item(
					indexesList.first().row())->setData(newImageFullPath, thumbView->FileNameRole);

        if (BimData::thumbsLayout != ThumbnailView::Squares) {
			thumbView->thumbViewModel->item(
					indexesList.first().row())->setData(newImageName, Qt::DisplayRole);
		}

        if (BimData::layoutMode == imageViewIdx) {
			thumbView->setImageviewWindowTitle();
			imageView->setInfo(newImageName);
			imageView->currentImageFullPath = newImageFullPath;
		}
	} else {
		QMessageBox msgBox;
		msgBox.critical(this, tr("Error"), tr("Failed to rename image."));
	}

cleanUp:
	if (isFullScreen()) {
		imageView->setCursorHiding(true);
	}
}

void BiManagerWindow::deleteDir()
{
	bool ok = true;
	QModelIndexList selectedDirs = fsTree->selectionModel()->selectedRows();
	QString deletePath = fsTree->fsModel->filePath(selectedDirs[0]);
	QModelIndex idxAbove = fsTree->indexAbove(selectedDirs[0]);
	QFileInfo dirInfo = QFileInfo(deletePath);
	QString question = tr("Permanently delete \"%1\" and all of its contents?").arg(dirInfo.completeBaseName());

	QMessageBox msgBox;
	msgBox.setText(question);
	msgBox.setWindowTitle(tr("Delete folder"));
	msgBox.setIcon(QMessageBox::Warning);
	msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::Cancel);
	msgBox.setDefaultButton(QMessageBox::Cancel);
	msgBox.setButtonText(QMessageBox::Yes, tr("Yes"));  
    msgBox.setButtonText(QMessageBox::Cancel, tr("Cancel"));  
	int ret = msgBox.exec();

	if (ret == QMessageBox::Yes)
		ok = removeDirOp(deletePath);
	else
	{
		selectCurrentViewDir();
		return;
	}

	if (!ok)
	{
		QMessageBox msgBox;
		msgBox.critical(this, tr("Error"), tr("Failed to delete folder."));
		selectCurrentViewDir();
	}

	QString state = QString(tr("Removed \"%1\"").arg(deletePath));
	setStatus(state);

	if (thumbView->currentViewDir == deletePath) 
	{
		if (idxAbove.isValid())
			fsTree->setCurrentIndex(idxAbove);
	}
	else
		selectCurrentViewDir();
}

void BiManagerWindow::createSubDirectory()
{
	QModelIndexList selectedDirs = fsTree->selectionModel()->selectedRows();
	QFileInfo dirInfo = QFileInfo(fsTree->fsModel->filePath(selectedDirs[0]));

	bool ok;
	QString newDirName = QInputDialog::getText(this, tr("New Sub folder"), 
							tr("New folder name:"), QLineEdit::Normal, "", &ok);

	if (!ok)
	{
		selectCurrentViewDir();
		return;
	}

	if(newDirName.isEmpty())
	{
		QMessageBox msgBox;
		msgBox.critical(this, tr("Error"), tr("Invalid name entered."));
		selectCurrentViewDir();
		return;
	}

	QDir dir(dirInfo.absoluteFilePath());
	ok = dir.mkdir(dirInfo.absoluteFilePath() + QDir::separator() + newDirName);

	if (!ok)
	{
		QMessageBox msgBox;
		msgBox.critical(this, tr("Error"), tr("Failed to create new folder."));
		selectCurrentViewDir();
		return;
	}

	setStatus(tr("Created \"%1\"").arg(newDirName));
	fsTree->expand(selectedDirs[0]);
}

QString BiManagerWindow::getSelectedPath()
{
	QModelIndexList selectedDirs = fsTree->selectionModel()->selectedRows();
	if (selectedDirs.size() && selectedDirs[0].isValid())
	{
		QFileInfo dirInfo = QFileInfo(fsTree->fsModel->filePath(selectedDirs[0]));
		return dirInfo.absoluteFilePath();
	}
	else
		return "";
}

void BiManagerWindow::wheelEvent(QWheelEvent *event)
{
    if (BimData::layoutMode == imageViewIdx
			|| QApplication::focusWidget() == imageView->scrlArea)
	{	
		if (event->modifiers() == Qt::ControlModifier)
		{
			if (event->delta() < 0)
				zoomOut();				
			else
				zoomIn();
		}
		else if (nextImageAction->isEnabled())
		{
			if (event->delta() < 0)
				loadNextImage();
			else
				loadPrevImage();
		}
	}
}

void BiManagerWindow::showNewImageWarning(QWidget *parent)
{
	QMessageBox msgBox;
	msgBox.warning(parent, tr("Warning"), tr("Cannot perform action with temporary image."));
}

bool BiManagerWindow::removeDirOp(QString dirToDelete)
{
	bool ok = true;
	QDir dir(dirToDelete);

	Q_FOREACH(QFileInfo info, dir.entryInfoList(QDir::NoDotAndDotDot | QDir::System | QDir::Hidden |
					QDir::AllDirs | QDir::Files, QDir::DirsFirst))
	{
		if (info.isDir())
			ok = removeDirOp(info.absoluteFilePath());
		else 
			ok = QFile::remove(info.absoluteFilePath());

		if (!ok)
			return ok;
	}
	ok = dir.rmdir(dirToDelete);

	return ok;
}

void BiManagerWindow::cleanupCropDialog()
{
	setInterfaceEnabled(true);
}

void BiManagerWindow::cleanupScaleDialog()
{
	delete resizeDialog;
	resizeDialog = 0;
	setInterfaceEnabled(true);
}

void BiManagerWindow::cleanupColorsDialog()
{
    BimData::colorsActive = false;
	setInterfaceEnabled(true);
}

void BiManagerWindow::setInterfaceEnabled(bool enable)
{
	// actions 
	colorsAct->setEnabled(enable);
	renameAction->setEnabled(enable);
	cropAct->setEnabled(enable);
	resizeAct->setEnabled(enable);
	closeImageAct->setEnabled(enable);
	nextImageAction->setEnabled(enable);
	prevImageAction->setEnabled(enable);
	firstImageAction->setEnabled(enable);
	lastImageAction->setEnabled(enable);
	randomImageAction->setEnabled(enable);
	slideShowAction->setEnabled(enable);
	copyToAction->setEnabled(enable);
	moveToAction->setEnabled(enable);
	deleteAction->setEnabled(enable);
	settingsAction->setEnabled(enable);
	openAction->setEnabled(enable);

	// other
	thumbView->setEnabled(enable);
	fsTree->setEnabled(enable);
	bookmarks->setEnabled(enable);
	menuBar()->setEnabled(enable);
	editToolBar->setEnabled(enable);
	goToolBar->setEnabled(enable);
	viewToolBar->setEnabled(enable);
	interfaceDisabled = !enable;

	if (enable) {
		if (isFullScreen())
			imageView->setCursorHiding(true);
		updateActions();
	} else {
		imageView->setCursorHiding(false);
	}
}

void BiManagerWindow::addNewBookmark()
{
	addBookmark(getSelectedPath());
}

void BiManagerWindow::addBookmark(QString path)
{
    BimData::bookmarkPaths.insert(path);
	bookmarks->reloadBookmarks();
}

void BiManagerWindow::findDuplicateImages()
{
	refreshThumbs(true);	
}

