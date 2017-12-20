/*
 * \copyright Copyright (c) 2014-2017 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include <QLabel>

namespace governikus
{

class ListItemIconLeft
	: public QLabel
{
	Q_OBJECT

	public:
		ListItemIconLeft(QWidget* pParent = nullptr);
		ListItemIconLeft(const QString& pText, QWidget* pParent = nullptr);
		virtual ~ListItemIconLeft();
};

} /* namespace governikus */
