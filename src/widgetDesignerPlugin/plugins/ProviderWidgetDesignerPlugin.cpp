/*!
 * \copyright Copyright (c) 2017 Governikus GmbH & Co. KG, Germany
 */

#include "ProviderWidgetDesignerPlugin.h"

#include "ProviderWidget.h"

using namespace governikus;


ProviderWidgetDesignerPlugin::ProviderWidgetDesignerPlugin(QObject* pParent)
	: QObject(pParent)
	, mInitialized(false)
{

}


QWidget* ProviderWidgetDesignerPlugin::createWidget(QWidget* pParent)
{
	return new ProviderWidget(pParent);
}


QString ProviderWidgetDesignerPlugin::domXml() const
{
	return QStringLiteral(
			"<ui language=\"c++\">\n"
			" <widget class=\"governikus::ProviderWidget\" name=\"providerWidget\">\n"
			" </widget>\n"
			"</ui>\n");
}


QString ProviderWidgetDesignerPlugin::includeFile() const
{
	return QStringLiteral("ProviderWidget.h");
}


QString ProviderWidgetDesignerPlugin::name() const
{
	return QStringLiteral("governikus::ProviderWidget");
}


QString ProviderWidgetDesignerPlugin::group() const
{
	return QStringLiteral("Governikus");
}


QString ProviderWidgetDesignerPlugin::toolTip() const
{
	return QString();
}


QString ProviderWidgetDesignerPlugin::whatsThis() const
{
	return QString();
}


bool ProviderWidgetDesignerPlugin::isContainer() const
{
	return false;
}


bool ProviderWidgetDesignerPlugin::isInitialized() const
{
	return mInitialized;
}


QIcon ProviderWidgetDesignerPlugin::icon() const
{
	return QIcon();
}


void ProviderWidgetDesignerPlugin::initialize(QDesignerFormEditorInterface*)
{
	if (mInitialized)
	{
		return;
	}

	mInitialized = true;
}
