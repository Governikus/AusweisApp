/**
 * Copyright (c) 2014-2025 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "ChipAuthenticationInfo.h"
#include "MobileEIDTypeInfo.h"
#include "PaceInfo.h"
#include "SecurityInfo.h"

#include <QByteArray>
#include <QList>
#include <QSharedPointer>


namespace governikus
{
using securityinfos_st = struct stack_st_securityinfo_st;

DECLARE_ASN1_FUNCTIONS(securityinfos_st)
DECLARE_ASN1_OBJECT(securityinfos_st)

/*!
 * SecurityInfos ::= SET OF SecurityInfo

 * defined in TR 3110 Part 3
 */
class SecurityInfos
{
	Q_DISABLE_COPY(SecurityInfos)
	friend class QSharedPointer<SecurityInfos>;

	private:
		const QByteArray mContentBytes;
		const QList<QSharedPointer<const SecurityInfo>> mSecurityInfos;
		const QList<QSharedPointer<const PaceInfo>> mPaceInfos;
		const QList<QSharedPointer<const ChipAuthenticationInfo>> mChipAuthenticationInfos;
		const QSharedPointer<const MobileEIDTypeInfo> mMobileEIDTypeInfo;

		SecurityInfos(const QByteArray& pBytes,
				const QList<QSharedPointer<const SecurityInfo>>& pSecurityInfos,
				const QList<QSharedPointer<const PaceInfo>>& pPaceInfos,
				const QList<QSharedPointer<const ChipAuthenticationInfo>>& pChipAuthenticationInfos,
				const QSharedPointer<const MobileEIDTypeInfo>& pMobileEIDTypeInfo);

	public:
		static QSharedPointer<SecurityInfos> fromHex(const QByteArray& pHexString);
		static QSharedPointer<SecurityInfos> decode(const QByteArray& pBytes);

		[[nodiscard]] const QByteArray& getContentBytes() const;
		[[nodiscard]] const QList<QSharedPointer<const SecurityInfo>>& getSecurityInfos() const;
		[[nodiscard]] const QList<QSharedPointer<const PaceInfo>>& getPaceInfos() const;
		[[nodiscard]] const QList<QSharedPointer<const ChipAuthenticationInfo>>& getChipAuthenticationInfos() const;
		[[nodiscard]] const QSharedPointer<const MobileEIDTypeInfo>& getMobileEIDTypeInfo() const;
};


/*!
 * EFCardAccess ::= SecurityInfos
 *
 * defined in TR 3110 Part 3
 */
using EFCardAccess = SecurityInfos;


} // namespace governikus
