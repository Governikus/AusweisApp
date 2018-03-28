/*!
 * \copyright Copyright (c) 2017-2018 Governikus GmbH & Co. KG, Germany
 */

#include "RemoteHelper.h"

#include "AppSettings.h"
#include "Env.h"
#include "KeyPair.h"
#include "TlsChecker.h"

using namespace governikus;

bool RemoteHelper::checkAndGenerateKey()
{
	auto& settings = Env::getSingleton<AppSettings>()->getRemoteServiceSettings();

	if (settings.getKey().isNull()
			|| settings.getCertificate().isNull()
			|| settings.getCertificate().expiryDate() < QDateTime::currentDateTime()
			|| !TlsChecker::hasValidCertificateKeyLength(settings.getCertificate()))
	{
		qDebug() << "Generate local keypair...";
		const auto& pair = KeyPair::generate();
		if (pair.isValid())
		{
			settings.setKey(pair.getKey());
			settings.setCertificate(pair.getCertificate());
			return true;
		}

		return false;
	}

	return true;
}
