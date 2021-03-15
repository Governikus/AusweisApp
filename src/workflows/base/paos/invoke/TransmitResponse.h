/*!
 * \brief Generate information for TransmitResponse.
 *
 * \copyright Copyright (c) 2014-2021 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "paos/ResponseType.h"
#include "PaosCreator.h"

namespace governikus
{

class TransmitResponse
	: public PaosCreator
	, public ResponseType
{
	private:
		QByteArrayList mOutputApdus;

		void createTransmitResponse();

		void createBodyElement() override;

		Q_DISABLE_COPY(TransmitResponse)

	public:
		TransmitResponse();

		void setOutputApdus(const QByteArrayList& outputApdus);
};

} // namespace governikus
