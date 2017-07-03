/*!
 * \brief Context for changing the Pin.
 *
 * \copyright Copyright (c) 2015 Governikus GmbH & Co. KG
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
	QString mPuk;
	QString mSuccessMessage;

	public:
		ChangePinContext();

		const QString& getNewPin() const;
		void setNewPin(const QString& pNewPin);

		const QString& getPuk() const;
		void setPuk(const QString& pPuk);

		const QString& getSuccessMessage() const;
		void setSuccessMessage(const QString& pSuccessMessage);

	Q_SIGNALS:
		void fireNewPinChanged();
		void firePukChanged();
		void fireSuccessMessageChanged();
};

} /* namespace governikus */
