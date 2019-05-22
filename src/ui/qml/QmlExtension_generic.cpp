/*!
 * \copyright Copyright (c) 2016-2019 Governikus GmbH & Co. KG, Germany
 */

#include "QmlExtension.h"

#ifndef Q_OS_WINRT
#include "PdfExporter.h"
#endif

#include "HelpAction.h"

#include <QLoggingCategory>
#include <QUrl>

Q_DECLARE_LOGGING_CATEGORY(qml)

using namespace governikus;


void QmlExtension::showSettings(const QString&)
{
	qCWarning(qml) << "NOT IMPLEMENTED";
}


void QmlExtension::showFeedback(const QString&)
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


void QmlExtension::openOnlineHelp(const QString& pHelpSectionName)
{
	HelpAction::openContextHelp(pHelpSectionName);
}


#include "moc_QmlExtension.cpp"
