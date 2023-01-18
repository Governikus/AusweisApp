/*!
 * \copyright Copyright (c) 2014-2023 Governikus GmbH & Co. KG, Germany
 */

#include "pace/CipherMac.h"

#include "ec/EcUtil.h"

#include <QLoggingCategory>
#include <QScopeGuard>

using namespace governikus;


Q_DECLARE_LOGGING_CATEGORY(card)


CipherMac::CipherMac(const SecurityProtocol& pSecurityProtocol, const QByteArray& pKeyBytes)
#if OPENSSL_VERSION_NUMBER < 0x30000000L
	: mKey(nullptr)
#else
	: mMac(nullptr)
	, mCtx(nullptr)
#endif
{
#if OPENSSL_VERSION_NUMBER < 0x30000000L
	const auto* cipher = pSecurityProtocol.getCipher();
#else
	const auto* cipher = pSecurityProtocol.getCipherString();
#endif
	if (cipher == nullptr)
	{
		return;
	}

#if OPENSSL_VERSION_NUMBER < 0x30000000L
	if (pKeyBytes.size() != EVP_CIPHER_key_length(cipher))
	{
		qCCritical(card) << "Key has wrong size (expected/got):" << EVP_CIPHER_key_length(cipher) << '/' << pKeyBytes.size();
		return;
	}
#endif

#if OPENSSL_VERSION_NUMBER < 0x10101000L || defined(LIBRESSL_VERSION_NUMBER)
	const auto ctx = EcUtil::create(EVP_PKEY_CTX_new_id(EVP_PKEY_CMAC, nullptr));
	if (ctx.isNull() || !EVP_PKEY_keygen_init(ctx.data()))
	{
		qCCritical(card) << "Cannot init ctx";
		return;
	}

	if (EVP_PKEY_CTX_ctrl(ctx.data(), -1, EVP_PKEY_OP_KEYGEN, EVP_PKEY_CTRL_CIPHER, 0, const_cast<EVP_CIPHER*>(cipher)) <= 0)
	{
		qCCritical(card) << "Cannot set cipher";
		return;
	}

	if (EVP_PKEY_CTX_ctrl(ctx.data(), -1, EVP_PKEY_OP_KEYGEN, EVP_PKEY_CTRL_SET_MAC_KEY, pKeyBytes.size(), const_cast<char*>(pKeyBytes.data())) <= 0)
	{
		qCCritical(card) << "Cannot set key";
		return;
	}

	if (!EVP_PKEY_keygen(ctx.data(), &mKey))
	{
		qCCritical(card) << "Cannot generate EVP pkey";
	}

#elif OPENSSL_VERSION_NUMBER < 0x30000000L
	mKey = EVP_PKEY_new_CMAC_key(nullptr, reinterpret_cast<const uchar*>(pKeyBytes.constData()), static_cast<size_t>(pKeyBytes.size()), cipher);

#else

	auto guard = qScopeGuard([this] {
			EVP_MAC_CTX_free(mCtx);
			mCtx = nullptr;

			EVP_MAC_free(mMac);
			mMac = nullptr;
		});

	mMac = EVP_MAC_fetch(nullptr, "cmac", nullptr);
	if (!mMac)
	{
		qCCritical(card) << "Cannot fetch cmac";
		return;
	}

	mCtx = EVP_MAC_CTX_new(mMac);
	if (!mCtx)
	{
		qCCritical(card) << "Cannot create new mac ctx";
		return;
	}

	const OSSL_PARAM params[] = {
		OSSL_PARAM_utf8_string("cipher", const_cast<char*>(cipher), 0),
		OSSL_PARAM_octet_string("key", const_cast<char*>(pKeyBytes.data()), static_cast<size_t>(pKeyBytes.size())),
		OSSL_PARAM_END
	};
	if (!EVP_MAC_CTX_set_params(mCtx, params))
	{
		qCCritical(card) << "Cannot set parameter";
		return;
	}

	guard.dismiss();
#endif
}


CipherMac::~CipherMac()
{
#if OPENSSL_VERSION_NUMBER < 0x30000000L
	EVP_PKEY_free(mKey);
#else
	EVP_MAC_CTX_free(mCtx);
	EVP_MAC_free(mMac);
#endif
}


bool CipherMac::isInitialized() const
{
#if OPENSSL_VERSION_NUMBER < 0x30000000L
	return mKey != nullptr;

#else
	return mMac != nullptr && mCtx != nullptr;

#endif
}


QByteArray CipherMac::generate(const QByteArray& pMessage)
{
	if (!isInitialized())
	{
		qCCritical(card) << "CipherMac not successfully initialized";
		return QByteArray();
	}

#if OPENSSL_VERSION_NUMBER < 0x30000000L
	QSharedPointer<EVP_MD_CTX> ctx(EVP_MD_CTX_create(), [](EVP_MD_CTX* pCtx)
		{
			EVP_MD_CTX_destroy(pCtx);
		});

	if (ctx.isNull() || !EVP_DigestSignInit(ctx.data(), nullptr, nullptr, nullptr, mKey))
	{
		qCCritical(card) << "Cannot init ctx";
		return QByteArray();
	}

	if (!EVP_DigestSignUpdate(ctx.data(), pMessage.constData(), static_cast<size_t>(pMessage.size())))
	{
		qCCritical(card) << "Cannot update cmac";
		return QByteArray();
	}

	QByteArray value(EVP_MAX_MD_SIZE, '\0');
	if (auto ret = static_cast<size_t>(value.size());
			!EVP_DigestSignFinal(ctx.data(), reinterpret_cast<uchar*>(value.data()), &ret))
	{
		qCCritical(card) << "Cannot finalize cmac";
		return QByteArray();
	}

#else
	auto* ctx = EVP_MAC_CTX_dup(mCtx);
	const auto guard = qScopeGuard([ctx] {
			EVP_MAC_CTX_free(ctx);
		});

	if (!EVP_MAC_init(ctx, nullptr, 0, nullptr))
	{
		qCCritical(card) << "Cannot init ctx";
		return QByteArray();
	}

	if (!EVP_MAC_update(ctx, reinterpret_cast<const uchar*>(pMessage.constData()), static_cast<size_t>(pMessage.size())))
	{
		qCCritical(card) << "Cannot update cmac";
		return QByteArray();
	}

	QByteArray value(static_cast<int>(EVP_MAC_CTX_get_mac_size(ctx)), '\0');
	size_t writtenBytes;
	if (!EVP_MAC_final(ctx, reinterpret_cast<uchar*>(value.data()), &writtenBytes, static_cast<size_t>(value.size())))
	{
		qCCritical(card) << "Cannot finalize cmac";
		return QByteArray();
	}

#endif

	// Use only 8 bytes, according to TR 03110 Part 3, A.2.4.2, E.2.2.2
	value.resize(8);
	return value;
}
