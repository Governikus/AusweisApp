/*!
 * \brief Widget for starting the self information workflow.
 *
 * \copyright Copyright (c) 2014 Governikus GmbH & Co. KG
 */

#pragma once

#include "generic/Page.h"

#include <QScopedPointer>

namespace Ui
{
class SelfInformationWidget;
}

namespace governikus
{

class SelfInformationWidget
	: public Page
{
	Q_OBJECT

	public:
		SelfInformationWidget(QWidget* pParent = nullptr);
		virtual ~SelfInformationWidget();

	Q_SIGNALS:
		void selfAuthenticationRequested();

	private:
		QScopedPointer<Ui::SelfInformationWidget> mUi;
		QScopedPointer<QPixmap> mPixDescLogoLabel;

		void paintEvent(QPaintEvent*);
};

} /* namespace governikus */
