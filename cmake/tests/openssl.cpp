/*
 * \copyright Copyright (c) 2021-2022 Governikus GmbH & Co. KG, Germany
 */

#include <QSslCipher>
#include <QSslSocket>

#include <openssl/crypto.h>

#include <algorithm>

#ifdef OPENSSL_NO_CMS
	#error Cryptographic Message Syntax (CMS) is required. Do you use old LibreSSL?
#endif

#ifdef OPENSSL_NO_EC
	#error Elliptic Curve is required.
#endif

#ifdef OPENSSL_NO_TLS1_2
	#error TLS 1.2 is required.
#endif

#ifdef OPENSSL_NO_PSK
	#error RSA-PSK is required.
#endif

int main()
{
	// It is required that Qt and AusweisApp2 uses the library and the same version.
	// Also this binary will crash if your libraries aren't binary compatible. For example
	// this occurs if you link Qt against OpenSSL and the AusweisApp2 against LibreSSL.
	if (QSslSocket::sslLibraryVersionString() != QLatin1String(OpenSSL_version(OPENSSL_VERSION)))
	{
		return 1;
	}

	// The AusweisApp2 requires at least one of an RSA-PSK cipher. LibreSSL and OpenSSL <= 1.0.2 does not support that!
	const QStringList ciphers({"RSA-PSK-AES256-GCM-SHA384", "RSA-PSK-AES256-CBC-SHA384", "RSA-PSK-AES128-GCM-SHA256", "RSA-PSK-AES128-CBC-SHA256", "RSA-PSK-AES256-CBC-SHA"});
	return std::any_of(ciphers.constBegin(), ciphers.constEnd(), [](const QString& pCipherName)
		{
			return !QSslCipher(pCipherName).isNull();
		}) ? 0 : 2;
}
