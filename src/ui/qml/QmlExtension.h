/*!
 * \brief Utility for sharing text.
 *
 * \copyright Copyright (c) 2016-2018 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include <QObject>
#include <QString>

namespace governikus
{

class QmlExtension
	: public QObject
{
	Q_OBJECT

	public:
		Q_INVOKABLE void showSettings(const QString& pAction);
		Q_INVOKABLE void showFeedback(const QString& pMessage);
		Q_INVOKABLE bool exportHistory(const QString& pPdfUrl) const;
		Q_INVOKABLE void keepScreenOn(bool pActive);
		Q_INVOKABLE void openOnlineHelp(const QString& pHelpSectionName);
};

} // namespace governikus
