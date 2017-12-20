/*!
 * \copyright Copyright (c) 2017 Governikus GmbH & Co. KG, Germany
 */

#include "GovernikusPluginCollection.h"

#include "plugins/GeneralSettingsWidgetDesignerPlugin.h"
#include "plugins/HistoryWidgetDesignerPlugin.h"
#include "plugins/PinSettingsWidgetDesignerPlugin.h"
#include "plugins/ProviderWidgetDesignerPlugin.h"
#include "plugins/SelfInformationWidgetDesignerPlugin.h"
#include "plugins/SettingsWidgetDesignerPlugin.h"

using namespace governikus;


GovernikusPluginCollection::GovernikusPluginCollection(QObject* pParent)
	: QObject(pParent)
{
	mWidgets += new SelfInformationWidgetDesignerPlugin(this);

	mWidgets += new ProviderWidgetDesignerPlugin(this);

	mWidgets += new HistoryWidgetDesignerPlugin(this);

	mWidgets += new GeneralSettingsWidgetDesignerPlugin(this);
	mWidgets += new PinSettingsWidgetDesignerPlugin(this);
	mWidgets += new SettingsWidgetDesignerPlugin(this);
}


QList<QDesignerCustomWidgetInterface*> GovernikusPluginCollection::customWidgets() const
{
	return mWidgets;
}
