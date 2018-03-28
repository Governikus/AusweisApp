/*!
 * \brief Generate information for TransmitResponse.
 *
 * \copyright Copyright (c) 2014-2018 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "paos/ResponseType.h"
#include "PaosCreator.h"

class test_TransmitResponse;

namespace governikus
{

class TransmitResponse
	: public PaosCreator
	, public ResponseType
{
	friend class ::test_TransmitResponse;

	private:
		QByteArrayList mOutputApdus;

		QDomElement createTransmitResponse();

		virtual QDomElement getDocumentStructure() override;

		Q_DISABLE_COPY(TransmitResponse)

	public:
		TransmitResponse();

		void setOutputApdus(const QByteArrayList& outputApdus);
};

} /* namespace governikus */
