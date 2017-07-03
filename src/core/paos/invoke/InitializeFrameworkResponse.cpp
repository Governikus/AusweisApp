/*!
 * \copyright Copyright (c) 2014 Governikus GmbH & Co. KG
 */

#include "InitializeFrameworkResponse.h"

using namespace governikus;

InitializeFrameworkResponse::InitializeFrameworkResponse()
	: ResponseType(PaosType::INITIALIZE_FRAMEWORK_RESPONSE)
{
}


QDomElement InitializeFrameworkResponse::getDocumentStructure()
{
	return createEnvelopeElement(createInitializeFrameworkResponse(), getRelatesTo(), getMessageId());
}


QDomElement InitializeFrameworkResponse::createInitializeFrameworkResponse()
{
	QDomElement element = mDoc.createElement(QStringLiteral("InitializeFrameworkResponse"));
	element.setAttribute(getNamespacePrefix(Namespace::DEFAULT), getNamespace(Namespace::ECARD));
	element.setAttribute(QStringLiteral("Profile"), getNamespace(Namespace::ECARD));

	element.appendChild(createResultElement());
	element.appendChild(createVersionElement());

	return element;
}


QDomElement InitializeFrameworkResponse::createVersionElement()
{
	QDomElement element = mDoc.createElement(QStringLiteral("Version"));

	element.appendChild(createTextElement(QStringLiteral("Major"), mSupportedAPI.getMajor()));
	element.appendChild(createTextElement(QStringLiteral("Minor"), mSupportedAPI.getMinor()));
	element.appendChild(createTextElement(QStringLiteral("SubMinor"), mSupportedAPI.getSubminor()));

	return element;
}
