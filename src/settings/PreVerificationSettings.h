/*!
 * \brief Settings to handle PreVerification and the corresponding linked certificates.
 *
 * \copyright Copyright (c) 2014-2023 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "AbstractSettings.h"

#include <QByteArrayList>

class test_PreVerificationSettings;
class test_StatePreVerification;

namespace governikus
{

class PreVerificationSettings
	: public AbstractSettings
{
	Q_OBJECT

	friend class AppSettings;
	friend class ::test_PreVerificationSettings;

	private:
		QSharedPointer<QSettings> mStore;

		PreVerificationSettings();
		void updateLinkCertificates(const QByteArrayList& pLinkCertificates);

	public:
		~PreVerificationSettings() override = default;

		[[nodiscard]] bool isEnabled() const;
		void setEnabled(bool pEnabled);
		[[nodiscard]] QByteArrayList getLinkCertificates() const;
		void removeLinkCertificate(const QByteArray& pCert);
		void addLinkCertificate(const QByteArray& pCert);
};


} // namespace governikus
