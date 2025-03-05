/**
 * Copyright (c) 2014-2025 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "PaosCreator.h"
#include "paos/ResponseType.h"

namespace governikus
{

class TransmitResponse
	: public PaosCreator
	, public ResponseType
{
	Q_DISABLE_COPY(TransmitResponse)

	private:
		QByteArrayList mOutputApdus;

		void createBodyElement(QXmlStreamWriter& pWriter) override;

	public:
		TransmitResponse();

		void setOutputApdus(const QByteArrayList& outputApdus);
};

} // namespace governikus
