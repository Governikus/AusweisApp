/*!
 * SelfAuthenticationData.cpp
 *
 * \copyright Copyright (c) 2014 Governikus GmbH & Co. KG
 */

#include "SelfAuthenticationData.h"

#include <QDomDocument>
#include <QLoggingCategory>

Q_DECLARE_LOGGING_CATEGORY(secure)

using namespace governikus;


SelfAuthenticationData::SelfAuthenticationData(const QByteArray& pData)
	: QObject()
	, mValid(false)
	, mOperationsAllowed()
	, mSelfAuthData()
	, mDateTime(QDateTime::currentDateTime())
{
	parse(pData);
}


SelfAuthenticationData::~SelfAuthenticationData()
{
}


void SelfAuthenticationData::parse(const QByteArray& pData)
{
	qCDebug(secure) << "parsing data:" << pData;

	QDomDocument doc(QStringLiteral("dataXML"));

	QString errorMsg;
	int errorLine;
	int errorColumn;
	if (!doc.setContent(pData, true, &errorMsg, &errorLine, &errorColumn))
	{
		qDebug() << "XML parsing failed. No valid values to parse. Error in line" << errorLine << "column"
				 << errorColumn << ":" << errorMsg;
		return;
	}

	const auto& parseOperations = std::bind(&SelfAuthenticationData::parseOperationsAllowedByUser, this, std::placeholders::_1);
	const auto& parsePersonal = std::bind(&SelfAuthenticationData::parsePersonalData, this, std::placeholders::_1);
	mValid = parse(doc, QStringLiteral("OperationsAllowedByUser"), parseOperations) && parse(doc, QStringLiteral("PersonalData"), parsePersonal);
}


bool SelfAuthenticationData::parseOperationsAllowedByUser(const QDomElement& pElement)
{
	for (auto elem = pElement; !elem.isNull(); elem = elem.nextSiblingElement())
	{
		auto authData = Enum<SelfAuthData>::fromString(elem.tagName(), SelfAuthData::UNKNOWN);
		if (authData == SelfAuthData::UNKNOWN)
		{
			qWarning() << "SelfAuthData is unknown:" << elem.tagName();
			continue;
		}

		auto permission = Enum<SelfAuthDataPermission>::fromString(elem.text(), SelfAuthDataPermission::UNKNOWN);
		if (permission == SelfAuthDataPermission::UNKNOWN)
		{
			qWarning() << "SelfAuthDataPermission is unknown:" << elem.tagName();
			continue;
		}

		mOperationsAllowed.insert(authData, permission);
	}

	return true;
}


bool SelfAuthenticationData::parsePersonalData(const QDomElement& pElement)
{
	for (auto elem = pElement; !elem.isNull(); elem = elem.nextSiblingElement())
	{
		auto authData = Enum<SelfAuthData>::fromString(elem.tagName(), SelfAuthData::UNKNOWN);
		if (authData == SelfAuthData::UNKNOWN)
		{
			qWarning() << "PersonalData is unknown:" << elem.tagName();
			continue;
		}

		if (authData == SelfAuthData::DateOfBirth)
		{
			tryToInsertChild(elem.firstChildElement(QStringLiteral("DateValue")), authData);
		}
		else if (authData == SelfAuthData::PlaceOfBirth)
		{
			tryToInsertChild(elem.firstChildElement(QStringLiteral("FreetextPlace")), authData);
		}
		else if (authData == SelfAuthData::PlaceOfResidence)
		{
			if (tryToInsertChild(elem.firstChildElement(QStringLiteral("NoPlaceInfo")), SelfAuthData::PlaceOfResidenceNoPlaceInfo))
			{
				mOperationsAllowed.insert(SelfAuthData::PlaceOfResidenceNoPlaceInfo, mOperationsAllowed.value(authData));
			}
			else
			{
				static const QMap<QString, SelfAuthData> placeInfo = {
					{QStringLiteral("City"), SelfAuthData::PlaceOfResidenceCity},
					{QStringLiteral("Country"), SelfAuthData::PlaceOfResidenceCountry},
					{QStringLiteral("Street"), SelfAuthData::PlaceOfResidenceStreet},
					{QStringLiteral("ZipCode"), SelfAuthData::PlaceOfResidenceZipCode}
				};

				auto structuredPlace = elem.firstChildElement(QStringLiteral("StructuredPlace"));
				for (auto iter = placeInfo.constBegin(); iter != placeInfo.constEnd(); ++iter)
				{
					if (tryToInsertChild(structuredPlace.firstChildElement(iter.key()), iter.value()))
					{
						mOperationsAllowed.insert(iter.value(), mOperationsAllowed.value(authData));
					}
				}
			}
		}
		else
		{
			tryToInsertChild(elem, authData);
		}
	}

	return true;
}


bool SelfAuthenticationData::tryToInsertChild(const QDomElement& pElement, SelfAuthData pAuthData)
{
	if (pElement.isNull() || pElement.text().isNull())
	{
		return false;
	}

	mSelfAuthData.insert(pAuthData, pElement.text());
	return true;
}


bool SelfAuthenticationData::parse(const QDomDocument& pDoc, const QString& pElementName, const std::function<bool(const QDomElement&)>& pParserFunc)
{
	const QDomNodeList nodeList = pDoc.documentElement().elementsByTagName(pElementName);
	if (nodeList.size() == 0)
	{
		qCritical() << "XML parsing failed. No valid values to parse for:" << pElementName;
		return false;
	}

	return pParserFunc(nodeList.at(0).toElement().firstChildElement());
}


bool SelfAuthenticationData::isValid() const
{
	return mValid;
}


QString SelfAuthenticationData::getValue(SelfAuthData pData) const
{
	if (mOperationsAllowed.value(pData) == SelfAuthDataPermission::ALLOWED)
	{
		return mSelfAuthData.value(pData);
	}
	else if (mOperationsAllowed.value(pData) == SelfAuthDataPermission::NOTONCHIP)
	{
		if (pData == SelfAuthData::Nationality && getValue(SelfAuthData::DocumentType) == QLatin1String("ID"))
		{
			return QStringLiteral("D");
		}

		return tr("This data has not been stored in this chip generation.");
	}

	return QString();
}


const QDateTime& SelfAuthenticationData::getDateTime() const
{
	return mDateTime;
}
