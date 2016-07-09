#ifndef CROPRUBBERBAND_H
#define CROPRUBBERBAND_H

#include <QtWidgets>

class CropToolWidget : public QWidget
{
public:
  CropToolWidget(QWidget *parent = 0);

private:
  QRubberBand *rubberband;
  void resizeEvent(QResizeEvent *);
};

#endif // CROPRUBBERBAND_H

