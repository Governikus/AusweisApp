/*!
 * \brief Generate information for DisconnectResponse.
 *
 * \copyright Copyright (c) 2014-2022 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "paos/ResponseType.h"
#include "PaosCreator.h"

namespace governikus
{

class DisconnectResponse
	: public PaosCreator
	, public ResponseType
{
	private:
		QString mSlotHandle;

		Q_DISABLE_COPY(DisconnectResponse)

		void createDisconnectResponse();

		void createBodyElement() override;

	public:
		DisconnectResponse();
		void setSlotHandle(const QString& slotHandle);
};

} // namespace governikus
