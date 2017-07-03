/*!
 * \brief Generate information for DisconnectResponse.
 *
 * \copyright Copyright (c) 2014 Governikus GmbH & Co. KG
 */

#pragma once

#include "PaosCreator.h"
#include "paos/ResponseType.h"

class test_DisconnectResponse;

namespace governikus
{

class DisconnectResponse
	: public PaosCreator
	, public ResponseType
{
	friend class::test_DisconnectResponse;

	private:
		QString mSlotHandle;

		Q_DISABLE_COPY(DisconnectResponse)

		QDomElement createDisconnectResponse();

		virtual QDomElement getDocumentStructure();

	public:
		DisconnectResponse();
		void setSlotHandle(const QString& slotHandle);
};

} /* namespace governikus */
