/**
 * Copyright (c) 2014-2024 Governikus GmbH & Co. KG, Germany
 */

#include "TransmitResponse.h"

using namespace governikus;

TransmitResponse::TransmitResponse()
	: ResponseType(PaosType::TRANSMIT_RESPONSE)
	, mOutputApdus()
{
}


void TransmitResponse::createBodyElement(QXmlStreamWriter& pWriter)
{
	pWriter.writeStartElement(QStringLiteral("TransmitResponse"));
	pWriter.writeAttribute(getNamespacePrefix(Namespace::DEFAULT), getNamespace(Namespace::TECHSCHEMA));
	pWriter.writeAttribute(QStringLiteral("Profile"), getNamespace(Namespace::ECARD));

	createResultElement(*this);

	for (const auto& apdu : std::as_const(mOutputApdus))
	{
		writeTextElement(QStringLiteral("OutputAPDU"), apdu);
	}

	pWriter.writeEndElement(); // TransmitResponse
}


void TransmitResponse::setOutputApdus(const QByteArrayList& outputApdus)
{
	mOutputApdus = outputApdus;
}
