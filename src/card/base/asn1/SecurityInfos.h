/*!
 * SecurityInfos.h
 *
 * \brief Implementation of SecurityInfos
 *
 * \copyright Copyright (c) 2015 Governikus GmbH & Co. KG
 */

#pragma once

#include "ChipAuthenticationInfo.h"
#include "PACEInfo.h"
#include "SecurityInfo.h"

#include <QByteArray>
#include <QSharedPointer>
#include <QVector>


namespace governikus
{
typedef struct stack_st_securityinfo_st securityinfos_st;

DECLARE_ASN1_FUNCTIONS(securityinfos_st)
DECLARE_ASN1_OBJECT(securityinfos_st)

/*!
 * SecurityInfos ::= SET OF SecurityInfo

 * defined in TR 3110 Part 3
 */
class SecurityInfos
{
	const QByteArray mContentBytes;
	const QVector<QSharedPointer<const SecurityInfo> > mSecurityInfos;
	const QVector<QSharedPointer<const PACEInfo> > mPACEInfos;
	const QVector<QSharedPointer<const ChipAuthenticationInfo> > mChipAuthenticationInfos;

	SecurityInfos(const QByteArray& pBytes,
			const QVector<QSharedPointer<const SecurityInfo> >& pSecurityInfos,
			const QVector<QSharedPointer<const PACEInfo> >& pPACEInfos,
			const QVector<QSharedPointer<const ChipAuthenticationInfo> >& pChipAuthenticationInfos);
	Q_DISABLE_COPY(SecurityInfos)

	public:
		static QSharedPointer<SecurityInfos> fromHex(const QByteArray& pHexString);
		static QSharedPointer<SecurityInfos> decode(const QByteArray& pBytes);

		const QByteArray& getContentBytes() const;
		const QVector<QSharedPointer<const SecurityInfo> >& getSecurityInfos() const;
		const QVector<QSharedPointer<const PACEInfo> >& getPACEInfos() const;
		const QVector<QSharedPointer<const ChipAuthenticationInfo> >& getChipAuthenticationInfos() const;
};


/*!
 * EFCardAccess ::= SecurityInfos
 *
 * defined in TR 3110 Part 3
 */
typedef SecurityInfos EFCardAccess;


} /* namespace governikus */
