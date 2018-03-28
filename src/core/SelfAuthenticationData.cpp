/*!
 * \copyright Copyright (c) 2014-2018 Governikus GmbH & Co. KG, Germany
 */

#include "SelfAuthenticationData.h"

#include "LanguageLoader.h"

#include <QDomDocument>
#include <QLoggingCategory>

Q_DECLARE_LOGGING_CATEGORY(secure)

using namespace governikus;


SelfAuthenticationData::SelfAuthenticationData(const QByteArray& pData)
	: d(new SelfData(pData))
{
}


bool SelfAuthenticationData::isValid() const
{
	return d->mValid;
}


QString SelfAuthenticationData::getValue(SelfAuthData pData) const
{
	return d->getValue(pData);
}


const QDateTime& SelfAuthenticationData::getDateTime() const
{
	return d->mDateTime;
}


SelfAuthenticationData::OrderedSelfData SelfAuthenticationData::getOrderedSelfData() const
{
	return d->getOrderedSelfInfo();
}


SelfAuthenticationData::SelfData::SelfData(const QByteArray& pData)
	: mValid(false)
	, mDateTime(QDateTime::currentDateTime())
	, mOperationsAllowed()
	, mSelfAuthData()
{
	if (pData.isEmpty())
	{
		return;
	}
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

	const auto& parseOperations = std::bind(&SelfAuthenticationData::SelfData::parseOperationsAllowedByUser, this, std::placeholders::_1);
	const auto& parsePersonal = std::bind(&SelfAuthenticationData::SelfData::parsePersonalData, this, std::placeholders::_1);
	mValid = parse(doc, QStringLiteral("OperationsAllowedByUser"), parseOperations) && parse(doc, QStringLiteral("PersonalData"), parsePersonal);
}


QString SelfAuthenticationData::SelfData::getValue(SelfAuthData pData) const
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


bool SelfAuthenticationData::SelfData::parseOperationsAllowedByUser(const QDomElement& pElement)
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


bool SelfAuthenticationData::SelfData::parsePersonalData(const QDomElement& pElement)
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


bool SelfAuthenticationData::SelfData::tryToInsertChild(const QDomElement& pElement, SelfAuthData pAuthData)
{
	if (pElement.isNull() || pElement.text().isNull())
	{
		return false;
	}

	mSelfAuthData.insert(pAuthData, pElement.text());
	return true;
}


bool SelfAuthenticationData::SelfData::parse(const QDomDocument& pDoc, const QString& pElementName, const std::function<bool(const QDomElement&)>& pParserFunc)
{
	const QDomNodeList nodeList = pDoc.documentElement().elementsByTagName(pElementName);
	if (nodeList.size() == 0)
	{
		qCritical() << "XML parsing failed. No valid values to parse for:" << pElementName;
		return false;
	}

	return pParserFunc(nodeList.at(0).toElement().firstChildElement());
}


SelfAuthenticationData::OrderedSelfData SelfAuthenticationData::SelfData::getOrderedSelfInfo() const
{
	OrderedSelfData orderedSelfData;

	if (!mValid)
	{
		return orderedSelfData;
	}

	const auto& formatDate = [](const QString& pIn){
				QDateTime dateTime = QDateTime::fromString(pIn, QStringLiteral("yyyy-MM-dd+hh:mm"));
				return LanguageLoader::getInstance().getUsedLocale().toString(dateTime, tr("dd.MM.yyyy"));
			};

	const auto& add = [&](const QString& pKey, const QString& pValue){
#if !defined(Q_OS_ANDROID) && !defined(Q_OS_IOS)
				if (!pKey.isEmpty())
				{
					orderedSelfData << qMakePair(pKey + QLatin1Char(':'), pValue);
				}
				else
#endif
				{
					orderedSelfData << qMakePair(pKey, pValue);
				}
			};

	//fill layout with new data, see 18 Personalausweisgesetz (PAuswG)
	if (!getValue(SelfAuthData::FamilyNames).isNull())
	{
		add(tr("Family name"), getValue(SelfAuthData::FamilyNames));
	}
	if (!getValue(SelfAuthData::BirthName).isNull())
	{
		add(tr("Birth name"), getValue(SelfAuthData::BirthName));
	}
	if (!getValue(SelfAuthData::GivenNames).isNull())
	{
		add(tr("Given name(s)"), getValue(SelfAuthData::GivenNames));
	}
	if (!getValue(SelfAuthData::AcademicTitle).isNull())
	{
		add(tr("Doctoral degree"), getValue(SelfAuthData::AcademicTitle));
	}
	if (!getValue(SelfAuthData::DateOfBirth).isNull())
	{
		add(tr("Date of birth"), formatDate(getValue(SelfAuthData::DateOfBirth)));
	}
	if (!getValue(SelfAuthData::PlaceOfBirth).isNull())
	{
		add(tr("Place of birth"), getValue(SelfAuthData::PlaceOfBirth));
	}
	if (!getValue(SelfAuthData::PlaceOfResidenceNoPlaceInfo).isNull())
	{
		add(tr("Address"), getValue(SelfAuthData::PlaceOfResidenceNoPlaceInfo));
	}
	if (!getValue(SelfAuthData::PlaceOfResidenceStreet).isNull())
	{
		add(getValue(SelfAuthData::PlaceOfResidenceNoPlaceInfo).isNull() ? tr("Address") : QString(), getValue(SelfAuthData::PlaceOfResidenceStreet));
	}
	if (!getValue(SelfAuthData::PlaceOfResidenceZipCode).isNull() || !getValue(SelfAuthData::PlaceOfResidenceCity).isNull())
	{
		add(getValue(SelfAuthData::PlaceOfResidenceStreet).isNull() ? tr("Address") : QString(), getValue(SelfAuthData::PlaceOfResidenceZipCode) + QLatin1Char(' ') + getValue(SelfAuthData::PlaceOfResidenceCity));
	}
	if (!getValue(SelfAuthData::PlaceOfResidenceCountry).isNull())
	{
		add(QString(), getValue(SelfAuthData::PlaceOfResidenceCountry));
	}

	const auto& documentType = getValue(SelfAuthData::DocumentType);
	if (!documentType.isNull())
	{
		add(tr("Document type"), documentType);
	}
	if (!getValue(SelfAuthData::Nationality).isNull())
	{
		add(tr("Nationality"), getValue(SelfAuthData::Nationality));
	}
	if (!getValue(SelfAuthData::ArtisticName).isNull())
	{
		add(tr("Religious / artistic name"), getValue(SelfAuthData::ArtisticName));
	}
	if (!getValue(SelfAuthData::IssuingState).isNull())
	{
		add(tr("Issuing country"), getValue(SelfAuthData::IssuingState));
	}

	// Show "Residence Permit" for eAT- and Test-Cards only
	// AR, AS, AF --> see TR-03127 (v1.16) chapter 3.2.3
	// TA --> Used by Test-Cards
	if (!getValue(SelfAuthData::ResidencePermitI).isNull() && (
				documentType == QLatin1String("AR") ||
				documentType == QLatin1String("AS") ||
				documentType == QLatin1String("AF") ||
				documentType == QLatin1String("TA")))
	{
		add(tr("Residence permit I"), getValue(SelfAuthData::ResidencePermitI));
	}

	return orderedSelfData;
}


#include "moc_SelfAuthenticationData.cpp"
