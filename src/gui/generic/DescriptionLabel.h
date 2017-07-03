/*!
 * DescriptionLabel.h
 *
 * \brief QLabel subclass used for description texts.
 *
 * Its only purpose is to serve as a class name for styling.
 *
 * \copyright Copyright (c) 2014 Governikus GmbH & Co. KG
 */

#pragma once

#include <QLabel>

namespace governikus
{

class DescriptionLabel
	: public QLabel
{
	Q_OBJECT

	public:
		DescriptionLabel(QWidget* pParent = nullptr, Qt::WindowFlags pFlags = 0);
		DescriptionLabel(const QString& pText, QWidget* pParent = nullptr, Qt::WindowFlags pFlags = 0);
		virtual ~DescriptionLabel();
};

} /* namespace governikus */
