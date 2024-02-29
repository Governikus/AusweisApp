/**
 * Copyright (c) 2014-2024 Governikus GmbH & Co. KG, Germany
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

	const UserAgent userAgent;
	mWriter.writeTextElement(QStringLiteral("Name"), userAgent.getName());
	mWriter.writeTextElement(QStringLiteral("VersionMajor"), userAgent.getVersionMajor());
	mWriter.writeTextElement(QStringLiteral("VersionMinor"), userAgent.getVersionMinor());
	mWriter.writeTextElement(QStringLiteral("VersionSubminor"), userAgent.getVersionSubminor());

	mWriter.writeEndElement(); // UserAgent
}


void StartPaos::createSupportedAPIVersionsElement()
{
	mWriter.writeStartElement(QStringLiteral("SupportedAPIVersions"));

	const SupportedAPI supportedAPI;
	mWriter.writeTextElement(QStringLiteral("Major"), supportedAPI.getMajor());
	mWriter.writeTextElement(QStringLiteral("Minor"), supportedAPI.getMinor());
	mWriter.writeTextElement(QStringLiteral("Subminor"), supportedAPI.getSubminor());

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
