/*!
 * SecurityInfos.h
 *
 * \brief Implementation of SecurityInfos
 *
 * \copyright Copyright (c) 2015 Governikus GmbH & Co. KG
 */

#pragma once

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
	const QVector<QSharedPointer<SecurityInfo> > mSecurityInfos;

	SecurityInfos(const QByteArray& pBytes, const QVector<QSharedPointer<SecurityInfo> >& pSecurityInfos);
	Q_DISABLE_COPY(SecurityInfos)

	public:
		static QSharedPointer<SecurityInfos> fromHex(const QByteArray& pHexString);
		static QSharedPointer<SecurityInfos> decode(const QByteArray& pBytes);

		template<typename T> QVector<QSharedPointer<T> > getSecurityInfos() const
		{
			QVector<QSharedPointer<T> > list;
			for (const auto& elem : mSecurityInfos)
			{
				const auto castedElem = elem.dynamicCast<T>();
				if (castedElem.isNull())
				{
					// cast failed, e.g. it's not of type T, so skip this element
					continue;
				}
				list += castedElem;
			}
			return list;
		}


		const QByteArray& getContentBytes() const;
};


/*!
 * EFCardAccess ::= SecurityInfos
 *
 * defined in TR 3110 Part 3
 */
typedef SecurityInfos EFCardAccess;


} /* namespace governikus */
