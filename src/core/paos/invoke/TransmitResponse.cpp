/*!
 * \copyright Copyright (c) 2014-2017 Governikus GmbH & Co. KG, Germany
 */

#include "TransmitResponse.h"

using namespace governikus;

TransmitResponse::TransmitResponse()
	: ResponseType(PaosType::TRANSMIT_RESPONSE)
	, mOutputApdus()
{
}


QDomElement TransmitResponse::getDocumentStructure()
{
	return createEnvelopeElement(createTransmitResponse(), getRelatesTo(), getMessageId());
}


QDomElement TransmitResponse::createTransmitResponse()
{
	QDomElement element = mDoc.createElement(QStringLiteral("TransmitResponse"));
	element.setAttribute(getNamespacePrefix(Namespace::DEFAULT), getNamespace(Namespace::TECHSCHEMA));
	element.setAttribute(QStringLiteral("Profile"), getNamespace(Namespace::ECARD));

	element.appendChild(createResultElement(*this));

	for (const auto& apdu : qAsConst(mOutputApdus))
	{
		element.appendChild(createTextElement(QStringLiteral("OutputAPDU"), apdu));
	}

	return element;
}


void TransmitResponse::setOutputApdus(const QByteArrayList& outputApdus)
{
	mOutputApdus = outputApdus;
}
