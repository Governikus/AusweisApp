/*
 * \copyright Copyright (c) 2014-2017 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include <QLabel>

namespace governikus
{

class ListItemIconRight
	: public QLabel
{
	Q_OBJECT

	public:
		ListItemIconRight(QWidget* pParent = nullptr);
		ListItemIconRight(const QString& pText, QWidget* pParent = nullptr);
		virtual ~ListItemIconRight();
};

} /* namespace governikus */
