/*!
 * \copyright Copyright (c) 2016 Governikus GmbH & Co. KG
 */

#include "HistoryWidgetQml.h"
#include "ui_HistoryWidgetQml.h"

#include "DpiCalculator.h"
#include "UIPlugInQml.h"

#include <QLoggingCategory>
#include <QQmlContext>
#include <QQmlEngine>

Q_DECLARE_LOGGING_CATEGORY(qml)

using namespace governikus;


HistoryWidgetQml::HistoryWidgetQml(QWidget* pParent)
	: Page(pParent)
	, mUi(new Ui::HistoryWidgetQml())
	, mDpi(DpiCalculator::getDpi())
	, mHistoryModel(&AppSettings::getInstance().getHistorySettings(), &AppSettings::getInstance().getProviderSettings())
	, mQmlExtension()
{
	mUi->setupUi(this);

	connect(mUi->quickWidget, &QQuickWidget::statusChanged, this, &HistoryWidgetQml::onQQuickWidgetStatusChanged);

	QQmlContext* context = mUi->quickWidget->rootContext();
	context->setContextProperty("widgetPlugin", this);
	context->setContextProperty("screenDpi", mDpi);
	context->setContextProperty("qmlExtension", &mQmlExtension);
	context->setContextProperty("historyModel", &mHistoryModel);

	mUi->quickWidget->setResizeMode(QQuickWidget::SizeRootObjectToView);

	mUi->quickWidget->engine()->addImportPath(UIPlugInQml::getFile("qml_stationary/").toString());
	mUi->quickWidget->setSource(UIPlugInQml::getFile("qml_stationary/HistoryWidgetQmlPlugin.qml"));
}


HistoryWidgetQml::~HistoryWidgetQml()
{
}


void HistoryWidgetQml::onQQuickWidgetStatusChanged(QQuickWidget::Status pStatus)
{
	qDebug(qml) << "HistoryWidget status:" << pStatus;

	const QList<QQmlError>& errors = mUi->quickWidget->errors();
	for (const QQmlError& error : errors)
	{
		qWarning(qml) << "HistoryWidget QML error:" << error.toString();
	}
}


void HistoryWidgetQml::showDetailDialog(const QString& pDetails)
{
	DetailDialog d(this);
	d.setDetails(pDetails);
	d.exec();
}
