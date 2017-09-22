/*!
 * \copyright Copyright (c) 2014 Governikus GmbH & Co. KG
 */

#include "StartPaos.h"

#include "paos/PaosType.h"

using namespace governikus;

StartPaos::StartPaos(const QByteArray& pSessionId)
	: PaosCreator()
	, PaosMessage(PaosType::STARTPAOS)
	, mSessionId(pSessionId)
{
	Q_ASSERT(!mSessionId.isEmpty());
}


QDomElement StartPaos::getDocumentStructure()
{
	return createEnvelopeElement(createStartPaosElement(), getRelatesTo(), getMessageId());
}


QDomElement StartPaos::createConnectionHandleElement()
{
	QDomElement element = mDoc.createElement(QStringLiteral("ConnectionHandle"));
	element.setAttribute(getNamespacePrefix(Namespace::XSI), getNamespace(Namespace::XSI));
	element.setAttribute(getNamespacePrefix(Namespace::XSI, QStringLiteral("type")), QStringLiteral("ConnectionHandleType"));

	element.appendChild(createTextElement(QStringLiteral("CardApplication"), QStringLiteral("e80704007f00070302")));
	element.appendChild(createTextElement(QStringLiteral("SlotHandle"), QStringLiteral("00")));

	return element;
}


QDomElement StartPaos::createUserAgentElement()
{
	QDomElement element = mDoc.createElement(QStringLiteral("UserAgent"));

	element.appendChild(createTextElement(QStringLiteral("Name"), mUserAgent.getName()));
	element.appendChild(createTextElement(QStringLiteral("VersionMajor"), mUserAgent.getVersionMajor()));
	element.appendChild(createTextElement(QStringLiteral("VersionMinor"), mUserAgent.getVersionMinor()));
	element.appendChild(createTextElement(QStringLiteral("VersionSubminor"), mUserAgent.getVersionSubminor()));

	return element;
}


QDomElement StartPaos::createSupportedAPIVersionsElement()
{
	QDomElement element = mDoc.createElement(QStringLiteral("SupportedAPIVersions"));

	element.appendChild(createTextElement(QStringLiteral("Major"), mSupportedAPI.getMajor()));
	element.appendChild(createTextElement(QStringLiteral("Minor"), mSupportedAPI.getMinor()));
	element.appendChild(createTextElement(QStringLiteral("Subminor"), mSupportedAPI.getSubminor()));

	return element;
}


QDomElement StartPaos::createStartPaosElement()
{
	QDomElement element = mDoc.createElement(QStringLiteral("StartPAOS"));
	element.setAttribute(getNamespacePrefix(Namespace::DEFAULT), getNamespace(Namespace::TECHSCHEMA));

	element.appendChild(createSessionIdentifierElement());
	element.appendChild(createConnectionHandleElement());
	element.appendChild(createUserAgentElement());
	element.appendChild(createSupportedAPIVersionsElement());

	return element;
}


QDomElement StartPaos::createSessionIdentifierElement()
{
	return createTextElement(QStringLiteral("SessionIdentifier"), mSessionId);
}
