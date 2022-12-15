/*!
 * \copyright Copyright (c) 2014-2022 Governikus GmbH & Co. KG, Germany
 */

#include "TransmitResponse.h"

using namespace governikus;

TransmitResponse::TransmitResponse()
	: ResponseType(PaosType::TRANSMIT_RESPONSE)
	, mOutputApdus()
{
}


void TransmitResponse::createBodyElement()
{
	mWriter.writeStartElement(QStringLiteral("TransmitResponse"));
	mWriter.writeAttribute(getNamespacePrefix(Namespace::DEFAULT), getNamespace(Namespace::TECHSCHEMA));
	mWriter.writeAttribute(QStringLiteral("Profile"), getNamespace(Namespace::ECARD));

	createResultElement(*this);

	for (const auto& apdu : std::as_const(mOutputApdus))
	{
		writeTextElement(QStringLiteral("OutputAPDU"), apdu);
	}

	mWriter.writeEndElement(); // TransmitResponse
}


void TransmitResponse::setOutputApdus(const QByteArrayList& outputApdus)
{
	mOutputApdus = outputApdus;
}
