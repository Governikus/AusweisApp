/*!
 * \copyright Copyright (c) 2014-2023 Governikus GmbH & Co. KG, Germany
 */

#include "pace/SymmetricCipher.h"

#include <QLoggingCategory>
#include <openssl/evp.h>


using namespace governikus;


Q_DECLARE_LOGGING_CATEGORY(card)


SymmetricCipher::SymmetricCipher(const SecurityProtocol& pSecurityProtocol, const QByteArray& pKeyBytes)
	: mCtx(nullptr)
	, mCipher(pSecurityProtocol.getCipher())
	, mIv()
	, mKeyBytes(pKeyBytes)
{
	if (!mCipher)
	{
		qCCritical(card) << "Unknown algorithm:" << pSecurityProtocol;
		return;
	}

	mIv.fill(0, EVP_CIPHER_iv_length(mCipher));

	if (mKeyBytes.size() != EVP_CIPHER_key_length(mCipher))
	{
		qCCritical(card) << "Error cipher key has wrong length";
		return;
	}

	mCtx = EVP_CIPHER_CTX_new();
#if OPENSSL_VERSION_NUMBER < 0x10100000L
	EVP_CIPHER_CTX_init(mCtx);
#else
	EVP_CIPHER_CTX_reset(mCtx);
#endif
}


SymmetricCipher::~SymmetricCipher()
{
#if OPENSSL_VERSION_NUMBER < 0x10100000L
	EVP_CIPHER_CTX_cleanup(mCtx);
#else
	EVP_CIPHER_CTX_reset(mCtx);
#endif

	EVP_CIPHER_CTX_free(mCtx);
}


bool SymmetricCipher::isInitialized() const
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
