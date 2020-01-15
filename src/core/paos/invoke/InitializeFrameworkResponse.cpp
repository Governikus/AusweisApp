/*!
 * \copyright Copyright (c) 2014-2020 Governikus GmbH & Co. KG, Germany
 */

#include "InitializeFrameworkResponse.h"

using namespace governikus;

InitializeFrameworkResponse::InitializeFrameworkResponse()
	: ResponseType(PaosType::INITIALIZE_FRAMEWORK_RESPONSE)
{
}


void InitializeFrameworkResponse::createBodyElement()
{
	mWriter.writeStartElement(QStringLiteral("InitializeFrameworkResponse"));

	mWriter.writeAttribute(getNamespacePrefix(Namespace::DEFAULT), getNamespace(Namespace::ECARD));
	mWriter.writeAttribute(QStringLiteral("Profile"), getNamespace(Namespace::ECARD));

	createResultElement(*this);
	createVersionElement();

	mWriter.writeEndElement(); // InitializeFrameworkResponse
}


void InitializeFrameworkResponse::createVersionElement()
{
	mWriter.writeStartElement(QStringLiteral("Version"));

	mWriter.writeTextElement(QStringLiteral("Major"), mSupportedAPI.getMajor());
	mWriter.writeTextElement(QStringLiteral("Minor"), mSupportedAPI.getMinor());
	mWriter.writeTextElement(QStringLiteral("SubMinor"), mSupportedAPI.getSubminor());

	mWriter.writeEndElement(); // InitializeFrameworkResponse
}
