/*!
 * \copyright Copyright (c) 2019-2020 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "ReaderManagerPlugInInfo.h"

#include <QQuickItem>

namespace governikus
{

class ReaderScanEnabler
	: public QQuickItem
{
	Q_OBJECT

	Q_PROPERTY(ReaderManagerPlugInType plugInType READ getPlugInType WRITE setPlugInType)

	private:
		ReaderManagerPlugInType mPlugInType;
		bool mObligedToStopScan;

		void enableScan(const bool pEnable);

	public:
		explicit ReaderScanEnabler(QQuickItem* pParent = nullptr);
		~ReaderScanEnabler() override;

		ReaderManagerPlugInType getPlugInType() const;
		void setPlugInType(ReaderManagerPlugInType pPlugInType);
		void itemChange(QQuickItem::ItemChange pChange, const QQuickItem::ItemChangeData& pValue) override;
};


} // namespace governikus
