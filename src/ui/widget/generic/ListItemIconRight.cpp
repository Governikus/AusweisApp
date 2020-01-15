/*
 * \copyright Copyright (c) 2014-2020 Governikus GmbH & Co. KG, Germany
 */

#include "ListItemIconRight.h"

using namespace governikus;

ListItemIconRight::ListItemIconRight(QWidget* pParent)
	: QLabel(pParent)
{
}


ListItemIconRight::ListItemIconRight(const QString& pText, QWidget* pParent)
	: QLabel(pText, pParent)
{
}


ListItemIconRight::~ListItemIconRight()
{
}
