/*!
 * \copyright Copyright (c) 2014-2020 Governikus GmbH & Co. KG, Germany
 */

#include "DidListResponse.h"

using namespace governikus;

DIDListResponse::DIDListResponse()
	: ResponseType(PaosType::DID_LIST_RESPONSE)
{
}


void DIDListResponse::createBodyElement()
{
	mWriter.writeStartElement(QStringLiteral("DIDListResponse"));
	mWriter.writeAttribute(getNamespacePrefix(Namespace::TECHSCHEMA), getNamespace(Namespace::TECHSCHEMA));
	mWriter.writeAttribute(QStringLiteral("Profile"), getNamespace(Namespace::ECARD));

	createResultElement(*this);

	mWriter.writeStartElement(QStringLiteral("DIDNameList"));
	mWriter.writeTextElement(QStringLiteral("DIDName"), QStringLiteral("PIN"));
	mWriter.writeEndElement(); // DIDNameList

	mWriter.writeEndElement(); // DIDListResponse
}
