/**
 * Copyright (c) 2014-2023 Governikus GmbH & Co. KG, Germany
 */

#include "ElementParser.h"

#include "paos/element/ConnectionHandleParser.h"

using namespace governikus;

ElementParser::ElementParser(QSharedPointer<QXmlStreamReader> pXmlReader)
	: mXmlReader(pXmlReader)
	, mParseError(false)
{
}


ElementParser::~ElementParser() = default;


bool ElementParser::parserFailed() const
{
	return mParseError;
}


bool ElementParser::readNextStartElement()
{
	return !mParseError && mXmlReader->readNextStartElement();
}


QString ElementParser::readElementText()
{
	QString text;

	while (mXmlReader->error() == QXmlStreamReader::NoError && !mXmlReader->isEndElement())
	{
		if (mXmlReader->readNext() == QXmlStreamReader::TokenType::Characters && !mXmlReader->isWhitespace())
		{
			text += QLatin1Char(' ');
			text += mXmlReader->text();
		}
	}

	if (mXmlReader->error() != QXmlStreamReader::NoError)
	{
		return QString();
	}

	return text.isEmpty() ? QLatin1String("") : text.simplified();
}


bool ElementParser::assertNoDuplicateElement(bool pNotYetSeen)
{
	if (!pNotYetSeen)
	{
		qCWarning(paos) << "Duplicate unique element:" << mXmlReader->name();
		mParseError = true;
	}

	return pNotYetSeen;
}


void ElementParser::assertMandatoryElement(const QString& pValue, const char* const pElementName)
{
	if (pValue.isNull())
	{
		qCWarning(paos) << "Mandatory element is null:" << pElementName;
		mParseError = true;
	}
}


bool ElementParser::readUniqueElementText(QString& pText)
{
	if (!assertNoDuplicateElement(pText.isNull()))
	{
		return false;
	}

	pText = readElementText();
	return !pText.isNull();
}


void ElementParser::skipCurrentElement() const
{
	mXmlReader->skipCurrentElement();
}


QStringView ElementParser::getElementName() const
{
	const auto& name = mXmlReader->name();
	qCDebug(paos) << name;
	return name;
}


QStringView ElementParser::getElementTypeByNamespace(const QString& pNamespace) const
{
	return mXmlReader->attributes().value(pNamespace, QStringLiteral("type"));
}


void ElementParser::initData(const QByteArray& pXmlData)
{
	mParseError = false;
	mXmlReader->clear();
	mXmlReader->addData(pXmlData);
}


void ElementParser::setParserFailed()
{
	mParseError = true;
}


ConnectionHandle ElementParser::parseConnectionHandle()
{
	ConnectionHandleParser parser(mXmlReader);
	const auto& handle = parser.parse();
	mParseError |= parser.parserFailed();
	return handle;
}
