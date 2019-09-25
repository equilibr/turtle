#include "ImageDisplay.h"

#include <QPainter>

void ImageDisplay::setImage(const QImage * image)
{
  this->image = image;
//  repaint();
  update();
}

QSize ImageDisplay::sizeHint() const
{
	if (!image)
		return QWidget::sizeHint();

	return image->size();
}

void ImageDisplay::paintEvent(QPaintEvent*)
{
	if (!image)
		return;

	//Rescale to fit
	QRectF box(image->rect());

	const double xScale = box.width() / rect().width();
	const double yScale = box.height() / rect().height();
	const double scale = qMax(xScale, yScale);

	box.setWidth(box.width() / scale);
	box.setHeight(box.height() / scale);

	box.moveLeft( rect().width() / 2 - box.width() / 2 );
	box.moveTop( rect().height() / 2 - box.height() / 2 );

	QPainter painter(this);
	painter.drawImage(box, *image);
}
