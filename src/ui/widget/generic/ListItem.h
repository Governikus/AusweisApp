/*
 * \copyright Copyright (c) 2014-2020 Governikus GmbH & Co. KG, Germany
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
		ListItem(QWidget* pParent = nullptr);
		virtual ~ListItem() override;

		void paintEvent(QPaintEvent*) override;
};

} // namespace governikus
