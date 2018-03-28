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
		Q_INVOKABLE void shareText(const QString& pText, const QString& pChooserTitle);
		Q_INVOKABLE void showFeedback(const QString& pMessage);
		Q_INVOKABLE bool exportHistory(const QString& pPdfUrl) const;
		Q_INVOKABLE void mailLog(const QString& pEmail, const QString& pSubject, const QString& pMsg);
		Q_INVOKABLE void keepScreenOn(bool pActive);
};

}
