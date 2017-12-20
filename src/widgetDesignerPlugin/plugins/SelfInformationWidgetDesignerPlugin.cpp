/*!
 * \copyright Copyright (c) 2017 Governikus GmbH & Co. KG, Germany
 */

#include "SelfInformationWidgetDesignerPlugin.h"

#include "SelfInformationWidget.h"

using namespace governikus;


SelfInformationWidgetDesignerPlugin::SelfInformationWidgetDesignerPlugin(QObject* pParent)
	: QObject(pParent)
	, mInitialized(false)
{

}


QWidget* SelfInformationWidgetDesignerPlugin::createWidget(QWidget* pParent)
{
	return new SelfInformationWidget(pParent);
}


QString SelfInformationWidgetDesignerPlugin::domXml() const
{
	return QStringLiteral(
			"<ui language=\"c++\">\n"
			" <widget class=\"governikus::SelfInformationWidget\" name=\"selfInformationWidget\">\n"
			" </widget>\n"
			"</ui>\n");
}


QString SelfInformationWidgetDesignerPlugin::includeFile() const
{
	return QStringLiteral("SelfInformationWidget.h");
}


QString SelfInformationWidgetDesignerPlugin::name() const
{
	return QStringLiteral("governikus::SelfInformationWidget");
}


QString SelfInformationWidgetDesignerPlugin::group() const
{
	return QStringLiteral("Governikus");
}


QString SelfInformationWidgetDesignerPlugin::toolTip() const
{
	return QString();
}


QString SelfInformationWidgetDesignerPlugin::whatsThis() const
{
	return QString();
}


bool SelfInformationWidgetDesignerPlugin::isContainer() const
{
	return false;
}


bool SelfInformationWidgetDesignerPlugin::isInitialized() const
{
	return mInitialized;
}


QIcon SelfInformationWidgetDesignerPlugin::icon() const
{
	return QIcon();
}


void SelfInformationWidgetDesignerPlugin::initialize(QDesignerFormEditorInterface*)
{
	if (mInitialized)
	{
		return;
	}

	mInitialized = true;
}
