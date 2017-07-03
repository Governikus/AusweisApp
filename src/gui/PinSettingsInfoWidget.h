/*!
 * \brief Widget for PIN settings information.
 *
 * \copyright Copyright (c) 2014 Governikus GmbH & Co. KG
 */

#pragma once

#include "generic/Page.h"

#include <QScopedPointer>

namespace Ui
{
class PinSettingsInfoWidget;
}

namespace governikus
{

class PinSettingsInfoWidget
	: public Page
{
	Q_OBJECT

	public:
		PinSettingsInfoWidget(QWidget* pParent = nullptr);
		virtual ~PinSettingsInfoWidget();

		void setInfoTitle(const QString& pTitle);
		void setInfoDescription(const QString& pDescription);

	private:
		QScopedPointer<Ui::PinSettingsInfoWidget> mUi;

		void paintEvent(QPaintEvent*);
};

} /* namespace governikus */
