/*!
 * \brief Widget for history item.
 *
 * \copyright Copyright (c) 2014-2017 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include <QScopedPointer>
#include <QWidget>

namespace Ui
{
class HistoryDetailWidget;
}

namespace governikus
{

class HistoryDetailWidget
	: public QWidget
{
	Q_OBJECT

	private:
		QScopedPointer<Ui::HistoryDetailWidget> mUi;

		virtual void paintEvent(QPaintEvent*) override;

	protected:
		virtual void changeEvent(QEvent* pEvent) override;

	public:
		HistoryDetailWidget(QWidget* pParent = nullptr);
		virtual ~HistoryDetailWidget() override;

		void setDetails(const QString& pDetails);
};

} /* namespace governikus */
