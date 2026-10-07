/**
 * Copyright (c) 2026 Governikus Service GmbH, Germany
 */

#pragma once

#include "EcdhGenericMapping.h"

#include <QByteArray>
#include <QSharedPointer>

#include <openssl/ec.h>
#include <openssl/evp.h>


class test_EcKeyPair;


namespace governikus
{
class EcKeyPair
{
	friend class ::test_EcKeyPair;

	private:
#if OPENSSL_VERSION_NUMBER >= 0x30000000L
		QSharedPointer<EVP_PKEY> mPrivateKey;
#else
		QSharedPointer<EC_KEY> mPrivateKey;
#endif
		QByteArray mPublicKey;

#if OPENSSL_VERSION_NUMBER >= 0x30000000L
		static QByteArray multiply(const QByteArray& pPublicKey, const QSharedPointer<EVP_PKEY>& pKey);
#else
		static QByteArray multiply(const QByteArray& pPublicKey, const QSharedPointer<EC_KEY>& pKey);
#endif

	public:
		EcKeyPair();
		explicit EcKeyPair(const EcdhGenericMapping& pMapping);
		explicit EcKeyPair(const QByteArray& pPrivateKey);

		[[nodiscard]] QByteArray getPublicKey(bool pCompressed = false) const;
		[[nodiscard]] QByteArray getSharedSecret(const QByteArray& pRemoteKey) const;
};

} // namespace governikus
