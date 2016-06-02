#include <QGraphicsDropShadowEffect>
#include <math.h>
#include "BimModel.h"
#include "ImageViewer.h"
#include "ThumbnailView.h"

#define CLIPBOARD_IMAGE_NAME		"clipboard.png"
#define ROUND(x) ((int) ((x) + 0.5))
#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define MIN(a, b) ((a) < (b) ? (a) : (b))

ImageViewer::ImageViewer(QWidget *parent) : QWidget(parent)
{
	mainWindow = parent;
	cursorIsHidden = false;
	moveImageLocked = false;
	mirrorLayout = LayNone;
	imageLabel = new QLabel;
	imageLabel->setScaledContents(true);
	isAnimation = false;
	anim = 0;
	setPalette(QPalette(BimData::backgroundColor));

	QVBoxLayout *mainVLayout = new QVBoxLayout();
	mainVLayout->setContentsMargins(0, 0, 0, 0);
	mainVLayout->setSpacing(0);
	mainVLayout->addWidget(imageLabel);

	scrlArea = new QScrollArea;
	scrlArea->setContentsMargins(0, 0, 0, 0);
	scrlArea->setAlignment(Qt::AlignCenter);
	scrlArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	scrlArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	scrlArea->verticalScrollBar()->blockSignals(true);
	scrlArea->horizontalScrollBar()->blockSignals(true);
	scrlArea->setFrameStyle(0);
	scrlArea->setLayout(mainVLayout);
	scrlArea->setWidgetResizable(true);

	QVBoxLayout *scrollLayout = new QVBoxLayout;
	scrollLayout->setContentsMargins(0, 0, 0, 0);
	scrollLayout->setSpacing(0);
	scrollLayout->addWidget(scrlArea);
	this->setLayout(scrollLayout);

	infoLabel = new QLabel(this);
	infoLabel->setVisible(false);
	infoLabel->setMargin(3);
	infoLabel->move(10, 10);
	infoLabel->setStyleSheet("QLabel { background-color : black; color : white; border-radius: 3px} ");

	feedbackLabel = new QLabel(this);
	feedbackLabel->setVisible(false);
	feedbackLabel->setMargin(3);
	feedbackLabel->setStyleSheet("QLabel { background-color : black; color : white; border-radius: 3px} ");

	QGraphicsOpacityEffect *infoEffect = new QGraphicsOpacityEffect;
	infoEffect->	setOpacity(0.5);
	infoLabel->setGraphicsEffect(infoEffect);
	QGraphicsOpacityEffect *feedbackEffect = new QGraphicsOpacityEffect;
	feedbackEffect->setOpacity(0.5);
	feedbackLabel->setGraphicsEffect(feedbackEffect);
	
	mouseMovementTimer = new QTimer(this);
	connect(mouseMovementTimer, SIGNAL(timeout()), this, SLOT(monitorCursorState()));

	BimData::cropLeft = BimData::cropTop = BimData::cropWidth = BimData::cropHeight = 0;
	BimData::cropLeftPercent= BimData::cropTopPercent = BimData::cropWidthPercent = BimData::cropHeightPercent = 0;

	BimData::hueVal = 0;
	BimData::saturationVal = 100;
	BimData::lightnessVal = 100;
	BimData::hueRedChannel = true;
	BimData::hueGreenChannel = true;
	BimData::hueBlueChannel = true;

	BimData::contrastVal = 78;
	BimData::brightVal = 100;

	BimData::dialogLastX = BimData::dialogLastY = 0;

	newImage = false;
	cropBand = 0;
}

static unsigned int getHeightByWidth(int imgWidth, int imgHeight, int newWidth)
{
	float aspect;
	aspect = (float) imgWidth / (float) newWidth;
	return(imgHeight / aspect);
}

static unsigned int getWidthByHeight(int imgHeight, int imgWidth, int newHeight)
{
	float aspect;
	aspect = (float) imgHeight / (float) newHeight;
	return(imgWidth / aspect);
}

