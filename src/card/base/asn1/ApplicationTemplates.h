/*!
 * \brief Implementation of the ISO 7816 EF.DIR data structure (set of
 *        application template)
 *
 * \copyright Copyright (c) 2021-2023 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "ApplicationTemplate.h"
#include "ChipAuthenticationInfo.h"
#include "PaceInfo.h"

#include <QByteArray>
#include <QSharedPointer>
#include <QVector>


namespace governikus
{

/*!
 * EF.DIR = ApplicationTemplates ::= SET OF ApplicationTemplate

 * defined in ISO 7816-4:2005 8.2.1.1
 */

#if OPENSSL_VERSION_NUMBER < 0x10100000L || defined(LIBRESSL_VERSION_NUMBER)
using ApplicationTemplatesInternal = stack_st_ApplicationTemplate;
#else
using ApplicationTemplatesInternal = STACK_OF(ApplicationTemplate);
#endif

DECLARE_ASN1_FUNCTIONS(ApplicationTemplatesInternal)
DECLARE_ASN1_OBJECT(ApplicationTemplatesInternal)

class ApplicationTemplates
{
	Q_DISABLE_COPY(ApplicationTemplates)
	friend class QSharedPointer<ApplicationTemplates>;

	private:
		const QByteArray mContentBytes;
		const QVector<QSharedPointer<const ApplicationTemplate>> mApplicationTemplates;

		ApplicationTemplates(const QByteArray& pBytes,
				const QVector<QSharedPointer<const ApplicationTemplate>>& pApplicationTemplates);

		static QByteArray buildAsn1LengthField(unsigned int pLength);

	public:
		static QSharedPointer<ApplicationTemplates> decode(const QByteArray& pBytes);

		bool contains(const QByteArray& pIdentifier);

		[[nodiscard]] const QByteArray& getContentBytes() const;
		[[nodiscard]] const QVector<QSharedPointer<const ApplicationTemplate>>& getApplicationTemplates() const;
		[[nodiscard]] int count() const;
};


} // namespace governikus
