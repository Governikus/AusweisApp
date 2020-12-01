/*!
 * \brief Data object for output of card command EstablishPaceChannel
 *
 * \copyright Copyright (c) 2015-2020 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "asn1/ASN1TemplateUtil.h"
#include "asn1/CertificateDescription.h"
#include "asn1/SecurityInfos.h"
#include "CommandApdu.h"
#include "SmartCardDefinitions.h"

#include <QByteArray>


namespace governikus
{

/*!
 * As defined in TR-03119:
 *
 * EstablishPaceChannelInput ::= SEQUENCE {
 *      passwordID             [1] INTEGER
 *      transmittedPassword    [2] NumericString OPTIONAL
 *      cHAT                   [3] OCTET STRING OPTIONAL
 *      certificateDescription [4] CertificateDescription OPTIONAL
 *      hashOID                [5] OBJECT IDENTIFIER OPTIONAL
 * }
 * CertificateDescription is defined in [TR-03110].
 */
struct ESTABLISHPACECHANNELINPUT
{
	ASN1_INTEGER* mPasswordID;
	ASN1_STRING* mTransmittedPassword;
	ASN1_OCTET_STRING* mCHAT;
	CertificateDescription* mCertificateDescription;
	ASN1_OBJECT* mHashOID;
};
DECLARE_ASN1_OBJECT(ESTABLISHPACECHANNELINPUT)


class EstablishPaceChannel
{
	private:
		PacePasswordId mPasswordId;
		QByteArray mChat;
		QByteArray mCertificateDescription;

	public:
		EstablishPaceChannel();

		/**
		 * Defined in pcsc10_v2.02.08_amd1.1
		 */
		QByteArray createCommandData() const;

		/**
		 * Defined in BSI-TR-03119_V1_pdf
		 */
		CommandApdu createCommandDataCcid() const;

		void setCertificateDescription(const QByteArray& pCertificateDescription);
		void setChat(const QByteArray& pChat);
		void setPasswordId(PacePasswordId pPasswordId);
};

}  // namespace governikus
