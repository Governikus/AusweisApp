/*!
 * \copyright Copyright (c) 2014 Governikus GmbH & Co. KG
 */

#include "asn1/KnownOIDs.h"
#include "pace/CipherMac.h"

#include <QLoggingCategory>
#include <openssl/evp.h>


using namespace governikus;


Q_DECLARE_LOGGING_CATEGORY(card)


CipherMac::CipherMac(const QByteArray& pPaceAlgorithm, const QByteArray& pKeyBytes)
	: mKeyBytes(pKeyBytes)
	, mCtx(nullptr)
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
		qCCritical(card) << "Unknown algorithm: " << pPaceAlgorithm;
		return;
	}
	if (mKeyBytes.size() != EVP_CIPHER_key_length(cipher))
	{
		qCCritical(card) << "Key has wrong size (expected/got):" << EVP_CIPHER_key_length(cipher) << "/" << mKeyBytes.size();
		return;
	}
	mCtx = CMAC_CTX_new();
	if (!CMAC_Init(mCtx, pKeyBytes.constData(), pKeyBytes.size(), cipher, nullptr))
	{
		qCCritical(card) << "Error on CMAC_Init";
	}
}


CipherMac::~CipherMac()
{
	if (mCtx)
	{
		CMAC_CTX_free(mCtx);
	}
}


bool CipherMac::isInitialized()
{
	return mCtx != nullptr;
}


QByteArray CipherMac::generate(const QByteArray& pMessage)
{
	if (!isInitialized())
	{
		qCCritical(card) << "CipherMac not successfully initialized";
		return QByteArray();
	}

	// reset context to allow for multiple use
	if (!CMAC_Init(mCtx, nullptr, 0, nullptr, nullptr))
	{
		qCCritical(card) << "Error on CMAC_Init";
		return QByteArray();
	}

	if (!CMAC_Update(mCtx, pMessage.constData(), pMessage.size()))
	{
		qCCritical(card) << "Error on CMAC_Update";
		return QByteArray();
	}

	size_t mac_len = 0;
	if (!CMAC_Final(mCtx, nullptr, &mac_len))
	{
		qCCritical(card) << "Error on CMAC_Final";
		return QByteArray();
	}

	Q_ASSERT(mac_len > 0);
	QVector<uchar> mac(mac_len);
	if (!CMAC_Final(mCtx, mac.data(), &mac_len))
	{
		qCCritical(card) << "Error on CMAC_Final";
		return QByteArray();
	}

	QByteArray value(reinterpret_cast<const char*>(mac.data()), mac_len);

	// Use only 8 bytes, according to TR 03110 Part 3, A.2.4.2, E.2.2.2
	return value.left(8);
}
