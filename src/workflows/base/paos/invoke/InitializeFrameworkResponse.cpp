/**
 * Copyright (c) 2014-2025 Governikus GmbH & Co. KG, Germany
 */

#include "InitializeFrameworkResponse.h"

using namespace governikus;

InitializeFrameworkResponse::InitializeFrameworkResponse()
	: ResponseType(PaosType::INITIALIZE_FRAMEWORK_RESPONSE)
{
}


void InitializeFrameworkResponse::createBodyElement(QXmlStreamWriter& pWriter)
{
	pWriter.writeStartElement(QStringLiteral("InitializeFrameworkResponse"));

	pWriter.writeAttribute(getNamespacePrefix(Namespace::DEFAULT), getNamespace(Namespace::ECARD));
	pWriter.writeAttribute(QStringLiteral("Profile"), getNamespace(Namespace::ECARD));

	createResultElement(*this);
	createVersionElement(pWriter);

	pWriter.writeEndElement(); // InitializeFrameworkResponse
}


void InitializeFrameworkResponse::createVersionElement(QXmlStreamWriter& pWriter) const
{
	pWriter.writeStartElement(QStringLiteral("Version"));

	pWriter.writeTextElement(QStringLiteral("Major"), mSupportedAPI.getMajor());
	pWriter.writeTextElement(QStringLiteral("Minor"), mSupportedAPI.getMinor());
	pWriter.writeTextElement(QStringLiteral("SubMinor"), mSupportedAPI.getSubminor());

	pWriter.writeEndElement(); // InitializeFrameworkResponse
}
