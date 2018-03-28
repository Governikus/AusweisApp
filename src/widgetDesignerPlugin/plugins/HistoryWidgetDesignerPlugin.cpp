/*!
 * \copyright Copyright (c) 2017-2018 Governikus GmbH & Co. KG, Germany
 */

#include "HistoryWidgetDesignerPlugin.h"

#include "HistoryWidget.h"

using namespace governikus;


HistoryWidgetDesignerPlugin::HistoryWidgetDesignerPlugin(QObject* pParent)
	: QObject(pParent)
	, mInitialized(false)
{

}


QWidget* HistoryWidgetDesignerPlugin::createWidget(QWidget* pParent)
{
	return new HistoryWidget(pParent);
}


QString HistoryWidgetDesignerPlugin::domXml() const
{
	return QStringLiteral(
			"<ui language=\"c++\">\n"
			" <widget class=\"governikus::HistoryWidget\" name=\"historyWidget\">\n"
			" </widget>\n"
			"</ui>\n");
}


QString HistoryWidgetDesignerPlugin::includeFile() const
{
	return QStringLiteral("HistoryWidget.h");
}


QString HistoryWidgetDesignerPlugin::name() const
{
	return QStringLiteral("governikus::HistoryWidget");
}


QString HistoryWidgetDesignerPlugin::group() const
{
	return QStringLiteral("Governikus");
}


QString HistoryWidgetDesignerPlugin::toolTip() const
{
	return QString();
}


QString HistoryWidgetDesignerPlugin::whatsThis() const
{
	return QString();
}


bool HistoryWidgetDesignerPlugin::isContainer() const
{
	return false;
}


bool HistoryWidgetDesignerPlugin::isInitialized() const
{
	return mInitialized;
}


QIcon HistoryWidgetDesignerPlugin::icon() const
{
	return QIcon();
}


void HistoryWidgetDesignerPlugin::initialize(QDesignerFormEditorInterface*)
{
	if (mInitialized)
	{
		return;
	}

	mInitialized = true;
}
