/*!
 * SelfInfoWidget.h
 *
 * \brief A widget displaying the card data retrieved in the self info workflow.
 *
 * \copyright Copyright (c) 2014 Governikus GmbH & Co. KG
 */

#pragma once

#include <QScopedPointer>
#include <QWidget>

namespace Ui
{
class SelfInfoWidget;
}

namespace governikus
{

class SelfAuthenticationData;

class SelfInfoWidget
	: public QWidget
{
	Q_OBJECT

	public:
		SelfInfoWidget(QWidget* pParent = nullptr);
		virtual ~SelfInfoWidget();

		void setInfo(SelfAuthenticationData* pData);

	protected:
		void paintEvent(QPaintEvent*) override;

	private:
		QScopedPointer<Ui::SelfInfoWidget> mUi;
		SelfAuthenticationData* mData;

		QString formatDateString(const QString& pIn);
		void insertDataRow(const QString& pLabel, const QString& pField);
		void cleanLayout(QLayout* pLayout);

	private Q_SLOTS:
		void onPrintButtonClicked();
};

} /* namespace governikus */
