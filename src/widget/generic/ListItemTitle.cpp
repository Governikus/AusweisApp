/*
 * \copyright Copyright (c) 2014-2017 Governikus GmbH & Co. KG, Germany
 */

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
