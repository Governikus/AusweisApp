/**
 * Copyright (c) 2014-2026 Governikus Service GmbH, Germany
 */

#include "ElementParser.h"


using namespace governikus;


Q_DECLARE_LOGGING_CATEGORY(paos)
Q_DECLARE_LOGGING_CATEGORY(secure)


ElementParser::ElementParser(QSharedPointer<QXmlStreamReader> pXmlReader, bool pLoggingAllowed)
	: mXmlReader(pXmlReader)
	, mParseError(false)
	, mLogger(spawnMessageLogger(pLoggingAllowed ? paos : secure))
	, mLoggerIndent(0)
{
	if (!pLoggingAllowed)
	{
		qCDebug(paos).noquote() << "no-log was requested, skip logging of xml data";
	}
}


QDebug ElementParser::logXml() const
{
	return mLogger.debug().noquote().nospace() << QByteArray(mLoggerIndent * 2, ' ');
}


QString ElementParser::toString(const QXmlStreamAttributes& pAttributes, QLatin1Char pJoin)
{
	QStringList parts;
	parts.reserve(pAttributes.size());

	for (const auto& entry : pAttributes)
	{
		parts << QStringLiteral("%1=\"%2\"").arg(entry.qualifiedName(), entry.value());
	}

	return parts.isEmpty() ? QString() : pJoin + parts.join(pJoin);
}


bool ElementParser::parserFailed() const
{
	return mParseError;
}


bool ElementParser::readNextStartElement()
{
	if (mParseError)
	{
		return false;
	}

	if (mXmlReader->isEndElement())
	{
		--mLoggerIndent;
		logXml() << '/' << mXmlReader->qualifiedName();
	}

	if (!mXmlReader->readNextStartElement())
	{
		return false;
	}

	logXml() << mXmlReader->qualifiedName() << toString(mXmlReader->attributes());
	++mLoggerIndent;

	return true;
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

	logXml() << text;
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
	return mXmlReader->name();
}


QStringView ElementParser::getElementTypeByNamespace(const QString& pNamespace) const
{
	return mXmlReader->attributes().value(pNamespace, QStringLiteral("type"));
}


void ElementParser::setParserFailed()
{
	mParseError = true;
}


const QLoggingCategory& ElementParser::getLoggingCategory()
{
	return paos();
}


void ElementParser::detectStartElements(const QStringList& pStartElementNames, const HandleFoundElement& pFunc)
{
	for (; !mXmlReader->atEnd(); mXmlReader->readNext())
	{
		if (mXmlReader->hasError())
		{
			qCWarning(paos) << "Error parsing PAOS message:" << mXmlReader->errorString();
			return;
		}
		else if (mXmlReader->isStartElement() && !handleStartElements(pStartElementNames, pFunc))
		{
			return;
		}
	}
}


bool ElementParser::handleStartElements(const QStringList& pStartElementNames, const HandleFoundElement& pFunc)
{
	const QString name = mXmlReader->name().toString();
	if (pStartElementNames.contains(name))
	{
		QXmlStreamAttributes attributes = mXmlReader->attributes();
		QString value;
		if (mXmlReader->readNext() == QXmlStreamReader::TokenType::Characters && !mXmlReader->isWhitespace())
		{
			value = mXmlReader->text().toString().simplified();
		}

		return pFunc(name, value, attributes);
	}

	return true;
}
