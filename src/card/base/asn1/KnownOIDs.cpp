/*!
 * \copyright Copyright (c) 2017 Governikus GmbH & Co. KG
 */

#include "KnownOIDs.h"

using namespace governikus::KnownOIDs;


QByteArray governikus::toByteArray(Base pValue)
{
	switch (pValue)
	{
		case Base::BSI_DE:
			return QByteArrayLiteral("0.4.0.127.0.7");

		case Base::SIGNED_DATA:
			return QByteArrayLiteral("1.2.840.113549.1.7.2");

		case Base::ID_SECURITY_OBJECT:
			return QByteArrayLiteral("0.4.0.127.0.7.3.2.1");

		case Base::ID_EXTENSIONS:
			return Base::BSI_DE + QByteArrayLiteral(".3.1.3");
	}

	Q_UNREACHABLE();
}


QByteArray governikus::toByteArray(CertificateExtensions pValue)
{
	switch (pValue)
	{
		case CertificateExtensions::ID_DESCRIPTION:
			return Base::ID_EXTENSIONS + QByteArrayLiteral(".1");

		case CertificateExtensions::ID_SECTOR:
			return Base::ID_EXTENSIONS + QByteArrayLiteral(".2");
	}

	Q_UNREACHABLE();
}


QByteArray governikus::toByteArray(TermsOfUsageType pValue)
{
	switch (pValue)
	{
		case TermsOfUsageType::ID_PLAIN_FORMAT:
			return CertificateExtensions::ID_DESCRIPTION + QByteArrayLiteral(".1");

		case TermsOfUsageType::ID_HTML_FORMAT:
			return CertificateExtensions::ID_DESCRIPTION + QByteArrayLiteral(".2");

		case TermsOfUsageType::ID_PDF_FORMAT:
			return CertificateExtensions::ID_DESCRIPTION + QByteArrayLiteral(".3");
	}

	Q_UNREACHABLE();
}


QByteArray governikus::toByteArray(CHATType pValue)
{
	switch (pValue)
	{
		case CHATType::ID_IS:
			// OID for the CHAT type inspection systems.
			return Base::BSI_DE + QByteArrayLiteral(".3.1.2.1");

		case CHATType::ID_AT:
			// OID for the CHAT type authentication terminals.
			return Base::BSI_DE + QByteArrayLiteral(".3.1.2.2");

		case CHATType::ID_ST:
			// OID for the CHAT type signature terminals.
			return Base::BSI_DE + QByteArrayLiteral(".3.1.2.3");
	}

	Q_UNREACHABLE();
}


QByteArray governikus::toByteArray(AuxilaryData pValue)
{
	switch (pValue)
	{
		case AuxilaryData::ID_DATE_OF_BIRTH:
			return Base::BSI_DE + QByteArrayLiteral(".3.1.4.1");

		case AuxilaryData::ID_DATE_OF_EXPIRY:
			return Base::BSI_DE + QByteArrayLiteral(".3.1.4.2");

		case AuxilaryData::ID_COMMUNITY_ID:
			return Base::BSI_DE + QByteArrayLiteral(".3.1.4.3");
	}

	Q_UNREACHABLE();
}


QByteArray governikus::toByteArray(SecurityProtocol pValue)
{
	switch (pValue)
	{
		case SecurityProtocol::ID_PK:
			return Base::BSI_DE + QByteArrayLiteral(".2.2.1");

		case SecurityProtocol::ID_TA:
			return Base::BSI_DE + QByteArrayLiteral(".2.2.2");

		case SecurityProtocol::ID_CA:
			return Base::BSI_DE + QByteArrayLiteral(".2.2.3");

		case SecurityProtocol::ID_PACE:
			return Base::BSI_DE + QByteArrayLiteral(".2.2.4");
	}

	Q_UNREACHABLE();
}


QByteArray governikus::toByteArray(id_ca pValue)
{
	switch (pValue)
	{
		case id_ca::DH:
			return SecurityProtocol::ID_CA + QByteArrayLiteral(".1");

		case id_ca::DH_3DES_CBC_CBC:
			return id_ca::DH + QByteArrayLiteral(".1");

		case id_ca::DH_AES_CBC_CMAC_128:
			return id_ca::DH + QByteArrayLiteral(".2");

		case id_ca::DH_AES_CBC_CMAC_192:
			return id_ca::DH + QByteArrayLiteral(".3");

		case id_ca::DH_AES_CBC_CMAC_256:
			return id_ca::DH + QByteArrayLiteral(".4");


		case id_ca::ECDH:
			return SecurityProtocol::ID_CA + QByteArrayLiteral(".2");

		case id_ca::ECDH_3DES_CBC_CBC:
			return id_ca::ECDH + QByteArrayLiteral(".1");

		case id_ca::ECDH_AES_CBC_CMAC_128:
			return id_ca::ECDH + QByteArrayLiteral(".2");

		case id_ca::ECDH_AES_CBC_CMAC_192:
			return id_ca::ECDH + QByteArrayLiteral(".3");

		case id_ca::ECDH_AES_CBC_CMAC_256:
			return id_ca::ECDH + QByteArrayLiteral(".4");
	}

	Q_UNREACHABLE();
}


