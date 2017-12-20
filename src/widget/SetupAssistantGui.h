/*!
 * \brief Qt widget based SetupAssistantUi implementation.
 *
 * \copyright Copyright (c) 2016-2017 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "SetupAssistantWizard.h"

class QWidget;

namespace governikus
{

class SetupAssistantGui
	: public QObject
{
	Q_OBJECT

	private:
		QPointer<SetupAssistantWizard> mWizard;

	public:
		SetupAssistantGui(QWidget* pParentWidget);
		virtual ~SetupAssistantGui();

		void activate();
		void deactivate();

	Q_SIGNALS:
		void fireChangePinButtonClicked();

};

} /* namespace governikus */
