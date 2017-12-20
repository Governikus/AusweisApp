/*!
 * \copyright Copyright (c) 2017 Governikus GmbH & Co. KG, Germany
 */

#include "SettingsWidgetDesignerPlugin.h"

#include "SettingsWidget.h"

using namespace governikus;


SettingsWidgetDesignerPlugin::SettingsWidgetDesignerPlugin(QObject* pParent)
	: QObject(pParent)
	, mInitialized(false)
{

}


QWidget* SettingsWidgetDesignerPlugin::createWidget(QWidget* pParent)
{
	return new SettingsWidget(pParent);
}


QString SettingsWidgetDesignerPlugin::domXml() const
{
	return QStringLiteral(
			"<ui language=\"c++\">\n"
			" <widget class=\"governikus::SettingsWidget\" name=\"settingsWidget\">\n"
			" </widget>\n"
			"</ui>\n");
}


QString SettingsWidgetDesignerPlugin::includeFile() const
{
	return QStringLiteral("SettingsWidget.h");
}


QString SettingsWidgetDesignerPlugin::name() const
{
	return QStringLiteral("governikus::SettingsWidget");
}


QString SettingsWidgetDesignerPlugin::group() const
{
	return QStringLiteral("Governikus");
}


QString SettingsWidgetDesignerPlugin::toolTip() const
{
	return QString();
}


QString SettingsWidgetDesignerPlugin::whatsThis() const
{
	return QString();
}


bool SettingsWidgetDesignerPlugin::isContainer() const
{
	return false;
}


bool SettingsWidgetDesignerPlugin::isInitialized() const
{
	return mInitialized;
}


QIcon SettingsWidgetDesignerPlugin::icon() const
{
	return QIcon();
}


void SettingsWidgetDesignerPlugin::initialize(QDesignerFormEditorInterface*)
{
	if (mInitialized)
	{
		return;
	}

	mInitialized = true;
}
