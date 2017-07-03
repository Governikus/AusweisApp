#include "ListItemTitle.h"

using namespace governikus;

ListItemTitle::ListItemTitle(QWidget* pParent)
	: QLabel(pParent)
{
}


ListItemTitle::ListItemTitle(const QString& pText, QWidget* pParent)
	: QLabel(pText, pParent)
{
}


ListItemTitle::~ListItemTitle()
{
}
