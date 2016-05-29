#ifndef DIALOGS_H
#define DIALOGS_H

#include <QtWidgets>
#include "ThumbnailView.h"
#include "ImageViewer.h"

int cpMvFile(bool isCopy, QString &srcFile, QString &srcPath, QString &dstPath, QString &dstDir);

class CpMvDialog : public QDialog
{
    Q_OBJECT

public slots:
	void abort();

public:
    CpMvDialog(QWidget *parent);
    void exec(ThumbnailView *thumbView, QString &destDir, bool pasteInCurrDir);
	int nfiles;

private:
	QLabel *opLabel;
	QPushButton *cancelButton;
	QFileInfo *dirInfo;
	bool abortOp;
};

class ShortcutsTableView : public QTableView
{
	Q_OBJECT

public:
	ShortcutsTableView();
	void addRow(QString action, QString description, QString shortcut);

public slots:
	void showShortcutsTableMenu(QPoint pt);
	void clearShortcut();

protected:
	void keyPressEvent(QKeyEvent *e);

private:
	QStandardItemModel *keysModel;
	QModelIndex selectedEntry;
	QMenu *shortcutsMenu;
	QAction *clearAction;
};

class SettingsDialog : public QDialog
{
    Q_OBJECT

public:
	static int const nZoomRadios = 5;
    SettingsDialog(QWidget *parent);

private slots:
	void pickColor();
	void pickThumbsColor();
	void pickThumbsTextColor();
	void pickStartupDir();
	void pickBgImage();

public slots:
	void abort();
	void saveSettings();

private:
	QRadioButton *fitLargeRadios[nZoomRadios];
	QRadioButton *fitSmallRadios[nZoomRadios];
	QCheckBox *compactLayoutCb;
	QToolButton *backgroundColorButton;
    QToolButton *colThumbButton;
    QToolButton *colThumbTextButton;
	QSpinBox *thumbSpacingSpin;
	QSpinBox *thumbPagesSpin;
	QSpinBox *saveQualitySpin;
	QColor bgColor;
	QColor thumbBgColor;
	QColor thumbTextColor;
	QCheckBox *exitCliCb;
	QCheckBox *wrapListCb;
	QCheckBox *enableAnimCb;
	QCheckBox *enableExifCb;
	QCheckBox *enableThumbExifCb;
	QCheckBox *imageInfoCb;
	QCheckBox *noSmallThumbCb;
	QCheckBox *reverseMouseCb;
	QCheckBox *deleteConfirmCb;
	QSpinBox *slideDelaySpin;
	QCheckBox *slideRandomCb;
	QRadioButton *startupDirRadios[3];
	QLineEdit *startupDirEdit;
	QLineEdit *thumbsBackImageEdit;

	void setButtonBgColor(QColor &color, QToolButton *button);
};

class CropDialog : public QDialog
{
    Q_OBJECT

public:
    CropDialog(QWidget *parent, ImageViewer *imageView);

public slots:
	void ok();
	void reset();
	void applyCrop(int);

private:
	QSpinBox *topSpin;
	QSpinBox *bottomSpin;
	QSpinBox *leftSpin;
	QSpinBox *rightSpin;
    ImageViewer *imageView;
};

class ResizeDialog : public QDialog
{
    Q_OBJECT

public:
    ResizeDialog(QWidget *parent, ImageViewer *imageView);

public slots:
	void ok();
	void abort();
	void setAspectLock();
	void setUnits();
	void adjustSizes();
	
private:
	int width;
	int height;
	int lastWidth;
	int lastHeight;
	bool aspectLocked;
	bool pixelUnits;
	int newWidth;
	int newHeight;
	
	QSpinBox *widthSpin;
	QSpinBox *heightSpin;
	QRadioButton *pixelsRadio;
	QRadioButton *percentRadio;
	QLabel *newSizePixelsLab;
    ImageViewer *imageView;
};

class ColorsDialog : public QDialog
{
    Q_OBJECT

public:
    ColorsDialog(QWidget *parent, ImageViewer *imageView);

public slots:
	void ok();
	void reset();
	void enableColorize(int state);
	void setRedChannel();
	void setGreenChannel();
	void setBlueChannel();
	void applyColors(int value);

private:
    ImageViewer *imageView;
	QSlider *hueSlide;
	QCheckBox *colorizeCb;
	QSlider *saturationSlide;
	QSlider *lightnessSlide;
	QCheckBox *redB;
	QCheckBox *greenB;
	QCheckBox *blueB;
	QSlider *brightSlide;
	QSlider *contrastSlide;
	QSlider *redSlide;
	QSlider *greenSlide;
	QSlider *blueSlide;
};

class AppMgmtDialog : public QDialog
{
    Q_OBJECT

public:
    AppMgmtDialog(QWidget *parent);

public slots:
	void ok();

private slots:
	void add();
	void remove();
	void entry();

private:
	QTableView *appsTable;
	QStandardItemModel *appsTableModel;

	void addTableModelItem(QStandardItemModel *model, QString &key, QString &val);
};

class CopyMoveToDialog : public QDialog
{
    Q_OBJECT

public:
    CopyMoveToDialog(QWidget *parent, QString thumbsPath, bool move);
	QString selectedPath;
	bool copyOp;

private slots:
	void copyOrMove();
	void justClose();
	void add();
	void remove();
	void selection(const QItemSelection&, const QItemSelection&);
	void pathDoubleClick(const QModelIndex &idx);

private:
	QTableView *pathsTable;
	QStandardItemModel *pathsTableModel;
	QString currentPath;
	QLabel *destinationLab;

	void savePaths();
};

#endif // DIALOGS_H