static inline int calcZoom(int size)
{
	return size * BimData::imageZoomFactor;
}

void ImageViewer::resizeImage()
{
	static bool busy = false;
	if (busy || (!imageLabel->pixmap() && !anim))
		return;
	busy = true;

	imageLabel->setVisible(false);
	QSize imgSize = isAnimation? anim->currentPixmap().size() : imageLabel->pixmap()->size();

	if (tempDisableResize) {
		imgSize.scale(calcZoom(imgSize.width()), calcZoom(imgSize.height()), Qt::KeepAspectRatio);
	} else {
		switch(BimData::zoomInFlags) {
			case Disable:
				if (imgSize.width() < size().width() && imgSize.height() < size().height())
					imgSize.scale(calcZoom(imgSize.width()), calcZoom(imgSize.height()),
																					Qt::KeepAspectRatio);
				break;
				
			case WidthNHeight:
				if (imgSize.width() < size().width() && imgSize.height() < size().height())
					imgSize.scale(calcZoom(size().width()), calcZoom(size().height()),
																					Qt::KeepAspectRatio);
				break;

			case Width:
				if (imgSize.width() < size().width())
					imgSize.scale(calcZoom(size().width()), 
						calcZoom(getHeightByWidth(imgSize.width(), imgSize.height(), size().width())),
						Qt::KeepAspectRatio);
				break;
				
			case Height:
				if (imgSize.height() < size().height())
					imgSize.scale(calcZoom(getWidthByHeight(imgSize.height(), imgSize.width(),
									size().height())), calcZoom(size().height()), Qt::KeepAspectRatio);
				break;

			case Disprop:
				int newWidth = imgSize.width(), newHeight = imgSize.height();
				if (newWidth < size().width())
					newWidth = size().width();
				if (newHeight < size().height())
					newHeight = size().height();
				imgSize.scale(calcZoom(newWidth), calcZoom(newHeight), Qt::IgnoreAspectRatio);
				break;
		}

		switch(BimData::zoomOutFlags) {
			case Disable:
				if (imgSize.width() >= size().width() || imgSize.height() >= size().height())
					imgSize.scale(calcZoom(imgSize.width()), calcZoom(imgSize.height()),
																					Qt::KeepAspectRatio);
				break;
		
			case WidthNHeight:
				if (imgSize.width() >= size().width() || imgSize.height() >= size().height())
					imgSize.scale(calcZoom(size().width()), calcZoom(size().height()),
																					Qt::KeepAspectRatio);
				break;

			case Width:
				if (imgSize.width() > size().width())
					imgSize.scale(calcZoom(size().width()), 
						calcZoom(getHeightByWidth(imgSize.width(), imgSize.height(), size().width())),
																					Qt::KeepAspectRatio);
				break;
				
			case Height:
				if (imgSize.height() > size().height())
					imgSize.scale(calcZoom(getWidthByHeight(imgSize.height(), imgSize.width(), 
																					size().height())),
						calcZoom(size().height()), Qt::KeepAspectRatio);
			
				break;

			case Disprop:
				int newWidth = imgSize.width(), newHeight = imgSize.height();
				if (newWidth > size().width())
					newWidth = size().width();
				if (newHeight > size().height())
					newHeight = size().height();
				imgSize.scale(calcZoom(newWidth), calcZoom(newHeight), Qt::IgnoreAspectRatio);
				break;
		}
	}

	imageLabel->setFixedSize(imgSize);
	imageLabel->setVisible(true);
	centerImage(imgSize);
	busy = false;
}

void ImageViewer::resizeEvent(QResizeEvent *event)
{
	QWidget::resizeEvent(event);
	resizeImage();
}

void ImageViewer::showEvent(QShowEvent *event)
{
	QWidget::showEvent(event);
	resizeImage();
}

void ImageViewer::centerImage(QSize &imgSize)
{
	int newX = imageLabel->pos().x();
	int newY = imageLabel->pos().y();

	newX = (size().width() - imgSize.width()) / 2;
	newY = (size().height() - imgSize.height()) / 2;

	if (newX != imageLabel->pos().x() || newY != imageLabel->pos().y())
		imageLabel->move(newX, newY);
}

