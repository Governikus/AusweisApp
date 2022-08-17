/*!
 * \brief Generate information for DIDAuthenticateResponseEAC1.
 *
 * \copyright Copyright (c) 2014-2022 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "PaosCreator.h"
#include "paos/ResponseType.h"

#include <QByteArrayList>
#include <QString>

namespace governikus
{

class EstablishPaceChannelOutput;


class DIDAuthenticateResponseEAC1
	: public PaosCreator
	, public ResponseType
{
	Q_DISABLE_COPY(DIDAuthenticateResponseEAC1)

	private:
		QByteArray mCertificateHolderAuthorizationTemplate;
		QByteArrayList mCertificationAuthorityReferences;
		QByteArray mEfCardAccess;
		QByteArray mIdPICC;
		QByteArray mChallenge;

		void createDIDAuthenticateResponseEAC1Element();
		void createAuthenticationProtocolDataElement();

		void createBodyElement() override;
		[[nodiscard]] virtual ECardApiResult getResult() const;

	public:
		DIDAuthenticateResponseEAC1();
		~DIDAuthenticateResponseEAC1() override = default;


		[[nodiscard]] const QByteArray& getCertificateHolderAuthorizationTemplate() const;
		[[nodiscard]] const QByteArray& getEFCardAccess() const;
		[[nodiscard]] const QByteArray& getIDPICC() const;
		[[nodiscard]] const QByteArray& getChallenge() const;

		void setCertificateHolderAuthorizationTemplate(const QByteArray& pValue);
		void setCertificationAuthorityReference(const EstablishPaceChannelOutput& pPaceChannelOutput);
		void setEFCardAccess(const QByteArray& pValue);
		void setIDPICC(const QByteArray& pValue);
		void setChallenge(const QByteArray& pValue);
};

} // namespace governikus
