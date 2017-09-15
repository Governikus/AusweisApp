/*!
 * \brief Settings to handle PreVerification and the corresponding linked certificates.
 *
 * \copyright Copyright (c) 2014 Governikus GmbH & Co. KG
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
	friend class ::test_StatePreVerification;

	private:
		QByteArrayList mLinkCertificates;
		bool mEnabled;

		PreVerificationSettings();

	public:
		virtual ~PreVerificationSettings();

		virtual void load() override;
		virtual bool isUnsaved() const override;
		virtual void save() override;

		bool isEnabled() const;
		void setEnabled(bool pEnabled);
		const QByteArrayList& getLinkCertificates() const;
		bool removeLinkCertificate(const QByteArray& pCert);
		bool addLinkCertificate(const QByteArray& pCert);
};

inline bool operator==(const PreVerificationSettings& pLeft, const PreVerificationSettings& pRight)
{
	return &pLeft == &pRight ||
		   (pLeft.isEnabled() == pRight.isEnabled() &&
		   containsAllEntries(pLeft.getLinkCertificates(), pRight.getLinkCertificates()));
}


inline bool operator!=(const PreVerificationSettings& pLeft, const PreVerificationSettings& pRight)
{
	return !(pLeft == pRight);
}


} /* namespace governikus */
