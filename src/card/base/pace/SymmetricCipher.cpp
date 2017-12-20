/*!
 * \copyright Copyright (c) 2014-2017 Governikus GmbH & Co. KG, Germany
 */

#include "asn1/KnownOIDs.h"
#include "pace/SymmetricCipher.h"

#include <openssl/evp.h>
#include <QLoggingCategory>


using namespace governikus;


Q_DECLARE_LOGGING_CATEGORY(card)


SymmetricCipher::SymmetricCipher(const QByteArray& pPaceAlgorithm, const QByteArray& pKeyBytes)
	: mCtx(nullptr)
	, mCipher(nullptr)
	, mIv()
	, mKeyBytes(pKeyBytes)
{
	using namespace governikus::KnownOIDs;

	if (pPaceAlgorithm == id_PACE::DH::GM_3DES_CBC_CBC || pPaceAlgorithm == id_PACE::DH::IM_3DES_CBC_CBC
			|| pPaceAlgorithm == id_PACE::ECDH::GM_3DES_CBC_CBC || pPaceAlgorithm == id_PACE::ECDH::IM_3DES_CBC_CBC)
	{
		qCCritical(card) << "3DES not supported";
		return;
	}
	else if (pPaceAlgorithm == id_PACE::DH::GM_AES_CBC_CMAC_128 || pPaceAlgorithm == id_PACE::DH::IM_AES_CBC_CMAC_128
			|| pPaceAlgorithm == id_PACE::ECDH::GM_AES_CBC_CMAC_128 || pPaceAlgorithm == id_PACE::ECDH::IM_AES_CBC_CMAC_128)
	{
		mCipher = EVP_aes_128_cbc();
	}
	else if (pPaceAlgorithm == id_PACE::DH::GM_AES_CBC_CMAC_192 || pPaceAlgorithm == id_PACE::DH::IM_AES_CBC_CMAC_192
			|| pPaceAlgorithm == id_PACE::ECDH::GM_AES_CBC_CMAC_192 || pPaceAlgorithm == id_PACE::ECDH::IM_AES_CBC_CMAC_192)
	{
		mCipher = EVP_aes_192_cbc();
	}
	else if (pPaceAlgorithm == id_PACE::DH::GM_AES_CBC_CMAC_256 || pPaceAlgorithm == id_PACE::DH::IM_AES_CBC_CMAC_256
			|| pPaceAlgorithm == id_PACE::ECDH::GM_AES_CBC_CMAC_256 || pPaceAlgorithm == id_PACE::ECDH::IM_AES_CBC_CMAC_256)
	{
		mCipher = EVP_aes_256_cbc();
	}
	else
	{
		qCCritical(card) << "Unknown algorithm: " << pPaceAlgorithm;
		return;
	}

	mIv.fill(0, EVP_CIPHER_iv_length(mCipher));

	if (mKeyBytes.size() != EVP_CIPHER_key_length(mCipher))
	{
		qCCritical(card) << "Error cipher key has wrong length";
		return;
	}

	mCtx = EVP_CIPHER_CTX_new();
	EVP_CIPHER_CTX_init(mCtx);
}


SymmetricCipher::~SymmetricCipher()
{
	// also frees the memory
	if (mCtx != nullptr && !EVP_CIPHER_CTX_cleanup(mCtx))
	{
		qCCritical(card) << "Error on EVP_CIPHER_CTX_cleanup";
	}
}


bool SymmetricCipher::isInitialized()
{
	return mCtx != nullptr && mCipher != nullptr;
}


QByteArray SymmetricCipher::encrypt(const QByteArray& pPlainData)
{
	if (!isInitialized())
	{
		qCCritical(card) << "SymmetricCipher not successfully initialized";
		return QByteArray();
	}

	if (!EVP_EncryptInit_ex(mCtx, mCipher, nullptr, reinterpret_cast<const uchar*>(mKeyBytes.constData()), reinterpret_cast<const uchar*>(mIv.constData())))
	{
		qCCritical(card) << "Error on EVP_EncryptInit_ex";
		return QByteArray();
	}
	EVP_CIPHER_CTX_set_padding(mCtx, 0);

	if (pPlainData.size() % EVP_CIPHER_block_size(EVP_CIPHER_CTX_cipher(mCtx)) != 0)
	{
		qCCritical(card) << "Plain data length is not a multiple of the block size";
		return QByteArray();
	}

	QVector<uchar> cryptogram(pPlainData.size());
	int update_len = 0;
	if (!EVP_EncryptUpdate(mCtx, cryptogram.data(), &update_len, reinterpret_cast<const uchar*>(pPlainData.constData()), pPlainData.size()))
	{
		qCCritical(card) << "Error on EVP_EncryptUpdate";
		return QByteArray();
	}
	int final_len = 0;
	if (!EVP_EncryptFinal_ex(mCtx, cryptogram.data() + update_len, &final_len))
	{
		qCCritical(card) << "Error on EVP_EncryptFinal_ex";
		return QByteArray();
	}
	QByteArray encryptedData;
	encryptedData.append(reinterpret_cast<const char*>(cryptogram.data()), update_len + final_len);

	return encryptedData;
}


bool SymmetricCipher::setIv(const QByteArray& pIv)
{
	Q_ASSERT(mCipher != nullptr);

	if (pIv.size() != EVP_CIPHER_iv_length(mCipher))
	{
		qCCritical(card) << "IV has bad size";
		return false;
	}
	mIv = pIv;
	return true;
}


int SymmetricCipher::getBlockSize() const
{
	Q_ASSERT(mCipher != nullptr);
	return EVP_CIPHER_block_size(mCipher);
}


QByteArray SymmetricCipher::decrypt(const QByteArray& pEncryptedData)
{
	if (!isInitialized())
	{
		qCCritical(card) << "SymmetricCipher not successfully initialized";
		return QByteArray();
	}

	if (!EVP_DecryptInit_ex(mCtx, mCipher, nullptr, reinterpret_cast<const uchar*>(mKeyBytes.constData()), reinterpret_cast<const uchar*>(mIv.constData())))
	{
		qCCritical(card) << "Error on EVP_DecryptInit_ex";
		return QByteArray();
	}
	EVP_CIPHER_CTX_set_padding(mCtx, 0);

	if (pEncryptedData.size() % EVP_CIPHER_block_size(EVP_CIPHER_CTX_cipher(mCtx)) != 0)
	{
		qCCritical(card) << "Encrypted data length is not a multiple of the block size";
		return QByteArray();
	}

	QVector<uchar> plaintext(pEncryptedData.size());
	int update_len = 0;
	if (!EVP_DecryptUpdate(mCtx, plaintext.data(), &update_len, reinterpret_cast<const uchar*>(pEncryptedData.constData()), pEncryptedData.size()))
	{
		qCCritical(card) << "Error on EVP_DecryptUpdate";
		return QByteArray();
	}
	int final_len = 0;
	if (!EVP_DecryptFinal_ex(mCtx, plaintext.data() + update_len, &final_len))
	{
		qCCritical(card) << "Error on EVP_DecryptFinal_ex";
		return QByteArray();
	}
	QByteArray decryptedData;
	decryptedData.append(reinterpret_cast<const char*>(plaintext.data()), update_len + final_len);

	return decryptedData;
}
