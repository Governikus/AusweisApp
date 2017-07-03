/*!
 * DescriptionLabel.cpp
 *
 * \copyright Copyright (c) 2014 Governikus GmbH & Co. KG
 */

#include "DescriptionLabel.h"
#include "GuiUtils.h"

using namespace governikus;

DescriptionLabel::DescriptionLabel(QWidget* pParent, Qt::WindowFlags pFlags)
	: QLabel(pParent, pFlags)
{
	GuiUtils::updateFontSize(this);
}


DescriptionLabel::DescriptionLabel(const QString& pText, QWidget* pParent, Qt::WindowFlags pFlags)
	: QLabel(pText, pParent, pFlags)
{
	GuiUtils::updateFontSize(this);
}


DescriptionLabel::~DescriptionLabel()
{
}
