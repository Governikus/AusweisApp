/*!
 * \copyright Copyright (c) 2014-2020 Governikus GmbH & Co. KG, Germany
 */

#include "pace/CipherMac.h"

#include "asn1/KnownOIDs.h"
#include "ec/EcUtil.h"

#include <QLoggingCategory>
#include <QVarLengthArray>

using namespace governikus;


Q_DECLARE_LOGGING_CATEGORY(card)


CipherMac::CipherMac(const QByteArray& pPaceAlgorithm, const QByteArray& pKeyBytes)
	: mKey(nullptr)
{
	using namespace governikus::KnownOIDs;

	const EVP_CIPHER* cipher;
	if (pPaceAlgorithm == id_PACE::DH::GM_3DES_CBC_CBC || pPaceAlgorithm == id_PACE::DH::IM_3DES_CBC_CBC || pPaceAlgorithm == id_PACE::ECDH::GM_3DES_CBC_CBC
			|| pPaceAlgorithm == id_PACE::ECDH::IM_3DES_CBC_CBC)
	{
		qCCritical(card) << "3DES not supported";
		return;
	}
	else if (pPaceAlgorithm == id_PACE::DH::GM_AES_CBC_CMAC_128 || pPaceAlgorithm == id_PACE::DH::IM_AES_CBC_CMAC_128 || pPaceAlgorithm == id_PACE::ECDH::GM_AES_CBC_CMAC_128
			|| pPaceAlgorithm == id_PACE::ECDH::IM_AES_CBC_CMAC_128)
	{
		cipher = EVP_aes_128_cbc();
	}
	else if (pPaceAlgorithm == id_PACE::DH::GM_AES_CBC_CMAC_192 || pPaceAlgorithm == id_PACE::DH::IM_AES_CBC_CMAC_192 || pPaceAlgorithm == id_PACE::ECDH::GM_AES_CBC_CMAC_192
			|| pPaceAlgorithm == id_PACE::ECDH::IM_AES_CBC_CMAC_192)
	{
		cipher = EVP_aes_192_cbc();
	}
	else if (pPaceAlgorithm == id_PACE::DH::GM_AES_CBC_CMAC_256 || pPaceAlgorithm == id_PACE::DH::IM_AES_CBC_CMAC_256 || pPaceAlgorithm == id_PACE::ECDH::GM_AES_CBC_CMAC_256
			|| pPaceAlgorithm == id_PACE::ECDH::IM_AES_CBC_CMAC_256)
	{
		cipher = EVP_aes_256_cbc();
	}
	else
	{
		qCCritical(card) << "Unknown algorithm:" << pPaceAlgorithm;
		return;
	}


	if (pKeyBytes.size() != EVP_CIPHER_key_length(cipher))
	{
		qCCritical(card) << "Key has wrong size (expected/got):" << EVP_CIPHER_key_length(cipher) << '/' << pKeyBytes.size();
		return;
	}

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

	if (EVP_PKEY_CTX_ctrl(ctx.data(), -1, EVP_PKEY_OP_KEYGEN, EVP_PKEY_CTRL_SET_MAC_KEY, pKeyBytes.size(), pKeyBytes.data()) <= 0)
	{
		qCCritical(card) << "Cannot set key";
		return;
	}

	if (!EVP_PKEY_keygen(ctx.data(), &mKey))
	{
		qCCritical(card) << "Cannot generate EVP pkey";
	}
#else
	mKey = EVP_PKEY_new_CMAC_key(nullptr, reinterpret_cast<const uchar*>(pKeyBytes.constData()), static_cast<size_t>(pKeyBytes.size()), cipher);
#endif
}


CipherMac::~CipherMac()
{
	EVP_PKEY_free(mKey);
}


bool CipherMac::isInitialized() const
{
	return mKey != nullptr;
}


QByteArray CipherMac::generate(const QByteArray& pMessage)
{
	if (!isInitialized())
	{
		qCCritical(card) << "CipherMac not successfully initialized";
		return QByteArray();
	}

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
	size_t ret = static_cast<size_t>(value.size());
	if (!EVP_DigestSignFinal(ctx.data(), reinterpret_cast<uchar*>(value.data()), &ret))
	{
		qCCritical(card) << "Cannot finalize cmac";
		return QByteArray();
	}

	// Use only 8 bytes, according to TR 03110 Part 3, A.2.4.2, E.2.2.2
	value.resize(8);
	return value;
}