void ImageViewer::rotateByExifRotation(QImage &image, const QString &imageFullPath)
{
	QTransform trans;
	Exiv2::Image::AutoPtr exifImage;

	try {
		exifImage = Exiv2::ImageFactory::open(imageFullPath.toStdString());
		exifImage->readMetadata();
	}
	catch (Exiv2::Error &error) {
		return;
	}

	Exiv2::ExifData &exifData = exifImage->exifData();
	long orientation = 1;

	if (!exifData.empty()) {
		try {
			orientation = exifData["Exif.Image.Orientation"].value().toLong();
		}
		catch (Exiv2::Error &error) {
			return;
		}
	}
	
	switch(orientation) {
		case 2:
			image = image.mirrored(true, false);
			break;
		case 3:
			trans.rotate(180);
			image = image.transformed(trans, Qt::SmoothTransformation);
			break;
		case 4:
			image = image.mirrored(false, true);
			break;
		case 5:
			trans.rotate(90);
			image = image.transformed(trans, Qt::SmoothTransformation);
			image = image.mirrored(true, false);
			break;
		case 6:
			trans.rotate(90);
			image = image.transformed(trans, Qt::SmoothTransformation);
			break;
		case 7:
			trans.rotate(90);
			image = image.transformed(trans, Qt::SmoothTransformation);
			image = image.mirrored(false, true);
			break;
		case 8:
			trans.rotate(270);
			image = image.transformed(trans, Qt::SmoothTransformation);
			break;
	}
}

void ImageViewer::transform()
{
	if (BimData::exifRotationEnabled)	{
		rotateByExifRotation(displayImage, currentImageFullPath);
	}

	if (BimData::rotation) {
		QTransform trans;
		trans.rotate(BimData::rotation);
		displayImage = displayImage.transformed(trans, Qt::SmoothTransformation);
	}

	if (BimData::flipH || BimData::flipV) {
		displayImage = displayImage.mirrored(BimData::flipH, BimData::flipV);
	}

	int cropLeftPercentPixels = 0, cropTopPercentPixels = 0, cropWidthPercentPixels = 0, cropHeightPercentPixels = 0;
	bool croppingOn = false;
	if (BimData::cropLeftPercent || BimData::cropTopPercent
			|| BimData::cropWidthPercent || BimData::cropHeightPercent)	{
		croppingOn = true;
		cropLeftPercentPixels = (displayImage.width() * BimData::cropLeftPercent) / 100;
		cropTopPercentPixels = (displayImage.height() * BimData::cropTopPercent) / 100;
		cropWidthPercentPixels = (displayImage.width() * BimData::cropWidthPercent) / 100;
		cropHeightPercentPixels = (displayImage.height() * BimData::cropHeightPercent) / 100;
	}
	
	if (BimData::cropLeft || BimData::cropTop || BimData::cropWidth || BimData::cropHeight)	{
		displayImage = displayImage.copy(
			BimData::cropLeft + cropLeftPercentPixels,
			BimData::cropTop + cropTopPercentPixels,
			displayImage.width() - BimData::cropLeft - BimData::cropWidth - cropLeftPercentPixels - cropWidthPercentPixels,
			displayImage.height() - BimData::cropTop - BimData::cropHeight - cropTopPercentPixels - cropHeightPercentPixels);
	} else {
		if (croppingOn)	{
			displayImage = displayImage.copy(
					cropLeftPercentPixels,
					cropTopPercentPixels,
					displayImage.width() - cropLeftPercentPixels - cropWidthPercentPixels,
					displayImage.height() - cropTopPercentPixels - cropHeightPercentPixels);
		}
	}
}