QByteArray governikus::toByteArray(id_ta pValue)
{
	switch (pValue)
	{
		case id_ta::ECDSA_SHA_1:
			return SecurityProtocol::ID_TA + QByteArrayLiteral(".2.1");

		case id_ta::ECDSA_SHA_224:
			return SecurityProtocol::ID_TA + QByteArrayLiteral(".2.2");

		case id_ta::ECDSA_SHA_256:
			return SecurityProtocol::ID_TA + QByteArrayLiteral(".2.3");

		case id_ta::ECDSA_SHA_384:
			return SecurityProtocol::ID_TA + QByteArrayLiteral(".2.4");

		case id_ta::ECDSA_SHA_512:
			return SecurityProtocol::ID_TA + QByteArrayLiteral(".2.5");
	}

	Q_UNREACHABLE();
}


QByteArray governikus::toByteArray(id_pk pValue)
{
	switch (pValue)
	{
		case id_pk::DH:
			return SecurityProtocol::ID_PK + QByteArrayLiteral(".1");

		case id_pk::ECDH:
			return SecurityProtocol::ID_PK + QByteArrayLiteral(".2");
	}

	Q_UNREACHABLE();
}


QByteArray governikus::toByteArray(id_PACE::DH pValue)
{
	switch (pValue)
	{
		case id_PACE::DH::GM:
			return SecurityProtocol::ID_PACE + QByteArrayLiteral(".1");

		case id_PACE::DH::GM_3DES_CBC_CBC:
			return id_PACE::DH::GM + QByteArrayLiteral(".1");

		case id_PACE::DH::GM_AES_CBC_CMAC_128:
			return id_PACE::DH::GM + QByteArrayLiteral(".2");

		case id_PACE::DH::GM_AES_CBC_CMAC_192:
			return id_PACE::DH::GM + QByteArrayLiteral(".3");

		case id_PACE::DH::GM_AES_CBC_CMAC_256:
			return id_PACE::DH::GM + QByteArrayLiteral(".4");


		case id_PACE::DH::IM:
			return SecurityProtocol::ID_PACE + QByteArrayLiteral(".3");

		case id_PACE::DH::IM_3DES_CBC_CBC:
			return id_PACE::DH::IM + QByteArrayLiteral(".1");

		case id_PACE::DH::IM_AES_CBC_CMAC_128:
			return id_PACE::DH::IM + QByteArrayLiteral(".2");

		case id_PACE::DH::IM_AES_CBC_CMAC_192:
			return id_PACE::DH::IM + QByteArrayLiteral(".3");

		case id_PACE::DH::IM_AES_CBC_CMAC_256:
			return id_PACE::DH::IM + QByteArrayLiteral(".4");
	}

	Q_UNREACHABLE();
}


QByteArray governikus::toByteArray(id_PACE::ECDH pValue)
{
	switch (pValue)
	{
		case id_PACE::ECDH::GM:
			return SecurityProtocol::ID_PACE + QByteArrayLiteral(".2");

		case id_PACE::ECDH::GM_3DES_CBC_CBC:
			return id_PACE::ECDH::GM + QByteArrayLiteral(".1");

		case id_PACE::ECDH::GM_AES_CBC_CMAC_128:
			return id_PACE::ECDH::GM + QByteArrayLiteral(".2");

		case id_PACE::ECDH::GM_AES_CBC_CMAC_192:
			return id_PACE::ECDH::GM + QByteArrayLiteral(".3");

		case id_PACE::ECDH::GM_AES_CBC_CMAC_256:
			return id_PACE::ECDH::GM + QByteArrayLiteral(".4");


		case id_PACE::ECDH::IM:
			return SecurityProtocol::ID_PACE + QByteArrayLiteral(".4");

		case id_PACE::ECDH::IM_3DES_CBC_CBC:
			return id_PACE::ECDH::IM + QByteArrayLiteral(".1");

		case id_PACE::ECDH::IM_AES_CBC_CMAC_128:
			return id_PACE::ECDH::IM + QByteArrayLiteral(".2");

		case id_PACE::ECDH::IM_AES_CBC_CMAC_192:
			return id_PACE::ECDH::IM + QByteArrayLiteral(".3");

		case id_PACE::ECDH::IM_AES_CBC_CMAC_256:
			return id_PACE::ECDH::IM + QByteArrayLiteral(".4");
	}

	Q_UNREACHABLE();
}
