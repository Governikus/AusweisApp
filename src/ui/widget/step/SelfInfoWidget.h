/*!
 * \brief A widget displaying the card data retrieved in the self info workflow.
 *
 * \copyright Copyright (c) 2014-2018 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "SelfAuthenticationData.h"

#include <QDateTime>
#include <QScopedPointer>
#include <QWidget>

namespace Ui
{
class SelfInfoWidget;
} // namespace Ui

namespace governikus
{
class SelfInfoWidget
	: public QWidget
{
	Q_OBJECT

	public:
		SelfInfoWidget(QWidget* pParent = nullptr);
		virtual ~SelfInfoWidget() override;

		void setInfo(const SelfAuthenticationData& pData);

	protected:
		void paintEvent(QPaintEvent*) override;
		void changeEvent(QEvent* pEvent) override;

	private:
		QScopedPointer<Ui::SelfInfoWidget> mUi;
		SelfAuthenticationData mSelfAuthenticationData;

		void add(const QString& pKey, const QString& pValue);
		void fillLayout();
		void cleanLayout(QLayout* pLayout);

	private Q_SLOTS:
		void onPrintButtonClicked();
};

} // namespace governikus