void ImageViewer::mirror()
{
	switch(mirrorLayout) {
		case LayDual: {
			mirrorImage = QImage(displayImage.width() * 2, displayImage.height(),
                                                    QImage::Format_ARGB32);
			QPainter painter(&mirrorImage);
			painter.drawImage(0, 0, displayImage);
			painter.drawImage(displayImage.width(), 0, displayImage.mirrored(true, false));
			break;
		}

		case LayTriple: {
			mirrorImage = QImage(displayImage.width() * 3, displayImage.height(),
                                                    QImage::Format_ARGB32);
			QPainter painter(&mirrorImage);
			painter.drawImage(0, 0, displayImage);
			painter.drawImage(displayImage.width(), 0, displayImage.mirrored(true, false));
			painter.drawImage(displayImage.width() * 2, 0, displayImage.mirrored(false, false));
			break;
		}

		case LayQuad: {
			mirrorImage = QImage(displayImage.width() * 2, displayImage.height() * 2,
                                                    QImage::Format_ARGB32);
			QPainter painter(&mirrorImage);
			painter.drawImage(0, 0, displayImage);
			painter.drawImage(displayImage.width(), 0, displayImage.mirrored(true, false));
			painter.drawImage(0, displayImage.height(), displayImage.mirrored(false, true));
			painter.drawImage(displayImage.width(), displayImage.height(),
																	displayImage.mirrored(true, true));
			break;
		}

		case LayVDual: {
			mirrorImage = QImage(displayImage.width(), displayImage.height() * 2,
                                                    QImage::Format_ARGB32);
			QPainter painter(&mirrorImage);
			painter.drawImage(0, 0, displayImage);
			painter.drawImage(0, displayImage.height(), displayImage.mirrored(false, true));
			break;
		}
	}

	displayImage = mirrorImage;
}

static inline int bound0_255(int val)
{
	return ((val > 255)? 255 : (val < 0)? 0 : val);
}

static inline int hslValue(double n1, double n2, double hue)
{
	double value;

	if (hue > 255)
		hue -= 255;
	else if (hue < 0)
		hue += 255;

	if (hue < 42.5)
		value = n1 + (n2 - n1) * (hue / 42.5);
	else if (hue < 127.5)
		value = n2;
	else if (hue < 170)
		value = n1 + (n2 - n1) * ((170 - hue) / 42.5);
	else
		value = n1;

	return ROUND(value * 255.0);
}

void rgbToHsl(int r, int g, int b, unsigned char *hue, unsigned char *sat, unsigned char *light)
{
	double h, s, l;
	int		min, max;
	int		delta;

	if (r > g) {
		max = MAX(r, b);
		min = MIN(g, b);
	} else {
		max = MAX(g, b);
		min = MIN(r, b);
	}

	l = (max + min) / 2.0;

	if (max == min) {
		s = 0.0;
		h = 0.0;
	} else {
		delta = (max - min);

		if (l < 128)
			s = 255 * (double) delta / (double) (max + min);
		else
			s = 255 * (double) delta / (double) (511 - max - min);

		if (r == max)
			h = (g - b) / (double) delta;
		else if (g == max)
			h = 2 + (b - r) / (double) delta;
		else
			h = 4 + (r - g) / (double) delta;

		h = h * 42.5;

		if (h < 0)
			h += 255;
		else if (h > 255)
			h -= 255;
	}

	*hue = ROUND(h);
	*sat = ROUND(s);
	*light = ROUND(l);
}

void hslToRgb(double h, double s, double l,
					unsigned char *red, unsigned char *green, unsigned char *blue)
{
	if (s == 0) {
		/* achromatic case */
		*red = l;
		*green = l;
		*blue = l;
	} else {
		double m1, m2;

		if (l < 128)
			m2 = (l * (255 + s)) / 65025.0;
		else
			m2 = (l + s - (l * s) / 255.0) / 255.0;

		m1 = (l / 127.5) - m2;

		/* chromatic case */
		*red = hslValue(m1, m2, h + 85);
		*green = hslValue(m1, m2, h);
		*blue = hslValue(m1, m2, h - 85);
	}
}

