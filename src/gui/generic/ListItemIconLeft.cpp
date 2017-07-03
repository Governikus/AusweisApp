#include "ListItemIconLeft.h"

using namespace governikus;

ListItemIconLeft::ListItemIconLeft(QWidget* pParent)
	: QLabel(pParent)
{
}


ListItemIconLeft::ListItemIconLeft(const QString& pText, QWidget* pParent)
	: QLabel(pText, pParent)
{
}


ListItemIconLeft::~ListItemIconLeft()
{
}
