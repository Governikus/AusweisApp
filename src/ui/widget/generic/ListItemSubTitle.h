/*
 * \copyright Copyright (c) 2014-2020 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include <QLabel>

namespace governikus
{

class ListItemSubTitle
	: public QLabel
{
	Q_OBJECT

	public:
		ListItemSubTitle(QWidget* pParent = nullptr);
		ListItemSubTitle(const QString& pText, QWidget* pParent = nullptr);
		virtual ~ListItemSubTitle();
};

} // namespace governikus
