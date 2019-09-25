#ifndef IMAGEDISPLAY_H
#define IMAGEDISPLAY_H

#include <QWidget>

class ImageDisplay : public QWidget
{
	Q_OBJECT
public:
	explicit ImageDisplay(QWidget *parent = nullptr) :
		QWidget(parent),
		image(nullptr) {}

	void setImage(const QImage* image);

	virtual QSize sizeHint() const override;

protected:
	void paintEvent(QPaintEvent* event) override;

private:
	const QImage* image;
};

#endif // IMAGEDISPLAY_H
