#include "HeaderBox.h"

#include <QPainter>
#include <QStyleOption>

using namespace governikus;

HeaderBox::HeaderBox(QWidget* pParent, Qt::WindowFlags pWindowFlags)
	: QWidget(pParent, pWindowFlags)
{
}


HeaderBox::~HeaderBox()
{
}


void HeaderBox::paintEvent(QPaintEvent*)
{
	QStyleOption opt;
	opt.init(this);
	QPainter p(this);
	style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}
