/*!
 * \copyright Copyright (c) 2017 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include <QDesignerCustomWidgetCollectionInterface>

namespace governikus
{
class GovernikusPluginCollection
	: public QObject
	, public QDesignerCustomWidgetCollectionInterface
{
	Q_OBJECT
	Q_PLUGIN_METADATA(IID "org.qt-project.Qt.QDesignerCustomWidgetCollectionInterface")
	Q_INTERFACES(QDesignerCustomWidgetCollectionInterface)

	public:
		GovernikusPluginCollection(QObject* pParent = 0);
		QList<QDesignerCustomWidgetInterface*> customWidgets() const override;

	private:
		QList<QDesignerCustomWidgetInterface*> mWidgets;
};

}
