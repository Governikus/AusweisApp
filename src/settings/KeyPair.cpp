/**
 * Copyright (c) 2017-2025 Governikus GmbH & Co. KG, Germany
 */


#include "KeyPair.h"

#include "Randomizer.h"

#include <openssl/bio.h>
#include <openssl/ec.h>
#include <openssl/pem.h>
#include <openssl/rsa.h>

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


KeyPair KeyPair::generatePair(EVP_PKEY* pKey, const QByteArray& pSignerKey, const QByteArray& pSignerCert)
{
	if (pKey)
	{
		const auto cert = createCertificate(pKey, parseKey(pSignerKey), parseCertificate(pSignerCert));
		if (cert)
		{
			return KeyPair(QSslKey(Qt::HANDLE(pKey), QSsl::PrivateKey), QSslCertificate(rewriteCertificate(cert.data())));
		}
		else
		{
			OpenSslCustomDeleter::cleanup(pKey);
		}
	}

	return KeyPair();
}


KeyPair KeyPair::generate(const char* pCurve, const QByteArray& pSignerKey, const QByteArray& pSignerCert)
{
	if (!pCurve)
	{
		return KeyPair();
	}

	const auto& func = [pCurve](EVP_PKEY_CTX* pCtx) {
				return EVP_PKEY_CTX_ctrl_str(pCtx, "ec_paramgen_curve", pCurve);
			};
	return generatePair(createKey(EVP_PKEY_EC, func), pSignerKey, pSignerCert);
}


KeyPair KeyPair::generate(int pKeySize, const QByteArray& pSignerKey, const QByteArray& pSignerCert)
{
	const auto& func = [pKeySize](EVP_PKEY_CTX* pCtx) {
				return EVP_PKEY_CTX_set_rsa_keygen_bits(pCtx, pKeySize);
			};
	return generatePair(createKey(EVP_PKEY_RSA, func), pSignerKey, pSignerCert);
}


const QSslKey& KeyPair::getKey() const
{
	return mKey;
}


const QSslCertificate& KeyPair::getCertificate() const
{
	return mCertificate;
}


EVP_PKEY* KeyPair::createKey(int pKeyCtxNid, const std::function<bool(EVP_PKEY_CTX*)>& pFunc)
{
	if (!Randomizer::getInstance().isSecureRandom())
	{
		qCCritical(settings) << "Cannot get enough entropy";
		return nullptr;
	}

	QScopedPointer<EVP_PKEY_CTX, OpenSslCustomDeleter> pkeyCtx(EVP_PKEY_CTX_new_id(pKeyCtxNid, nullptr));

	if (pkeyCtx.isNull())
	{
		qCCritical(settings) << "Cannot create EVP_PKEY_CTX";
		return nullptr;
	}

	if (EVP_PKEY_keygen_init(pkeyCtx.data()) != 1)
	{
		qCCritical(settings) << "Cannot init key ctx";
		return nullptr;
	}

	if (pFunc && pFunc(pkeyCtx.data()) != 1)
	{
		qCCritical(settings) << "Cannot set parameters";
		return nullptr;
	}

	EVP_PKEY* pkey = nullptr;
	if (EVP_PKEY_keygen(pkeyCtx.data(), &pkey) != 1)
	{
		qCCritical(settings) << "Cannot generate key";
		return nullptr;
	}

	return pkey;
}


QSharedPointer<EVP_PKEY> KeyPair::parseKey(const QByteArray& pData)
{
	if (pData.isEmpty())
	{
		return nullptr;
	}

	QScopedPointer<BIO, OpenSslCustomDeleter> bio(BIO_new_mem_buf(pData.constData(), -1));
	return QSharedPointer<EVP_PKEY>(PEM_read_bio_PrivateKey(bio.data(), nullptr, nullptr, nullptr), &EVP_PKEY_free);
}


QSharedPointer<X509> KeyPair::parseCertificate(const QByteArray& pData)
{
	if (pData.isEmpty())
	{
		return nullptr;
	}

	QScopedPointer<BIO, OpenSslCustomDeleter> bio(BIO_new_mem_buf(pData.constData(), -1));
	return QSharedPointer<X509>(PEM_read_bio_X509(bio.data(), nullptr, nullptr, nullptr), &X509_free);
}


QSharedPointer<X509> KeyPair::createCertificate(EVP_PKEY* pPkey, const QSharedPointer<EVP_PKEY>& pSignerKey, const QSharedPointer<X509>& pSignerCert)
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
	EVP_PKEY* signer = pPkey;

	if (pSignerKey && pSignerCert)
	{
		signer = pSignerKey.data();
		name.reset(X509_NAME_dup(X509_get_subject_name(pSignerCert.data())));
	}
	X509_set_issuer_name(x509.data(), name.data());

	if (!X509_sign(x509.data(), signer, EVP_sha256()))
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
