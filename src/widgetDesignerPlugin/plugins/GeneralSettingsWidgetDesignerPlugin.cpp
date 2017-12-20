/*!
 * \copyright Copyright (c) 2017 Governikus GmbH & Co. KG, Germany
 */

#include "GeneralSettingsWidgetDesignerPlugin.h"

#include "GeneralSettingsWidget.h"

using namespace governikus;


GeneralSettingsWidgetDesignerPlugin::GeneralSettingsWidgetDesignerPlugin(QObject* pParent)
	: QObject(pParent)
	, mInitialized(false)
{

}


QWidget* GeneralSettingsWidgetDesignerPlugin::createWidget(QWidget* pParent)
{
	return new GeneralSettingsWidget(pParent);
}


QString GeneralSettingsWidgetDesignerPlugin::domXml() const
{
	return QStringLiteral(
			"<ui language=\"c++\">\n"
			" <widget class=\"governikus::GeneralSettingsWidget\" name=\"generalSettingsWidget\">\n"
			" </widget>\n"
			"</ui>\n");
}


QString GeneralSettingsWidgetDesignerPlugin::includeFile() const
{
	return QStringLiteral("GeneralSettingsWidget.h");
}


QString GeneralSettingsWidgetDesignerPlugin::name() const
{
	return QStringLiteral("governikus::GeneralSettingsWidget");
}


QString GeneralSettingsWidgetDesignerPlugin::group() const
{
	return QStringLiteral("Governikus");
}


QString GeneralSettingsWidgetDesignerPlugin::toolTip() const
{
	return QString();
}


QString GeneralSettingsWidgetDesignerPlugin::whatsThis() const
{
	return QString();
}


bool GeneralSettingsWidgetDesignerPlugin::isContainer() const
{
	return false;
}


bool GeneralSettingsWidgetDesignerPlugin::isInitialized() const
{
	return mInitialized;
}


QIcon GeneralSettingsWidgetDesignerPlugin::icon() const
{
	return QIcon();
}


void GeneralSettingsWidgetDesignerPlugin::initialize(QDesignerFormEditorInterface*)
{
	if (mInitialized)
	{
		return;
	}

	mInitialized = true;
}
