/**
 * Copyright (c) 2014-2023 Governikus GmbH & Co. KG, Germany
 */

/*!
 * \brief Store information of Eac1InputType.
 */

#pragma once

#include "SmartCardDefinitions.h"
#include "asn1/AuthenticatedAuxiliaryData.h"
#include "asn1/CVCertificate.h"
#include "asn1/CertificateDescription.h"
#include "asn1/Chat.h"

#include <QByteArray>
#include <QSharedPointer>
#include <QString>
#include <QVector>


namespace governikus
{
class TestAuthContext;

class Eac1InputType
{
	friend class DidAuthenticateEac1Parser;
	friend class TestAuthContext;

	private:
		QVector<QSharedPointer<const CVCertificate>> mCvCertificates;
		QByteArray mCertificateDescriptionAsBinary;
		QSharedPointer<const CertificateDescription> mCertificateDescription;
		QSharedPointer<const CHAT> mRequiredChat;
		QSharedPointer<const CHAT> mOptionalChat;
		QByteArray mAuthenticatedAuxiliaryDataAsBinary;
		QSharedPointer<const AuthenticatedAuxiliaryData> mAuthenticatedAuxiliaryData;
		QString mTransactionInfo;
		QVector<AcceptedEidType> mAcceptedEidTypes;

		void setAuthenticatedAuxiliaryData(const QSharedPointer<const AuthenticatedAuxiliaryData>& pAuthenticatedAuxiliaryData)
		{
			mAuthenticatedAuxiliaryData = pAuthenticatedAuxiliaryData;
		}


		void setAuthenticatedAuxiliaryDataAsBinary(const QByteArray& pAuthenticatedAuxiliaryDataAsBinary)
		{
			mAuthenticatedAuxiliaryDataAsBinary = pAuthenticatedAuxiliaryDataAsBinary;
		}


		void setCertificateDescription(const QSharedPointer<const CertificateDescription>& pCertificateDescription)
		{
			mCertificateDescription = pCertificateDescription;
		}


		void setCertificateDescriptionAsBinary(const QByteArray& pCertificateDescriptionAsBinary)
		{
			mCertificateDescriptionAsBinary = pCertificateDescriptionAsBinary;
		}


		void appendCvcerts(const QSharedPointer<const CVCertificate>& pCert)
		{
			mCvCertificates += pCert;
		}


		void setOptionalChat(const QSharedPointer<const CHAT>& pOptionalChat)
		{
			mOptionalChat = pOptionalChat;
		}


		void setRequiredChat(const QSharedPointer<CHAT>& pRequiredChat)
		{
			mRequiredChat = pRequiredChat;
		}


		void setTransactionInfo(const QString& pTransactionInfo)
		{
			mTransactionInfo = pTransactionInfo;
		}


		void appendAcceptedEidType(AcceptedEidType pAcceptedEidType)
		{
			mAcceptedEidTypes += pAcceptedEidType;
		}

	public:
		Eac1InputType();
		~Eac1InputType() = default;

		[[nodiscard]] const QSharedPointer<const AuthenticatedAuxiliaryData>& getAuthenticatedAuxiliaryData() const
		{
			return mAuthenticatedAuxiliaryData;
		}


		[[nodiscard]] const QByteArray& getAuthenticatedAuxiliaryDataAsBinary() const
		{
			return mAuthenticatedAuxiliaryDataAsBinary;
		}


		[[nodiscard]] const QSharedPointer<const CertificateDescription>& getCertificateDescription() const
		{
			return mCertificateDescription;
		}


		[[nodiscard]] const QByteArray& getCertificateDescriptionAsBinary() const
		{
			return mCertificateDescriptionAsBinary;
		}


		[[nodiscard]] const QVector<QSharedPointer<const CVCertificate>>& getCvCertificates() const
		{
			return mCvCertificates;
		}


		[[nodiscard]] const QSharedPointer<const CHAT>& getOptionalChat() const
		{
			return mOptionalChat;
		}


		[[nodiscard]] const QSharedPointer<const CHAT>& getRequiredChat() const
		{
			return mRequiredChat;
		}


		[[nodiscard]] const QString& getTransactionInfo() const
		{
			return mTransactionInfo;
		}


		[[nodiscard]] const QVector<AcceptedEidType>& getAcceptedEidTypes() const
		{
			return mAcceptedEidTypes;
		}


};

} // namespace governikus
