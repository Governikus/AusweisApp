/*!
 * \copyright Copyright (c) 2016-2018 Governikus GmbH & Co. KG, Germany
 */

#include "QmlExtension.h"

#ifndef Q_OS_WINRT
#include "PdfExporter.h"
#endif

#include <QLoggingCategory>
#include <QUrl>

Q_DECLARE_LOGGING_CATEGORY(qml)

using namespace governikus;


void QmlExtension::showSettings(const QString&)
{
	qCWarning(qml) << "NOT IMPLEMENTED";
}


void QmlExtension::shareText(const QString&, const QString&)
{
	qCWarning(qml) << "NOT IMPLEMENTED";
}


void QmlExtension::showFeedback(const QString&)
{
	qCWarning(qml) << "NOT IMPLEMENTED";
}


void QmlExtension::mailLog(const QString&, const QString&, const QString&)
{
	qCWarning(qml) << "NOT IMPLEMENTED";
}


bool QmlExtension::exportHistory(const QString& pPdfUrl) const
{
#ifdef Q_OS_WINRT
	return false;

#else
	PdfExporter exporter(QUrl(pPdfUrl).toLocalFile());
	return exporter.exportHistory();

#endif
}


void QmlExtension::keepScreenOn(bool)
{
	qCWarning(qml) << "NOT IMPLEMENTED";
}


#include "moc_QmlExtension.cpp"
