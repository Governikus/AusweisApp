/*!
 * \copyright Copyright (c) 2014-2017 Governikus GmbH & Co. KG, Germany
 */

#include "DidListResponse.h"

using namespace governikus;

DIDListResponse::DIDListResponse()
	: ResponseType(PaosType::DID_LIST_RESPONSE)
{
}


QDomElement DIDListResponse::getDocumentStructure()
{
	return createEnvelopeElement(createDidListResponseElement(), getRelatesTo(), getMessageId());
}


QDomElement DIDListResponse::createDidListResponseElement()
{
	QDomElement element = mDoc.createElement(QStringLiteral("DIDListResponse"));
	element.setAttribute(getNamespacePrefix(Namespace::TECHSCHEMA), getNamespace(Namespace::TECHSCHEMA));
	element.setAttribute(QStringLiteral("Profile"), getNamespace(Namespace::ECARD));

	element.appendChild(createResultElement(*this));
	element.appendChild(createDidListElement());

	return element;
}


QDomElement DIDListResponse::createDidListElement()
{
	QDomElement didNameList = mDoc.createElement(QStringLiteral("DIDNameList"));
	// create enumeration for PIN (see TR)
	didNameList.appendChild(createTextElement(QStringLiteral("DIDName"), QStringLiteral("PIN")));
	return didNameList;
}
