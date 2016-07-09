#include "CropToolWidget.h"

CropToolWidget::CropToolWidget(QWidget *parent) : QWidget(parent) {

	setWindowFlags(Qt::SubWindow);

	QVBoxLayout* mainLayout = new QVBoxLayout(this);
	mainLayout->setContentsMargins(0, 0, 0, 0);

	QHBoxLayout* topLayout = new QHBoxLayout();
	topLayout->setContentsMargins(0, 0, 0, 0);
	QHBoxLayout* bottomLayout = new QHBoxLayout();
	bottomLayout->setContentsMargins(0, 0, 0, 0);
	
	QSizeGrip* grip1 = new QSizeGrip(this);
	QSizeGrip* grip2 = new QSizeGrip(this);
	QSizeGrip* grip3 = new QSizeGrip(this);
	QSizeGrip* grip4 = new QSizeGrip(this);

    grip1->setStyleSheet("background-color: rgba(0, 0, 0, 0%)");
	grip2->setStyleSheet("background-color: rgba(0, 0, 0, 0%)");
	grip3->setStyleSheet("background-color: rgba(0, 0, 0, 0%)");
	grip4->setStyleSheet("background-color: rgba(0, 0, 0, 0%)");

	topLayout->addWidget(grip1, 0, Qt::AlignTop | Qt::AlignLeft);
	topLayout->addWidget(grip2, 1, Qt::AlignTop | Qt::AlignRight);
	bottomLayout->addWidget(grip3, 0, Qt::AlignBottom | Qt::AlignLeft);
	bottomLayout->addWidget(grip4, 1, Qt::AlignBottom | Qt::AlignRight);

	mainLayout->addLayout(topLayout);
	mainLayout->addLayout(bottomLayout);

	rubberband = new QRubberBand(QRubberBand::Rectangle, this);
	rubberband->setStyleSheet("background-color: rgb(255, 255, 255)");
	rubberband->show();
}

void CropToolWidget::resizeEvent(QResizeEvent *)
{
	rubberband->resize(size());
}

