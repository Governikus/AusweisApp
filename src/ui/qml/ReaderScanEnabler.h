/*!
 * \copyright Copyright (c) 2019 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include <QQuickItem>

namespace governikus
{

class ReaderScanEnabler
	: public QQuickItem
{
	Q_OBJECT

	private:
		bool mObligedToStopScan;

		void enableScan(const bool pEnable);

	public:
		explicit ReaderScanEnabler(QQuickItem* pParent = nullptr);
		~ReaderScanEnabler() override;

		void itemChange(QQuickItem::ItemChange pChange, const QQuickItem::ItemChangeData& pValue) override;
};


} // namespace governikus
