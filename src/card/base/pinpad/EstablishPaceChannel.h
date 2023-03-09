/**
 * Copyright (c) 2015-2023 Governikus GmbH & Co. KG, Germany
 */

/*!
 * \brief Data object for output of card command EstablishPaceChannel
 */

#pragma once

#include "SmartCardDefinitions.h"
#include "asn1/ASN1TemplateUtil.h"
#include "asn1/CertificateDescription.h"

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
		QByteArray mPassword;

	public:
		EstablishPaceChannel(
			PacePasswordId pPasswordId = PacePasswordId::UNKNOWN,
			const QByteArray& pChat = QByteArray(),
			const QByteArray& pCertificateDescription = QByteArray());

		void setPassword(const QByteArray& pPassword);

		static bool isCcid(const QByteArray& pInput);
		bool fromCcid(const QByteArray& pInput);
		bool fromInputData(const QByteArray& pInput);

		[[nodiscard]] PacePasswordId getPasswordId() const;
		[[nodiscard]] const QByteArray& getChat() const;
		[[nodiscard]] const QByteArray& getCertificateDescription() const;

		/**
		 * Defined in pcsc10_v2.02.08_amd1.1 section 2.6.16
		 */
		[[nodiscard]] QByteArray createInputData() const;

		/**
		 * Defined in pcsc10_v2.02.08_amd1.1 section 2.5.12
		 */
		[[nodiscard]] QByteArray createCommandData() const;

		/**
		 * Defined in BSI-TR-03119_V1_pdf
		 */
		[[nodiscard]] QByteArray createCommandDataCcid() const;

#ifndef QT_NO_DEBUG
		bool operator==(const EstablishPaceChannel& pOther) const
		{
			return mPasswordId == pOther.mPasswordId
				   && mChat == pOther.mChat
				   && mCertificateDescription == pOther.mCertificateDescription;
		}


#endif
};

}  // namespace governikus
