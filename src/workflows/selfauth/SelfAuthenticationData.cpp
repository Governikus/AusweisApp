/**
 * Copyright (c) 2014-2023 Governikus GmbH & Co. KG, Germany
 */

#include "SelfAuthenticationData.h"

#include "LanguageLoader.h"

#include <QJsonDocument>
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
	qCDebug(secure).noquote() << "parsing data:\n" << pData;

	mValid = parse(pData);
}


bool SelfAuthenticationData::SelfData::parse(const QByteArray& pData)
{
	QJsonParseError jsonError {};
	const auto& json = QJsonDocument::fromJson(pData, &jsonError);
	if (jsonError.error != QJsonParseError::NoError)
	{
		qDebug() << "JSON parsing failed:" << jsonError.errorString();
		return false;
	}

	const auto& obj = json.object();
	return parseOperationsAllowedByUser(obj) && parsePersonalData(obj);
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

		//: INFO ALL_PLATFORMS The requested data is not stored on this chip's generation.
		return tr("This data has not been stored in this chip generation.");
	}

	return QString();
}


bool SelfAuthenticationData::SelfData::parseOperationsAllowedByUser(const QJsonObject& pObject)
{
	const auto& allowedOperationsValue = pObject.value(QLatin1String("OperationsAllowedByUser"));
	if (!allowedOperationsValue.isObject())
	{
		qDebug() << "JSON value \"OperationsAllowedByUser\" is of wrong type:" << allowedOperationsValue.type();
		return false;
	}

	const auto& allowedOperations = allowedOperationsValue.toObject();
	const auto& keys = allowedOperations.keys();
	for (const auto& entry : keys)
	{
		auto authData = Enum<SelfAuthData>::fromString(entry, SelfAuthData::UNKNOWN);
		if (authData == SelfAuthData::UNKNOWN)
		{
			qWarning() << "SelfAuthData is unknown:" << entry;
			continue;
		}

		const auto tagName = allowedOperations.value(entry).toString();
		auto permission = Enum<SelfAuthDataPermission>::fromString(tagName, SelfAuthDataPermission::UNKNOWN);
		if (permission == SelfAuthDataPermission::UNKNOWN)
		{
			qWarning() << "SelfAuthDataPermission is unknown:" << entry << '|' << tagName;
			continue;
		}

		mOperationsAllowed.insert(authData, permission);
	}

	return true;
}


bool SelfAuthenticationData::SelfData::parsePersonalData(const QJsonObject& pObject)
{
	const auto& personalDataValue = pObject.value(QLatin1String("PersonalData"));
	if (!personalDataValue.isObject())
	{
		qDebug() << "JSON value \"PersonalData\" is of wrong type:" << personalDataValue.type();
		return false;
	}

	const auto& personalData = personalDataValue.toObject();
	const auto& keys = personalData.keys();
	for (const auto& entry : keys)
	{
		const auto subvalue = [&personalData, &entry](const char* const pValue){
					return personalData.value(entry).toObject().value(QLatin1String(pValue));
				};

		switch (const auto& authData = Enum<SelfAuthData>::fromString(entry, SelfAuthData::UNKNOWN))
		{
			case SelfAuthData::UNKNOWN:
				qWarning() << "PersonalData is unknown:" << entry;
				break;

			case SelfAuthData::PlaceOfBirth:
				tryToInsertChild(subvalue("FreetextPlace"), authData);
				break;

			case SelfAuthData::PlaceOfResidence:
			{
				if (tryToInsertChild(subvalue("NoPlaceInfo"), SelfAuthData::PlaceOfResidenceNoPlaceInfo))
				{
					mOperationsAllowed.insert(SelfAuthData::PlaceOfResidenceNoPlaceInfo, mOperationsAllowed.value(authData));
					break;
				}

				parseStructuredPlace(subvalue("StructuredPlace").toObject(), mOperationsAllowed.value(authData));
				break;
			}

			default:
				tryToInsertChild(personalData.value(entry), authData);
		}
	}

	return true;
}


void SelfAuthenticationData::SelfData::parseStructuredPlace(const QJsonObject& pObject, SelfAuthDataPermission pPermission)
{
	static const QMap<QString, SelfAuthData> placeInfo = {
		{QStringLiteral("City"), SelfAuthData::PlaceOfResidenceCity},
		{QStringLiteral("Country"), SelfAuthData::PlaceOfResidenceCountry},
		{QStringLiteral("Street"), SelfAuthData::PlaceOfResidenceStreet},
		{QStringLiteral("ZipCode"), SelfAuthData::PlaceOfResidenceZipCode}
	};

	for (auto iter = placeInfo.constBegin(); iter != placeInfo.constEnd(); ++iter)
	{
		if (tryToInsertChild(pObject.value(iter.key()), iter.value()))
		{
			mOperationsAllowed.insert(iter.value(), pPermission);
		}
	}
}


bool SelfAuthenticationData::SelfData::tryToInsertChild(const QJsonValue& pValue, SelfAuthData pAuthData)
{
	if (pValue.isString())
	{
		mSelfAuthData.insert(pAuthData, pValue.toString());
		return true;
	}

	return false;
}


void SelfAuthenticationData::SelfData::add(OrderedSelfData& pSelfData, const QString& pGroupName, const QString& pGroupValue) const
{
	if (pGroupValue.isNull())
	{
		return;
	}

	pSelfData << qMakePair(pGroupName, pGroupValue);
}


