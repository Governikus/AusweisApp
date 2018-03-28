/*!
 * \brief Implementation of SecurityInfo
 *
 * \copyright Copyright (c) 2014-2018 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "ASN1TemplateUtil.h"

#include <openssl/asn1.h>
#include <QSharedPointer>


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
#if OPENSSL_VERSION_NUMBER < 0x10100000L
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
	const QSharedPointer<const securityinfo_st> mDelegate;

	SecurityInfo(const QSharedPointer<const securityinfo_st>& pDelegate);
	Q_DISABLE_COPY(SecurityInfo)

	/*
	 * Sub classes must override this method to allow the base class to access
	 * the protocol's ASN1_OBJECT pointer.
	 */
	virtual ASN1_OBJECT * getProtocolObjectIdentifier() const;

	protected:
		SecurityInfo();

	public:
		static QSharedPointer<SecurityInfo> decode(const QByteArray& pBytes)
		{
			if (const auto& delegate = decodeObject<securityinfo_st>(pBytes))
			{
				return QSharedPointer<SecurityInfo>(new SecurityInfo(delegate));
			}
			return QSharedPointer<SecurityInfo>();
		}


		virtual ~SecurityInfo();

		/**
		 * Returns the raw bytes of the protocol OID value, i.e. not the tag and not the length structure, but the value bytes.
		 */
		QByteArray getProtocolValueBytes() const;

		/**
		 * Returns the protocol OID in string representation, e.g. 0.4.0.127.0.7.2.2.4.2.2
		 */
		QByteArray getProtocol() const;
};


DECLARE_ASN1_OBJECT(securityinfo_st)

} /* namespace governikus */
