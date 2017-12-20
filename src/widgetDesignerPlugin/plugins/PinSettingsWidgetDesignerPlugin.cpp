/*!
 * \copyright Copyright (c) 2017 Governikus GmbH & Co. KG, Germany
 */

#include "PinSettingsWidgetDesignerPlugin.h"

#include "PinSettingsWidget.h"

using namespace governikus;


PinSettingsWidgetDesignerPlugin::PinSettingsWidgetDesignerPlugin(QObject* pParent)
	: QObject(pParent)
	, mInitialized(false)
{

}


QWidget* PinSettingsWidgetDesignerPlugin::createWidget(QWidget* pParent)
{
	return new PinSettingsWidget(pParent);
}


QString PinSettingsWidgetDesignerPlugin::domXml() const
{
	return QStringLiteral(
			"<ui language=\"c++\">\n"
			" <widget class=\"governikus::PinSettingsWidget\" name=\"pinSettingsWidget\">\n"
			" </widget>\n"
			"</ui>\n");
}


QString PinSettingsWidgetDesignerPlugin::includeFile() const
{
	return QStringLiteral("PinSettingsWidget.h");
}


QString PinSettingsWidgetDesignerPlugin::name() const
{
	return QStringLiteral("governikus::PinSettingsWidget");
}


QString PinSettingsWidgetDesignerPlugin::group() const
{
	return QStringLiteral("Governikus");
}


QString PinSettingsWidgetDesignerPlugin::toolTip() const
{
	return QString();
}


QString PinSettingsWidgetDesignerPlugin::whatsThis() const
{
	return QString();
}


bool PinSettingsWidgetDesignerPlugin::isContainer() const
{
	return false;
}


bool PinSettingsWidgetDesignerPlugin::isInitialized() const
{
	return mInitialized;
}


QIcon PinSettingsWidgetDesignerPlugin::icon() const
{
	return QIcon();
}


void PinSettingsWidgetDesignerPlugin::initialize(QDesignerFormEditorInterface*)
{
	if (mInitialized)
	{
		return;
	}

	mInitialized = true;
}