void ImageViewer::colorize()
{
	int y, x;
	unsigned char hr, hg, hb;
	int r, g, b;
	QRgb *line;
	unsigned char h, s, l;
	static unsigned char contrastTransform[256];	
	static unsigned char brightTransform[256];

	if (displayImage.format() == QImage::Format_Indexed8)
		displayImage = displayImage.convertToFormat(QImage::Format_RGB32);

	int i;
	float contrast = ((float)BimData::contrastVal / 100.0);
	float brightness = ((float)BimData::brightVal / 100.0);
	
	for(i = 0; i < 256; ++i) {
		if (i < (int)(128.0f + 128.0f * tan(contrast)) && i > (int)(128.0f - 128.0f * tan(contrast)))
			contrastTransform[i] = (i - 128) / tan(contrast) + 128;
		else if (i >= (int)(128.0f + 128.0f * tan(contrast)))
			contrastTransform[i] = 255;
		else
			contrastTransform[i] = 0;
	}

	for (i = 0; i < 256; ++i) {
		brightTransform[i] = MIN(255,(int)((255.0 * pow(i / 255.0, 1.0 / brightness)) + 0.5));
	}

	for(y = 0; y < displayImage.height(); ++y) {
		line = (QRgb *)displayImage.scanLine(y);
 
		for(x = 0; x < displayImage.width(); ++x) {
			r = qRed(line[x]);
			g = qGreen(line[x]);
			b = qBlue(line[x]);

			r = bound0_255((r * (BimData::redVal + 100)) / 100);
			g = bound0_255((g * (BimData::greenVal + 100)) / 100);
			b = bound0_255((b * (BimData::blueVal + 100)) / 100);

			r = bound0_255(contrastTransform[r]);
			g = bound0_255(contrastTransform[g]);
			b = bound0_255(contrastTransform[b]);

			r = bound0_255(brightTransform[r]);
			g = bound0_255(brightTransform[g]);
			b = bound0_255(brightTransform[b]);

			rgbToHsl(r, g, b, &h, &s, &l);
								
			if (BimData::colorizeEnabled)
				h = BimData::hueVal;
			else
				h += BimData::hueVal;

			s = bound0_255(((s * BimData::saturationVal) / 100));
			l = bound0_255(((l * BimData::lightnessVal) / 100));

			hslToRgb(h, s, l, &hr, &hg, &hb);

			r = BimData::hueRedChannel? hr : qRed(line[x]);
			g = BimData::hueGreenChannel? hg : qGreen(line[x]);
			b = BimData::hueBlueChannel? hb: qBlue(line[x]);

			line[x] = qRgb(r, g, b);
		}
	}
}

