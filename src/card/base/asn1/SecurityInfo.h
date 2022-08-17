/*!
 * \brief Implementation of SecurityInfo
 *
 * \copyright Copyright (c) 2014-2022 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "ASN1TemplateUtil.h"
#include "SecurityProtocol.h"

#include <QSharedPointer>
#include <openssl/asn1.h>


namespace governikus
{

/**
 * SecurityInfo ::= SEQUENCE {
 *      protocol OBJECT IDENTIFIER,
 *      requiredData ANY DEFINED BY protocol,
 *      optionalData ANY DEFINED BY protocol OPTIONAL
 * }
 *
 * defined in TR 3110 Part 3
 */
struct securityinfo_st
{
	ASN1_OBJECT* mProtocol;
	ASN1_TYPE* mRequiredData;
	ASN1_TYPE* mOptionalData;
};

DECLARE_ASN1_FUNCTIONS(securityinfo_st)
#if OPENSSL_VERSION_NUMBER < 0x10100000L || defined(LIBRESSL_VERSION_NUMBER)
DECLARE_STACK_OF(securityinfo_st)
#else
DEFINE_STACK_OF(securityinfo_st)
#endif

/*
 * Because OpenSSL's template macro system does not support inheritance,
 * we wrap the securityinfo_st in class SecurityInfo and use this for inheritance.
 */
class SecurityInfo
{
	Q_DISABLE_COPY(SecurityInfo)
	friend class QSharedPointer<SecurityInfo>;

	private:
		const QSharedPointer<const securityinfo_st> mDelegate;

		explicit SecurityInfo(const QSharedPointer<const securityinfo_st>& pDelegate);

		/*
		 * Sub classes must override this method to allow the base class to access
		 * the protocol's ASN1_OBJECT pointer.
		 */
		[[nodiscard]] virtual ASN1_OBJECT* getProtocolObjectIdentifier() const;

	protected:
		SecurityInfo();

	public:
		static QSharedPointer<SecurityInfo> decode(const QByteArray& pBytes)
		{
			if (const auto& delegate = decodeObject<securityinfo_st>(pBytes))
			{
				return QSharedPointer<SecurityInfo>::create(delegate);
			}
			return QSharedPointer<SecurityInfo>();
		}


		virtual ~SecurityInfo() = default;

		/**
		 * Returns the protocol OID.
		 */
		[[nodiscard]] Oid getOid() const;

		/**
		 * Returns the security protocol defined by the OID.
		 */
		[[nodiscard]] SecurityProtocol getProtocol() const;
};


DECLARE_ASN1_OBJECT(securityinfo_st)

} // namespace governikus
