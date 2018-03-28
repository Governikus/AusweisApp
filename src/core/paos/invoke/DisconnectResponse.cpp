/*!
 * \copyright Copyright (c) 2014-2018 Governikus GmbH & Co. KG, Germany
 */

#include "DisconnectResponse.h"

using namespace governikus;


DisconnectResponse::DisconnectResponse()
	: ResponseType(PaosType::DISCONNECT_RESPONSE)
{
}


QDomElement DisconnectResponse::getDocumentStructure()
{
	return createEnvelopeElement(createDisconnectResponse(), getRelatesTo(), getMessageId());
}


QDomElement DisconnectResponse::createDisconnectResponse()
{
	QDomElement element = mDoc.createElement(QStringLiteral("DisconnectResponse"));
	element.setAttribute(getNamespacePrefix(Namespace::DEFAULT), getNamespace(Namespace::TECHSCHEMA));
	element.setAttribute(QStringLiteral("Profile"), getNamespace(Namespace::ECARD));

	element.appendChild(createResultElement(*this));
	if (!mSlotHandle.isNull())
	{
		element.appendChild(createTextElement(QStringLiteral("SlotHandle"), mSlotHandle));
	}

	return element;
}


void DisconnectResponse::setSlotHandle(const QString& slotHandle)
{
	mSlotHandle = slotHandle;
}