void SelfAuthenticationData::SelfData::addAddress(OrderedSelfData& pSelfData) const
{
	if (!getValue(SelfAuthData::PlaceOfResidenceNoPlaceInfo).isNull())
	{
		//: LABEL ALL_PLATFORMS
		add(pSelfData, tr("Address"), getValue(SelfAuthData::PlaceOfResidenceNoPlaceInfo));
	}
	if (!getValue(SelfAuthData::PlaceOfResidenceStreet).isNull())
	{
		//: LABEL ALL_PLATFORMS
		add(pSelfData, getValue(SelfAuthData::PlaceOfResidenceNoPlaceInfo).isNull() ? tr("Address") : QString(), getValue(SelfAuthData::PlaceOfResidenceStreet));
	}
	if (!getValue(SelfAuthData::PlaceOfResidenceZipCode).isNull() || !getValue(SelfAuthData::PlaceOfResidenceCity).isNull())
	{
		//: LABEL ALL_PLATFORMS
		add(pSelfData, getValue(SelfAuthData::PlaceOfResidenceStreet).isNull() ? tr("Address") : QString(), getValue(SelfAuthData::PlaceOfResidenceZipCode) + QLatin1Char(' ') + getValue(SelfAuthData::PlaceOfResidenceCity));
	}
	if (!getValue(SelfAuthData::PlaceOfResidenceCountry).isNull())
	{
		add(pSelfData, QString(), getValue(SelfAuthData::PlaceOfResidenceCountry));
	}
}


QString SelfAuthenticationData::SelfData::formatDate(const QString& pDate)
{
	static const QVector<QPair<QString, QLatin1String>> formattingPattern({
	            //: LABEL ALL_PLATFORMS Date format according to https://doc.qt.io/qt/qdate.html#toString
				qMakePair(QStringLiteral("yyyy-MM-dd+hh:mm"), QLatin1String(QT_TR_NOOP("dd.MM.yyyy"))),
	            //: LABEL ALL_PLATFORMS Date format according to https://doc.qt.io/qt/qdate.html#toString with unknown day
				qMakePair(QStringLiteral("yyyy-MM"), QLatin1String(QT_TR_NOOP("xx.MM.yyyy"))),
	            //: LABEL ALL_PLATFORMS Date format according to https://doc.qt.io/qt/qdate.html#toString with unknown day and month
				qMakePair(QStringLiteral("yyyy"), QLatin1String(QT_TR_NOOP("xx.xx.yyyy"))),
			});

	for (const auto& entry : formattingPattern)
	{
		const QDateTime dateTime = QDateTime::fromString(pDate, entry.first);
		if (dateTime.isValid())
		{
			return LanguageLoader::getInstance().getUsedLocale().toString(dateTime, tr(entry.second.data()));
		}
	}

	return QString();
}


SelfAuthenticationData::OrderedSelfData SelfAuthenticationData::SelfData::getOrderedSelfInfo() const
{
	OrderedSelfData orderedSelfData;

	if (!mValid)
	{
		return orderedSelfData;
	}

	//fill layout with new data, see 18 Personalausweisgesetz (PAuswG)
	//: LABEL ALL_PLATFORMS
	add(orderedSelfData, tr("Family name"), getValue(SelfAuthData::FamilyNames));
	//: LABEL ALL_PLATFORMS
	add(orderedSelfData, tr("Birth name"), getValue(SelfAuthData::BirthName));
	//: LABEL ALL_PLATFORMS
	add(orderedSelfData, tr("Given name(s)"), getValue(SelfAuthData::GivenNames));
	//: LABEL ALL_PLATFORMS
	add(orderedSelfData, tr("Doctoral degree"), getValue(SelfAuthData::AcademicTitle));
	//: LABEL ALL_PLATFORMS
	add(orderedSelfData, tr("Date of birth"), formatDate(getValue(SelfAuthData::DateOfBirth)));
	//: LABEL ALL_PLATFORMS
	add(orderedSelfData, tr("Place of birth"), getValue(SelfAuthData::PlaceOfBirth));
	addAddress(orderedSelfData);
	//: LABEL ALL_PLATFORMS
	add(orderedSelfData, tr("Document type"), getValue(SelfAuthData::DocumentType));
	//: LABEL ALL_PLATFORMS
	add(orderedSelfData, tr("Nationality"), getValue(SelfAuthData::Nationality));
	//: LABEL ALL_PLATFORMS
	add(orderedSelfData, tr("Religious / artistic name"), getValue(SelfAuthData::ArtisticName));
	//: LABEL ALL_PLATFORMS
	add(orderedSelfData, tr("Issuing country"), getValue(SelfAuthData::IssuingState));

	const auto& documentType = getValue(SelfAuthData::DocumentType);
	// Show "Residence Permit" for eAT- and Test-Cards only
	// AR, AS, AF --> see TR-03127 (v1.16) chapter 3.2.3
	// TA --> Used by Test-Cards
	if (!getValue(SelfAuthData::ResidencePermitI).isNull() && (
				documentType == QLatin1String("AR") ||
				documentType == QLatin1String("AS") ||
				documentType == QLatin1String("AF") ||
				documentType == QLatin1String("TA")))
	{
		//: LABEL ALL_PLATFORMS
		add(orderedSelfData, tr("Residence permit I"), getValue(SelfAuthData::ResidencePermitI));
	}

	//: LABEL ALL_PLATFORMS
	add(orderedSelfData, tr("Date of expiry"), formatDate(getValue(SelfAuthData::DateOfExpiry)));

	return orderedSelfData;
}


#include "moc_SelfAuthenticationData.cpp"
