/*
 * \copyright Copyright (c) 2014-2019 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include <QWidget>


namespace governikus
{

class ListItem
	: public QWidget
{
	Q_OBJECT

	public:
		ListItem(QWidget* pParent = nullptr, Qt::WindowFlags pWindowFlags = 0);
		virtual ~ListItem() override;

		void paintEvent(QPaintEvent*) override;
};

} // namespace governikus
