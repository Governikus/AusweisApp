/*!
 * AccessRoleAndRight.h
 *
 * \brief Defines the AccessRight and AccessRole enum.
 *
 * Note: When using a QHash<AccessRight,...> directly or indirectly (e.g. via QSet),
 * this header must be included before <QHash> (also indirectly via <QSet> or <QList>),
 * or otherwise the complain about the qHash() function for AccessRight not being
 * found.
 *
 * \copyright Copyright (c) 2015 Governikus GmbH & Co. KG
 */

#pragma once

#include "EnumHelper.h"

#include <QList>
#include <QString>


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


inline uint qHash(governikus::AccessRight pAccessRight)
{
	return static_cast<uint>(pAccessRight);
}


defineEnumType(AccessRole,
		CVCA = 3, DV_od = 2, DV_no_f = 1, AT = 0, UNKNOWN = -1
		)


class AccessRoleAndRightsUtil
	: public QObject
{
	Q_OBJECT

	private:
		static QList<AccessRight> mAllRights;
		static QList<AccessRight> mAllDisplayedOrderedRights;
		AccessRoleAndRightsUtil();

	public:
		static const QList<AccessRight>& allDisplayedOrderedRights();
		static const QList<AccessRight>& allRights();
		static QString toDisplayText(AccessRight pRight);
};

}

Q_DECLARE_TYPEINFO(governikus::AccessRight, Q_PRIMITIVE_TYPE);
