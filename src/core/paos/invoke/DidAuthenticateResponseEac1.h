/*!
 * \brief Generate information for DIDAuthenticateResponseEAC1.
 *
 * \copyright Copyright (c) 2014 Governikus GmbH & Co. KG
 */

#pragma once

#include "EstablishPACEChannel.h"
#include "paos/ResponseType.h"
#include "PaosCreator.h"

#include <QByteArrayList>
#include <QString>

namespace governikus
{

class DIDAuthenticateResponseEAC1
	: public PaosCreator
	, public ResponseType
{
	private:
		QByteArray mCertificateHolderAuthorizationTemplate;
		QByteArrayList mCertificationAuthorityReferences;
		QByteArray mEfCardAccess;
		QByteArray mIdPICC;
		QByteArray mChallenge;

		QDomElement createDIDAuthenticateResponseEAC1Element();
		QDomElement createAuthenticationProtocolDataElement();

		virtual QDomElement getDocumentStructure();
		virtual Result getResult() const;

		Q_DISABLE_COPY(DIDAuthenticateResponseEAC1)

	public:
		DIDAuthenticateResponseEAC1();
		virtual ~DIDAuthenticateResponseEAC1();


		const QByteArray& getCertificateHolderAuthorizationTemplate() const;
		const QByteArray& getEFCardAccess() const;
		const QByteArray& getIDPICC() const;
		const QByteArray& getChallenge() const;

		void setCertificateHolderAuthorizationTemplate(const QByteArray& pValue);
		void setCertificationAuthorityReference(const EstablishPACEChannelOutput& pPaceChannelOutput);
		void setEFCardAccess(const QByteArray& pValue);
		void setIDPICC(const QByteArray& pValue);
		void setChallenge(const QByteArray& pValue);
};

} /* namespace governikus */
