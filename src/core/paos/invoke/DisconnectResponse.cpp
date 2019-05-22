/*!
 * \copyright Copyright (c) 2014-2019 Governikus GmbH & Co. KG, Germany
 */

#include "DisconnectResponse.h"

using namespace governikus;


DisconnectResponse::DisconnectResponse()
	: ResponseType(PaosType::DISCONNECT_RESPONSE)
{
}


void DisconnectResponse::createBodyElement()
{
	mWriter.writeStartElement(QStringLiteral("DisconnectResponse"));
	mWriter.writeAttribute(getNamespacePrefix(Namespace::DEFAULT), getNamespace(Namespace::TECHSCHEMA));
	mWriter.writeAttribute(QStringLiteral("Profile"), getNamespace(Namespace::ECARD));

	createResultElement(*this);
	if (!mSlotHandle.isNull())
	{
		mWriter.writeTextElement(QStringLiteral("SlotHandle"), mSlotHandle);
	}

	mWriter.writeEndElement(); // DisconnectResponse
}


void DisconnectResponse::setSlotHandle(const QString& slotHandle)
{
	mSlotHandle = slotHandle;
}
