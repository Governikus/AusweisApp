/*!
 * DetailWidget.h
 *
 * \brief Widget for cvc description.
 *
 * \copyright Copyright (c) 2014 Governikus GmbH & Co. KG
 */

#pragma once

#include <QScopedPointer>

#include "generic/Page.h"

namespace Ui
{
class DetailWidget;
}

namespace governikus
{

class DetailWidget
	: public Page
{
	Q_OBJECT

	public:
		DetailWidget(QWidget* pParent = nullptr);
		virtual ~DetailWidget();

		void setDetails(const QString& pDetails);

	private:
		QScopedPointer<Ui::DetailWidget> mUi;

		void paintEvent(QPaintEvent*);
};

} /* namespace governikus */
