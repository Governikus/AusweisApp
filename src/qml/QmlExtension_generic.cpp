/*!
 * \copyright Copyright (c) 2016 Governikus GmbH & Co. KG
 */

#include "QmlExtension.h"

#ifndef Q_OS_WINRT
#include "PdfCreator.h"
#endif

#include <QLoggingCategory>
#include <QUrl>

Q_DECLARE_LOGGING_CATEGORY(qml)

using namespace governikus;


void QmlExtension::showSettings(const QString&)
{
	qCWarning(qml) << "NOT IMPLEMENTED YET";
}


void QmlExtension::shareText(const QString&, const QString&)
{
	qCWarning(qml) << "NOT IMPLEMENTED YET";
}


void QmlExtension::showFeedback(const QString&)
{
	qCWarning(qml) << "NOT IMPLEMENTED YET";
}


void QmlExtension::mailLog(const QString&, const QString&, const QString&)
{
	qCWarning(qml) << "NOT IMPLEMENTED YET";
}


bool QmlExtension::exportHistory(const QString& pPdfUrl) const
{
#ifdef Q_OS_WINRT
	return false;

#else
	return PdfExport::exportHistory(QUrl(pPdfUrl).toLocalFile());

#endif
}


#include "moc_QmlExtension.cpp"
