/*!
 * \brief Context for changing the Pin.
 *
 * \copyright Copyright (c) 2015-2018 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "WorkflowContext.h"

namespace governikus
{

class ChangePinContext
	: public WorkflowContext
{
	Q_OBJECT

	QString mNewPin;
	QString mSuccessMessage;

	public:
		ChangePinContext();

		const QString& getNewPin() const;
		void setNewPin(const QString& pNewPin);

		const QString& getSuccessMessage() const;
		void setSuccessMessage(const QString& pSuccessMessage);

	Q_SIGNALS:
		void fireNewPinChanged();
		void fireSuccessMessageChanged();
};

} /* namespace governikus */
