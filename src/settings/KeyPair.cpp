/*
 * \copyright Copyright (c) 2017-2021 Governikus GmbH & Co. KG, Germany
 */


#include "KeyPair.h"

#include "Randomizer.h"

#include <openssl/bio.h>
#include <openssl/pem.h>
#include <openssl/rsa.h>
#include <openssl/x509.h>

#include <QCoreApplication>
#include <QLoggingCategory>
#include <QScopedPointer>

using namespace governikus;

Q_DECLARE_LOGGING_CATEGORY(settings)

namespace
{
struct OpenSslCustomDeleter
{
	static inline void cleanup(EVP_PKEY* pData)
	{
		EVP_PKEY_free(pData);
	}


	static inline void cleanup(EVP_PKEY_CTX* pData)
	{
		EVP_PKEY_CTX_free(pData);
	}


	static inline void cleanup(BIO* pData)
	{
		BIO_free(pData);
	}


	static inline void cleanup(X509_NAME* pData)
	{
		X509_NAME_free(pData);
	}


};
} // namespace

KeyPair::KeyPair(const QSslKey& pKey, const QSslCertificate& pCert)
	: mKey(pKey)
	, mCertificate(pCert)
{
}


KeyPair KeyPair::generate()
{
	if (!Randomizer::getInstance().isSecureRandom())
	{
		qCCritical(settings) << "Cannot get enough entropy";
		return KeyPair();
	}

	auto pkey = createKey();
	if (pkey)
	{
		auto cert = createCertificate(pkey);
		if (cert)
		{
			return KeyPair(QSslKey(Qt::HANDLE(pkey), QSsl::PrivateKey), QSslCertificate(rewriteCertificate(cert.data())));
		}
		else
		{
			OpenSslCustomDeleter::cleanup(pkey);
		}
	}

	return KeyPair();
}


const QSslKey& KeyPair::getKey() const
{
	return mKey;
}


const QSslCertificate& KeyPair::getCertificate() const
{
	return mCertificate;
}


EVP_PKEY* KeyPair::createKey()
{
	QScopedPointer<EVP_PKEY_CTX, OpenSslCustomDeleter> pkeyCtx(EVP_PKEY_CTX_new_id(EVP_PKEY_RSA, nullptr));

	if (pkeyCtx.isNull())
	{
		qCCritical(settings) << "Cannot create EVP_PKEY_CTX";
		return nullptr;
	}

	if (!EVP_PKEY_keygen_init(pkeyCtx.data()))
	{
		qCCritical(settings) << "Cannot init rsa key ctx";
		return nullptr;
	}

	if (!EVP_PKEY_CTX_set_rsa_keygen_bits(pkeyCtx.data(), 2048))
	{
		qCCritical(settings) << "Cannot generate rsa key bits";
		return nullptr;
	}

	EVP_PKEY* pkey = nullptr;
	if (!EVP_PKEY_keygen(pkeyCtx.data(), &pkey))
	{
		qCCritical(settings) << "Cannot generate rsa key";
		return nullptr;
	}

	return pkey;
}


QSharedPointer<X509> KeyPair::createCertificate(EVP_PKEY* pPkey)
{
	QSharedPointer<X509> x509(X509_new(), &X509_free);
	if (x509.isNull())
	{
		qCCritical(settings) << "Cannot create X509 structure";
		return nullptr;
	}

	auto& randomizer = Randomizer::getInstance().getGenerator();
	std::uniform_int_distribution<long> uni_long(1);
	std::uniform_int_distribution<qulonglong> uni_qulonglong(1);

	#if OPENSSL_VERSION_NUMBER < 0x10100000L
		#define X509_getm_notBefore X509_get_notBefore
		#define X509_getm_notAfter X509_get_notAfter
	#endif

	ASN1_INTEGER_set(X509_get_serialNumber(x509.data()), uni_long(randomizer));
	// see: https://tools.ietf.org/html/rfc5280#section-4.1.2.5
	ASN1_TIME_set_string(X509_getm_notBefore(x509.data()), "19700101000000Z");
	ASN1_TIME_set_string(X509_getm_notAfter(x509.data()), "99991231235959Z");
	X509_set_pubkey(x509.data(), pPkey);

	auto randomSerial = QByteArray::number(uni_qulonglong(randomizer));
	QScopedPointer<X509_NAME, OpenSslCustomDeleter> name(X509_NAME_dup(X509_get_subject_name(x509.data())));
	X509_NAME_add_entry_by_txt(name.data(), "CN", MBSTRING_ASC,
			reinterpret_cast<const uchar*>(QCoreApplication::applicationName().toLatin1().constData()), -1, -1, 0);
	X509_NAME_add_entry_by_txt(name.data(), "serialNumber", MBSTRING_ASC,
			reinterpret_cast<const uchar*>(randomSerial.constData()), -1, -1, 0);
	X509_set_subject_name(x509.data(), name.data());
	X509_set_issuer_name(x509.data(), name.data());

	if (!X509_sign(x509.data(), pPkey, EVP_sha256()))
	{
		qCCritical(settings) << "Cannot sign certificate";
		return nullptr;
	}

	return x509;
}


QByteArray KeyPair::rewriteCertificate(X509* pX509)
{
	QScopedPointer<BIO, OpenSslCustomDeleter> bio(BIO_new(BIO_s_mem()));
	PEM_write_bio_X509(bio.data(), pX509);
	char* data = nullptr;

	// See macro BIO_get_mem_data(bio, &data); if BIO_ctrl fails.
	// We do not use this to avoid -Wold-style-cast warning
	const long len = BIO_ctrl(bio.data(), BIO_CTRL_INFO, 0, &data);
	return QByteArray(data, static_cast<int>(len));
}


bool KeyPair::isValid() const
{
	return !mKey.isNull() && !mCertificate.isNull();
}
