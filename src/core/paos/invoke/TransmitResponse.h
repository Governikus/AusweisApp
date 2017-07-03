/*!
 * \brief Generate information for TransmitResponse.
 *
 * \copyright Copyright (c) 2014 Governikus GmbH & Co. KG
 */

#pragma once

#include "PaosCreator.h"
#include "paos/ResponseType.h"

class test_TransmitResponse;

namespace governikus
{

class TransmitResponse
	: public PaosCreator
	, public ResponseType
{
	friend class::test_TransmitResponse;

	private:
		QByteArrayList mOutputApdus;

		QDomElement createTransmitResponse();

		virtual QDomElement getDocumentStructure();

		Q_DISABLE_COPY(TransmitResponse)

	public:
		TransmitResponse();

		void setOutputApdus(const QByteArrayList& outputApdus);
};

} /* namespace governikus */
