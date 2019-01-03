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
	const bool mRequestTransportPin;

	public:
		explicit ChangePinContext(bool pRequestTransportPin = false);

		const QString& getNewPin() const;
		void setNewPin(const QString& pNewPin);

		void resetPacePasswords() override;

		const QString& getSuccessMessage() const;
		void setSuccessMessage(const QString& pSuccessMessage);

		bool requestTransportPin() const;

	Q_SIGNALS:
		void fireNewPinChanged();
		void fireSuccessMessageChanged();
};

} // namespace governikus
