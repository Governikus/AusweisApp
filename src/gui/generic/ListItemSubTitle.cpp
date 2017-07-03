#include "ListItemSubTitle.h"

using namespace governikus;

ListItemSubTitle::ListItemSubTitle(QWidget* pParent)
	: QLabel(pParent)
{
}


ListItemSubTitle::ListItemSubTitle(const QString& pText, QWidget* pParent)
	: QLabel(pText, pParent)
{
}


ListItemSubTitle::~ListItemSubTitle()
{
}
