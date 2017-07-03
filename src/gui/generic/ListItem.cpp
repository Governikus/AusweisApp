#include "ListItem.h"

#include <QPainter>
#include <QStyleOption>

using namespace governikus;

ListItem::ListItem(QWidget* pParent, Qt::WindowFlags pWindowFlags)
	: QWidget(pParent, pWindowFlags)
{
}


ListItem::~ListItem()
{
}


void ListItem::paintEvent(QPaintEvent*)
{
	QStyleOption opt;
	opt.init(this);
	QPainter p(this);
	style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}
