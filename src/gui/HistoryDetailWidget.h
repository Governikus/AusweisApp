/*!
 * \brief Widget for history item.
 *
 * \copyright Copyright (c) 2014 Governikus GmbH & Co. KG
 */

#pragma once

#include "generic/Page.h"

#include <QScopedPointer>

namespace Ui
{
class HistoryDetailWidget;
}

namespace governikus
{

class HistoryDetailWidget
	: public Page
{
	Q_OBJECT

	public:
		HistoryDetailWidget(QWidget* pParent = nullptr);
		virtual ~HistoryDetailWidget();

		void setDetails(const QString& pDetails);

	private:
		QScopedPointer<Ui::HistoryDetailWidget> mUi;

		void paintEvent(QPaintEvent*);
};

} /* namespace governikus */
