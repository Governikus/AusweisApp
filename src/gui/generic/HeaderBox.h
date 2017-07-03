/*!
 * \copyright Copyright (c) 2014 Governikus GmbH & Co. KG
 */

#pragma once

#include <QWidget>

namespace governikus
{

class HeaderBox
	: public QWidget
{
	Q_OBJECT

	public:
		explicit HeaderBox(QWidget* pParent = nullptr, Qt::WindowFlags pWindowFlags = 0);
		virtual ~HeaderBox();

		void paintEvent(QPaintEvent*) override;
};

} /* namespace governikus */
