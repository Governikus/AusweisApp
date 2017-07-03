/*!
 * \brief Utility for changing the color of the status bar.
 *
 * \copyright Copyright (c) 2016 Governikus GmbH & Co. KG
 */

#pragma once

#include <QObject>
#include <QString>

namespace governikus
{

class StatusBarUtil
	: public QObject
{
	Q_OBJECT

	private:
		bool catchJavaExceptions() const;

	public:
		Q_INVOKABLE void setStatusBarColor(const QString& pColor);
};

}
