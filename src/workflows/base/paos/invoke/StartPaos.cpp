/*!
 * \copyright Copyright (c) 2014-2020 Governikus GmbH & Co. KG, Germany
 */

#include "StartPaos.h"

#include "paos/PaosType.h"

using namespace governikus;

StartPaos::StartPaos(const QByteArray& pSessionId)
	: PaosCreator()
	, PaosMessage(PaosType::STARTPAOS)
	, mSessionId(pSessionId)
	, mUserAgent()
	, mSupportedAPI()
{
	Q_ASSERT(!mSessionId.isEmpty());
}


void StartPaos::createConnectionHandleElement()
{
	mWriter.writeStartElement(QStringLiteral("ConnectionHandle"));
	mWriter.writeAttribute(getNamespacePrefix(Namespace::XSI), getNamespace(Namespace::XSI));
	mWriter.writeAttribute(getNamespacePrefix(Namespace::XSI, QStringLiteral("type")), QStringLiteral("ConnectionHandleType"));

	mWriter.writeTextElement(QStringLiteral("CardApplication"), QStringLiteral("e80704007f00070302"));
	mWriter.writeTextElement(QStringLiteral("SlotHandle"), QStringLiteral("00"));

	mWriter.writeEndElement(); // ConnectionHandle
}


void StartPaos::createUserAgentElement()
{
	mWriter.writeStartElement(QStringLiteral("UserAgent"));

	mWriter.writeTextElement(QStringLiteral("Name"), mUserAgent.getName());
	mWriter.writeTextElement(QStringLiteral("VersionMajor"), mUserAgent.getVersionMajor());
	mWriter.writeTextElement(QStringLiteral("VersionMinor"), mUserAgent.getVersionMinor());
	mWriter.writeTextElement(QStringLiteral("VersionSubminor"), mUserAgent.getVersionSubminor());

	mWriter.writeEndElement(); // UserAgent
}


void StartPaos::createSupportedAPIVersionsElement()
{
	mWriter.writeStartElement(QStringLiteral("SupportedAPIVersions"));

	mWriter.writeTextElement(QStringLiteral("Major"), mSupportedAPI.getMajor());
	mWriter.writeTextElement(QStringLiteral("Minor"), mSupportedAPI.getMinor());
	mWriter.writeTextElement(QStringLiteral("Subminor"), mSupportedAPI.getSubminor());

	mWriter.writeEndElement(); // SupportedAPIVersions
}


void StartPaos::createSessionIdentifierElement()
{
	writeTextElement(QStringLiteral("SessionIdentifier"), mSessionId);
}


void StartPaos::createBodyElement()
{
	mWriter.writeStartElement(QStringLiteral("StartPAOS"));
	mWriter.writeAttribute(getNamespacePrefix(Namespace::DEFAULT), getNamespace(Namespace::TECHSCHEMA));

	createSessionIdentifierElement();
	createConnectionHandleElement();
	createUserAgentElement();
	createSupportedAPIVersionsElement();

	mWriter.writeEndElement(); // StartPAOS
}
