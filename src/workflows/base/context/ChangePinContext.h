/*!
 * \brief Context for changing the Pin.
 *
 * \copyright Copyright (c) 2015-2022 Governikus GmbH & Co. KG, Germany
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
		~ChangePinContext() override;

		[[nodiscard]] const QString& getNewPin() const;
		void setNewPin(const QString& pNewPin);

		void resetPacePasswords() override;

		[[nodiscard]] const QString& getSuccessMessage() const;
		void setSuccessMessage(const QString& pSuccessMessage);

		[[nodiscard]] bool isRequestTransportPin() const;

	Q_SIGNALS:
		void fireNewPinChanged();
		void fireSuccessMessageChanged();
};

} // namespace governikus
