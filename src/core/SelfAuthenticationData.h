/*!
 * SelfAuthenticationData.h
 *
 * \brief Parses self authentication data from XML data and provides its content.
 *
 * \copyright Copyright (c) 2014 Governikus GmbH & Co. KG
 */

#pragma once

#include "EnumHelper.h"

#include <functional>

#include <QDateTime>
#include <QDomElement>
#include <QMap>
#include <QObject>

namespace governikus
{

defineEnumType(SelfAuthDataPermission,
		UNKNOWN /* used for parser only */,
		ALLOWED,
		PROHIBITED,
		NOTONCHIP)

defineEnumType(SelfAuthData,
		UNKNOWN, /* used for parser only */
		DocumentType,
		IssuingState,
		DateOfExpiry,
		GivenNames,
		FamilyNames,
		ArtisticName,
		AcademicTitle,
		BirthName,
		DateOfBirth,
		PlaceOfBirth,
		PlaceOfResidence,
		Nationality,
		ResidencePermitI,
		RestrictedID,
		AgeVerification,
		PlaceVerification,
		/* additional fields */
		PlaceOfResidenceStreet,
		PlaceOfResidenceCity,
		PlaceOfResidenceCountry,
		PlaceOfResidenceZipCode,
		PlaceOfResidenceNoPlaceInfo)


class SelfAuthenticationData
	: public QObject
{
	Q_OBJECT

	private:
		bool mValid;

		QMap<SelfAuthData, SelfAuthDataPermission> mOperationsAllowed;
		QMap<SelfAuthData, QString> mSelfAuthData;

		const QDateTime mDateTime;

		void parse(const QByteArray& pData);
		bool parse(const QDomDocument& pDoc, const QString& pElementName, const std::function<bool(const QDomElement&)>& pParserFunc);
		bool parseOperationsAllowedByUser(const QDomElement& pElement);
		bool parsePersonalData(const QDomElement& pElement);
		bool tryToInsertChild(const QDomElement& pElement, SelfAuthData pAuthData);

	public:
		SelfAuthenticationData(const QByteArray& pData);
		virtual ~SelfAuthenticationData();

		/**
		 * Check if parsing of given data was successful.
		 */
		bool isValid() const;
		QString getValue(SelfAuthData pData) const;
		const QDateTime& getDateTime() const;
};

} /* namespace governikus */