void ImageViewer::refresh()
{
	if (isAnimation)
		return;

	if (BimData::scaledWidth)
		displayImage = origImage.scaled(BimData::scaledWidth, BimData::scaledHeight,
											Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
	else
		displayImage = origImage;

	transform();

	if (BimData::colorsActive || BimData::keepTransform)
		colorize();

	if (mirrorLayout)
		mirror();

	displayPixmap = QPixmap::fromImage(displayImage);
	imageLabel->setPixmap(displayPixmap);
	resizeImage();
}

void ImageViewer::reload()
{
	if (BimData::enableImageInfoFS) {
		if (currentImageFullPath.isEmpty())
			setInfo("Clipboard");
		else
			setInfo(QFileInfo(currentImageFullPath).fileName());
	}

	if (!BimData::keepTransform) {
		BimData::cropLeftPercent = BimData::cropTopPercent = BimData::cropWidthPercent = BimData::cropHeightPercent = 0;
		BimData::rotation = 0;
		BimData::flipH = BimData::flipV = false;
	}
	BimData::scaledWidth = BimData::scaledHeight = 0;
	BimData::cropLeft = BimData::cropTop = BimData::cropWidth = BimData::cropHeight = 0;

	if (newImage || currentImageFullPath.isEmpty())	{
		newImage = true;
		currentImageFullPath = CLIPBOARD_IMAGE_NAME;
		origImage.load(":/images/no_image.png");
		displayImage = origImage;
		displayPixmap = QPixmap::fromImage(displayImage);
		imageLabel->setPixmap(displayPixmap);
		pasteImage();
        mainWindow->setWindowTitle(tr("Clipboard") + " - Bim");
		isAnimation = false;
		return;
	}

	imageReader.setFileName(currentImageFullPath);

	isAnimation = BimData::enableAnimations? imageReader.supportsAnimation() : false;
	if (isAnimation) {
		if (anim)
			delete anim;
		anim = new QMovie(currentImageFullPath);
		imageLabel->setMovie(anim);
		anim->start();
	} else {
		if (imageReader.size().isValid()) {
			origImage.load(currentImageFullPath);
			displayImage = origImage;
			transform();
			if (BimData::colorsActive || BimData::keepTransform)
				colorize();
			if (mirrorLayout)
				mirror();
			displayPixmap = QPixmap::fromImage(displayImage);
		} else {
			displayPixmap = QIcon::fromTheme("image-missing", 
										QIcon(":/images/error_image.png")).pixmap(128, 128);
		}

		imageLabel->setPixmap(displayPixmap);
	}

	resizeImage();
}

void ImageViewer::setInfo(QString infoString)
{
	infoLabel->setText(infoString);
	infoLabel->adjustSize();
}

void ImageViewer::unsetFeedback()
{
	feedbackLabel->setText("");
	feedbackLabel->setVisible(false);
}

void ImageViewer::setFeedback(QString feedbackString)
{
	feedbackLabel->setText(feedbackString);
	feedbackLabel->setVisible(true);

	int margin = infoLabel->isVisible()? (infoLabel->height() + 15) : 10;
	feedbackLabel->move(10, margin);
	
	feedbackLabel->adjustSize();
	QTimer::singleShot(3000, this, SLOT(unsetFeedback()));
}

void ImageViewer::loadImage(QString imageFileName)
{
	newImage = false;
	tempDisableResize = false;
	currentImageFullPath = imageFileName;

	if (!BimData::keepZoomFactor)
		BimData::imageZoomFactor = 1.0;

	QApplication::processEvents();
	reload();
}

void ImageViewer::monitorCursorState()
{
	static QPoint lastPos;
	
	if (QCursor::pos() != lastPos)
	{
		lastPos = QCursor::pos();
		if (cursorIsHidden) 
		{
			QApplication::restoreOverrideCursor();
			cursorIsHidden = false;
		}
	}
	else
	{
		if (!cursorIsHidden)
		{
			QApplication::setOverrideCursor(Qt::BlankCursor);
			cursorIsHidden = true;
		}
	}
}

void ImageViewer::setCursorHiding(bool hide)
{
	if (hide) {
		mouseMovementTimer->start(500);
	} else {
		mouseMovementTimer->stop();
		if (cursorIsHidden) {
			QApplication::restoreOverrideCursor();
			cursorIsHidden = false;
		}
	}
}

void ImageViewer::mouseDoubleClickEvent(QMouseEvent *event)
{
	QWidget::mouseDoubleClickEvent(event);
	while (QApplication::overrideCursor())
		QApplication::restoreOverrideCursor();
}

void ImageViewer::mousePressEvent(QMouseEvent *event)
{
	if (event->button() == Qt::LeftButton) {

		if (event->modifiers() == Qt::ControlModifier) {
			cropOrigin = event->pos();
		    if (!cropBand)
		        cropBand = new CropToolWidget(this);
	        cropBand->show();
			cropBand->setGeometry(QRect(cropOrigin , event->pos()).normalized());
		} else {
			if (cropBand) {
				cropBand->hide();
			}
		}

		setMouseMoveData(true, event->x(), event->y());
		QApplication::setOverrideCursor(Qt::ClosedHandCursor);
		event->accept();
	}
	QWidget::mousePressEvent(event);
}

void ImageViewer::mouseReleaseEvent(QMouseEvent *event)
{
	if (event->button() == Qt::LeftButton) {
		setMouseMoveData(false, 0, 0);
		while (QApplication::overrideCursor())
			QApplication::restoreOverrideCursor();
	}

	QWidget::mouseReleaseEvent(event);
}

void ImageViewer::cropToSelection()
{
	if (cropBand && cropBand->isVisible()) {

		QPoint bandTL = mapToGlobal(cropBand->geometry().topLeft());
		QPoint bandBR = mapToGlobal(cropBand->geometry().bottomRight());

		bandTL = imageLabel->mapFromGlobal(bandTL);
		bandBR = imageLabel->mapFromGlobal(bandBR);
		
		double scaledX = imageLabel->rect().width();
		double scaledY = imageLabel->rect().height();
		scaledX = displayPixmap.width() / scaledX;
		scaledY = displayPixmap.height() / scaledY;

		bandTL.setX(int(bandTL.x() * scaledX));
		bandTL.setY(int(bandTL.y() * scaledY));
		bandBR.setX(int(bandBR.x() * scaledX));
		bandBR.setY(int(bandBR.y() * scaledY));

		int cropLeft = bandTL.x();
		int cropTop = bandTL.y();
		int cropWidth = displayPixmap.width() - bandBR.x();
		int cropHeight = displayPixmap.height() -  bandBR.y();

		if (cropLeft > 0)
			BimData::cropLeft += cropLeft;
		if (cropTop > 0)
			BimData::cropTop += cropTop;
		if (cropWidth > 0)
			BimData::cropWidth += cropWidth;
		if (cropHeight > 0)
			BimData::cropHeight += cropHeight;

		cropBand->hide();
		refresh();
	} else {
		QMessageBox msgBox;
		msgBox.warning(this, tr("No selection"), tr("Hold down the Ctrl key and select a region using the mouse."));
	}
}

void ImageViewer::setMouseMoveData(bool lockMove, int lMouseX, int lMouseY)
{
	moveImageLocked = lockMove;
	mouseX = lMouseX;
	mouseY = lMouseY;
	layoutX = imageLabel->pos().x();
	layoutY = imageLabel->pos().y();
}

void ImageViewer::mouseMoveEvent(QMouseEvent *event)
{
	if (event->modifiers() == Qt::ControlModifier) {
		if (cropBand && cropBand->isVisible()) {
			cropBand->setGeometry(QRect(cropOrigin , event->pos()).normalized());
		}
	} else {
		if (moveImageLocked) 
		{
			int newX = layoutX + (event->pos().x() - mouseX);
			int newY = layoutY + (event->pos().y() - mouseY);
			bool needToMove = false;

			if (imageLabel->size().width() > size().width()) 	{
				if (newX > 0)
					newX = 0;
				else if (newX < (size().width() - imageLabel->size().width()))
					newX = (size().width() - imageLabel->size().width());
				needToMove = true;
			}
			else
				newX = layoutX;

			if (imageLabel->size().height() > size().height()) {
				if (newY > 0)
					newY = 0;
				else if (newY < (size().height() - imageLabel->size().height()))
					newY = (size().height() - imageLabel->size().height());
				needToMove = true;
			}
			else
				newY = layoutY;

			if (needToMove)
				imageLabel->move(newX, newY);
		}
	}
}

void ImageViewer::keyMoveEvent(int direction)
{
	int newX = layoutX = imageLabel->pos().x();
	int newY = layoutY = imageLabel->pos().y();
	bool needToMove = false;

	switch (direction)
	{
		case MoveLeft:
			newX += 50;
			break;
		case MoveRight:
			newX -= 50;
			break;
		case MoveUp:
			newY += 50;
			break;
		case MoveDown:
			newY -= 50;
			break;
	}

	if (imageLabel->size().width() > size().width())
	{
		if (newX > 0)
			newX = 0;
		else if (newX < (size().width() - imageLabel->size().width()))
			newX = (size().width() - imageLabel->size().width());
		needToMove = true;
	}
	else
		newX = layoutX;

	if (imageLabel->size().height() > size().height())
	{
		if (newY > 0)
			newY = 0;
		else if (newY < (size().height() - imageLabel->size().height()))
			newY = (size().height() - imageLabel->size().height());
		needToMove = true;
	}
	else
		newY = layoutY;

	if (needToMove)
	{
		int i;

		switch (direction)
		{
			case MoveLeft:
				for (i = imageLabel->pos().x(); i <= newX; ++i)
					imageLabel->move(newX, newY);
				break;
			case MoveRight:
				for (i = imageLabel->pos().x(); i >= newX; --i)
					imageLabel->move(newX, newY);
				break;
			case MoveUp:
				for (i = imageLabel->pos().y(); i <= newY; ++i)
					imageLabel->move(newX, newY);
				break;
			case MoveDown:
				for (i = imageLabel->pos().y(); i >= newY; --i)
					imageLabel->move(newX, newY);
				break;
		}
	}
}

void ImageViewer::saveImage()
{
	Exiv2::Image::AutoPtr image;
	bool exifError = false;

	if (newImage)
	{
		saveImageAs();
		return;
	}

	setFeedback(tr("Saving..."));

	try
	{
		image = Exiv2::ImageFactory::open(currentImageFullPath.toStdString());
		image->readMetadata();
	}
	catch (Exiv2::Error &error)
	{
		exifError = true;
	}

	QImageReader imgReader(currentImageFullPath);
	if (!displayPixmap.save(currentImageFullPath, imgReader.format().toUpper(),
																			BimData::defaultSaveQuality))
	{
		QMessageBox msgBox;
		msgBox.critical(this, tr("Error"), tr("Failed to save image."));
		return;
	}

	if (!exifError)
	{
		try
		{
			image->writeMetadata();
		}
		catch (Exiv2::Error &error)
		{
			QMessageBox msgBox;
			msgBox.critical(this, tr("Error"), tr("Failed to save Exif metadata."));
		}
	}

	reload();
	setFeedback(tr("Image saved."));
}

void ImageViewer::saveImageAs()
{
	Exiv2::Image::AutoPtr exifImage;
	Exiv2::Image::AutoPtr newExifImage;
	bool exifError = false;

	setCursorHiding(false);

	QString fileName = QFileDialog::getSaveFileName(this,
		tr("Save image as"),
		currentImageFullPath,
		tr("Images") + " (*.jpg *.jpeg *.jpe *.png *.bmp *.tiff *.tif *.ppm *.xbm *.xpm)");
		
	if (!fileName.isEmpty())
	{
		try
		{
			exifImage = Exiv2::ImageFactory::open(currentImageFullPath.toStdString());
			exifImage->readMetadata();
		}
		catch (Exiv2::Error &error)
		{
			exifError = true;
		}

	
		if (!displayPixmap.save(fileName, 0, BimData::defaultSaveQuality))
		{
			QMessageBox msgBox;
			msgBox.critical(this, tr("Error"), tr("Failed to save image."));
		}
		else
		{
			if (!exifError)
			{
				try
				{
					newExifImage = Exiv2::ImageFactory::open(fileName.toStdString());
					newExifImage->setMetadata(*exifImage);
					newExifImage->writeMetadata();
				}
				catch (Exiv2::Error &error)
				{
					exifError = true;
				}
			}
		
			setFeedback(tr("Image saved."));
		}
	}
	if (mainWindow->isFullScreen()) {
		setCursorHiding(true);
	}
}

void ImageViewer::contextMenuEvent(QContextMenuEvent *)
{
	while (QApplication::overrideCursor())
		QApplication::restoreOverrideCursor();

	ImagePopUpMenu->exec(QCursor::pos());
}

int ImageViewer::getImageWidthPreCropped()
{
	return origImage.width();
}

int ImageViewer::getImageHeightPreCropped()
{
	return origImage.height();
}

bool ImageViewer::isNewImage()
{
	return newImage;
}

void ImageViewer::copyImage()
{
	QApplication::clipboard()->setImage(displayImage);
}

void ImageViewer::pasteImage()
{
	if (isAnimation)
		return;

	if (!QApplication::clipboard()->image().isNull())
	{
		origImage = QApplication::clipboard()->image();
		refresh();
	}
}

