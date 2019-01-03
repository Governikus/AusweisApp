/*!
 * \brief Utility for changing the color of the status bar.
 *
 * \copyright Copyright (c) 2016-2018 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include <QObject>
#include <QQmlEngine>
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
		StatusBarUtil() = default;
		~StatusBarUtil() override = default;

		Q_INVOKABLE void setStatusBarColor(const QString& pColor);
};

} // namespace governikus
