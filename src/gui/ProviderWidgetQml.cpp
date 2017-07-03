/*!
 * \copyright Copyright (c) 2016 Governikus GmbH & Co. KG
 */

#include "ProviderWidgetQml.h"
#include "ui_ProviderWidgetQml.h"

#include "ApplicationModel.h"
#include "DpiCalculator.h"
#include "UIPlugInQml.h"

#include <QLoggingCategory>
#include <QQmlContext>
#include <QQmlEngine>
#include <QQuickWidget>

Q_DECLARE_LOGGING_CATEGORY(qml)

using namespace governikus;


ProviderWidgetQml::ProviderWidgetQml(QWidget* pParent)
	: Page(pParent)
	, mUi(new Ui::ProviderWidgetQml())
	, mDpi(DpiCalculator::getDpi())
	, mHistoryModel(&AppSettings::getInstance().getHistorySettings(), &AppSettings::getInstance().getProviderSettings())
	, mProviderModel(&AppSettings::getInstance().getProviderSettings())
	, mQmlExtension()
{
	mUi->setupUi(this);

	connect(mUi->quickWidget, &QQuickWidget::statusChanged, this, &ProviderWidgetQml::onQQuickWidgetStatusChanged);

	QQmlContext* context = mUi->quickWidget->rootContext();
	context->setContextProperty("widgetPlugin", this);
	context->setContextProperty("screenDpi", mDpi);
	context->setContextProperty("qmlExtension", &mQmlExtension);
	context->setContextProperty("historyModel", &mHistoryModel);
	context->setContextProperty("providerModel", &mProviderModel);
	context->setContextProperty("applicationModel", &ApplicationModel::getWidgetInstance());

	mUi->quickWidget->setResizeMode(QQuickWidget::SizeRootObjectToView);

	mUi->quickWidget->engine()->addImportPath(UIPlugInQml::getFile("qml_stationary/").toString());
	mUi->quickWidget->setSource(UIPlugInQml::getFile("qml_stationary/ProviderWidgetQmlPlugin.qml"));
}


ProviderWidgetQml::~ProviderWidgetQml()
{
}


void ProviderWidgetQml::onQQuickWidgetStatusChanged(QQuickWidget::Status pStatus)
{
	qDebug(qml) << "ProviderWidget status:" << pStatus;

	const QList<QQmlError>& errors = mUi->quickWidget->errors();
	for (const QQmlError& error : errors)
	{
		qWarning(qml) << "ProviderWidget QML error:" << error.toString();
	}
}
