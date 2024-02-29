/**
 * Copyright (c) 2014-2024 Governikus GmbH & Co. KG, Germany
 */

/*!
 * \brief Defines the AccessRight and AccessRole enum.
 *
 * Note: When using a QHash<AccessRight,...> directly or indirectly (e.g. via QSet),
 * this header must be included before <QHash> (also indirectly via <QSet> or <QList>),
 * or otherwise they complain about the qHash() function for AccessRight not being
 * found.
 */

#pragma once

#include "EnumHelper.h"

#include <QCoreApplication>
#include <QList>
#include <QString>
#include <functional>


namespace governikus
{

defineTypedEnumType(AccessRight, uint,
		WRITE_DG17 = 37,
		WRITE_DG18 = 36,
		WRITE_DG19 = 35,
		WRITE_DG20 = 34,
		WRITE_DG21 = 33,
		RFU_32 = 32, /* Reserved for Future Use */
		RFU_31 = 31, /* Reserved for Future Use */
		RFU_30 = 30, /* Reserved for Future Use */
		RFU_29 = 29, /* Reserved for Future Use */
		READ_DG21 = 28,
		READ_DG20 = 27,
		READ_DG19 = 26,
		READ_DG18 = 25,
		READ_DG17 = 24,
		READ_DG16 = 23,
		READ_DG15 = 22,
		READ_DG14 = 21,
		READ_DG13 = 20,
		READ_DG12 = 19,
		READ_DG11 = 18,
		READ_DG10 = 17,
		READ_DG09 = 16,
		READ_DG08 = 15,
		READ_DG07 = 14,
		READ_DG06 = 13,
		READ_DG05 = 12,
		READ_DG04 = 11,
		READ_DG03 = 10,
		READ_DG02 = 9,
		READ_DG01 = 8,

		INSTALL_QUAL_CERT = 7,
		INSTALL_CERT = 6,
		PIN_MANAGEMENT = 5,
		CAN_ALLOWED = 4,
		PRIVILEGED_TERMINAL = 3,
		RESTRICTED_IDENTIFICATION = 2,
		COMMUNITY_ID_VERIFICATION = 1,
		AGE_VERIFICATION = 0
		)

// do NOT change any key/value as this is used as an unique identifier!
// if you add entries you need to modify the SDK documentation.
#define TYPE(x) static_cast<std::underlying_type_t<AccessRight>>(x)
defineTypedEnumType(AccessRightNames, std::underlying_type_t<AccessRight>,
		ResidencePermitII = TYPE(AccessRight::READ_DG20),
		ResidencePermitI = TYPE(AccessRight::READ_DG19),
		CommunityID = TYPE(AccessRight::READ_DG18),
		Address = TYPE(AccessRight::READ_DG17),
		BirthName = TYPE(AccessRight::READ_DG13),
		Nationality = TYPE(AccessRight::READ_DG10),
		PlaceOfBirth = TYPE(AccessRight::READ_DG09),
		DateOfBirth = TYPE(AccessRight::READ_DG08),
		DoctoralDegree = TYPE(AccessRight::READ_DG07),
		ArtisticName = TYPE(AccessRight::READ_DG06),
		FamilyName = TYPE(AccessRight::READ_DG05),
		GivenNames = TYPE(AccessRight::READ_DG04),
		ValidUntil = TYPE(AccessRight::READ_DG03),
		IssuingCountry = TYPE(AccessRight::READ_DG02),
		DocumentType = TYPE(AccessRight::READ_DG01),
		CanAllowed = TYPE(AccessRight::CAN_ALLOWED),
		Pseudonym = TYPE(AccessRight::RESTRICTED_IDENTIFICATION),
		AddressVerification = TYPE(AccessRight::COMMUNITY_ID_VERIFICATION),
		AgeVerification = TYPE(AccessRight::AGE_VERIFICATION),
		WriteAddress = TYPE(AccessRight::WRITE_DG17),
		WriteCommunityID = TYPE(AccessRight::WRITE_DG18),
		WriteResidencePermitI = TYPE(AccessRight::WRITE_DG19),
		WriteResidencePermitII = TYPE(AccessRight::WRITE_DG20),
		PinManagement = TYPE(AccessRight::PIN_MANAGEMENT)
		)
#undef TYPE

[[nodiscard]] inline uint qHash(governikus::AccessRight pAccessRight)
{
	return static_cast<uint>(pAccessRight);
}


defineEnumType(AccessRole,
		CVCA = 3, DV_od = 2, DV_no_f = 1, AT = 0, UNKNOWN = -1
		)


class AccessRoleAndRightsUtil
{
	Q_DECLARE_TR_FUNCTIONS(governikus::AccessRoleAndRightsUtil)

	public:
		enum JoinRight
		{
			READ = 0x1,
			WRITE = 0x2,
			READWRITE = 0x3
		};
		Q_DECLARE_FLAGS(JoinRights, JoinRight)

	private:
		static QList<AccessRight> mAllRights;
		static QList<AccessRight> mAllDisplayedOrderedRights;

		static bool fromTechnicalName(const char* const pStr, const std::function<void(AccessRight)>& pFunc);
		static QStringList fromTechnicalName(const QStringList& pStr, JoinRights pJoinRight);

		AccessRoleAndRightsUtil() = delete;
		~AccessRoleAndRightsUtil() = delete;

	public:
		static const QList<AccessRight>& allDisplayedOrderedRights();
		static const QList<AccessRight>& allRights();
		[[nodiscard]] static bool isWriteAccessRight(AccessRight pRight);
		static QString toDisplayText(AccessRight pRight);
		static QLatin1String toTechnicalName(AccessRight pRight);
		static bool fromTechnicalName(const QString& pStr, const std::function<void(AccessRight)>& pFunc);
		static QString joinFromTechnicalName(const QStringList& pStr,
				JoinRights pJoinRight = JoinRight::READWRITE,
				const QString& pJoin = QStringLiteral(", "));
};

Q_DECLARE_OPERATORS_FOR_FLAGS(AccessRoleAndRightsUtil::JoinRights)

} // namespace governikus

Q_DECLARE_TYPEINFO(governikus::AccessRight, Q_PRIMITIVE_TYPE);
