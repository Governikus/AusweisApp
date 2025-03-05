/**
 * Copyright (c) 2014-2025 Governikus GmbH & Co. KG, Germany
 */

#include "StartPaos.h"

#include "paos/PaosType.h"
#include "paos/element/SupportedApi.h"
#include "paos/element/UserAgent.h"

using namespace governikus;

StartPaos::StartPaos(const QByteArray& pSessionId)
	: PaosCreator()
	, PaosMessage(PaosType::STARTPAOS)
	, mSessionId(pSessionId)
{
	Q_ASSERT(!mSessionId.isEmpty());
}


void StartPaos::createBodyElement(QXmlStreamWriter& pWriter)
{
	pWriter.writeStartElement(QStringLiteral("StartPAOS"));
	pWriter.writeAttribute(getNamespacePrefix(Namespace::DEFAULT), getNamespace(Namespace::TECHSCHEMA));

	createSessionIdentifierElement();
	createConnectionHandleElement(pWriter);
	createUserAgentElement(pWriter);
	createSupportedAPIVersionsElement(pWriter);

	pWriter.writeEndElement(); // StartPAOS
}


void StartPaos::createSessionIdentifierElement()
{
	writeTextElement(QStringLiteral("SessionIdentifier"), mSessionId);
}


void StartPaos::createConnectionHandleElement(QXmlStreamWriter& pWriter) const
{
	pWriter.writeStartElement(QStringLiteral("ConnectionHandle"));
	pWriter.writeAttribute(getNamespacePrefix(Namespace::XSI), getNamespace(Namespace::XSI));
	pWriter.writeAttribute(getNamespacePrefix(Namespace::XSI, QStringLiteral("type")), QStringLiteral("ConnectionHandleType"));

	pWriter.writeTextElement(QStringLiteral("CardApplication"), QStringLiteral("e80704007f00070302"));
	pWriter.writeTextElement(QStringLiteral("SlotHandle"), QStringLiteral("00"));

	pWriter.writeEndElement(); // ConnectionHandle
}


void StartPaos::createUserAgentElement(QXmlStreamWriter& pWriter) const
{
	pWriter.writeStartElement(QStringLiteral("UserAgent"));

	const UserAgent userAgent;
	pWriter.writeTextElement(QStringLiteral("Name"), userAgent.getName());
	pWriter.writeTextElement(QStringLiteral("VersionMajor"), userAgent.getVersionMajor());
	pWriter.writeTextElement(QStringLiteral("VersionMinor"), userAgent.getVersionMinor());
	pWriter.writeTextElement(QStringLiteral("VersionSubminor"), userAgent.getVersionSubminor());

	pWriter.writeEndElement(); // UserAgent
}


void StartPaos::createSupportedAPIVersionsElement(QXmlStreamWriter& pWriter) const
{
	pWriter.writeStartElement(QStringLiteral("SupportedAPIVersions"));

	const SupportedAPI supportedAPI;
	pWriter.writeTextElement(QStringLiteral("Major"), supportedAPI.getMajor());
	pWriter.writeTextElement(QStringLiteral("Minor"), supportedAPI.getMinor());
	pWriter.writeTextElement(QStringLiteral("Subminor"), supportedAPI.getSubminor());

	pWriter.writeEndElement(); // SupportedAPIVersions
}
