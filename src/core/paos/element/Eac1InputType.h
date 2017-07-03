/*!
 * \brief Store information of Eac1InputType.
 *
 * \copyright Copyright (c) 2014 Governikus GmbH & Co. KG
 */

#pragma once

#include "asn1/AuthenticatedAuxiliaryData.h"
#include "asn1/CVCertificate.h"
#include "asn1/CertificateDescription.h"
#include "asn1/Chat.h"

#include <QByteArray>
#include <QSharedPointer>
#include <QString>
#include <QVector>

class test_StatePrepareChat;
class test_StateExtractCvcsFromEac1InputType;
class test_StatePreVerification;
class test_StateCertificateDescriptionCheck;
class test_StateProcessCertificatesFromEac2;

namespace governikus
{

class Eac1InputType
{
	friend class DidAuthenticateEac1Parser;
	friend class::test_StatePrepareChat;
	friend class::test_StateExtractCvcsFromEac1InputType;
	friend class::test_StatePreVerification;
	friend class::test_StateCertificateDescriptionCheck;
	friend class::test_StateProcessCertificatesFromEac2;

	private:
		QVector<QSharedPointer<CVCertificate> > mCvCertificates;
		QByteArray mCertificateDescriptionAsBinary;
		QSharedPointer<CertificateDescription> mCertificateDescription;
		QSharedPointer<CHAT> mRequiredChat, mOptionalChat;
		QByteArray mAuthenticatedAuxiliaryDataAsBinary;
		QSharedPointer<AuthenticatedAuxiliaryData> mAuthenticatedAuxiliaryData;
		QString mTransactionInfo;

		void setAuthenticatedAuxiliaryData(const QSharedPointer<AuthenticatedAuxiliaryData>& authenticatedAuxiliaryData)
		{
			mAuthenticatedAuxiliaryData = authenticatedAuxiliaryData;
		}


		void setAuthenticatedAuxiliaryDataAsBinary(const QByteArray& authenticatedAuxiliaryDataAsBinary)
		{
			mAuthenticatedAuxiliaryDataAsBinary = authenticatedAuxiliaryDataAsBinary;
		}


		void setCertificateDescription(QSharedPointer<CertificateDescription> certificateDescription)
		{
			mCertificateDescription = certificateDescription;
		}


		void setCertificateDescriptionAsBinary(const QByteArray& certificateDescriptionAsBinary)
		{
			mCertificateDescriptionAsBinary = certificateDescriptionAsBinary;
		}


		void appendCvcerts(QSharedPointer<CVCertificate> cvcert)
		{
			mCvCertificates.append(cvcert);
		}


		void setOptionalChat(const QSharedPointer<CHAT>& optionalChat)
		{
			mOptionalChat = optionalChat;
		}


		void setRequiredChat(const QSharedPointer<CHAT>& requiredChat)
		{
			mRequiredChat = requiredChat;
		}


		void setTransactionInfo(const QString& transactionInfo)
		{
			mTransactionInfo = transactionInfo;
		}


	public:
		Eac1InputType();
		~Eac1InputType();

		QSharedPointer<AuthenticatedAuxiliaryData> getAuthenticatedAuxiliaryData() const
		{
			return mAuthenticatedAuxiliaryData;
		}


		const QByteArray& getAuthenticatedAuxiliaryDataAsBinary() const
		{
			return mAuthenticatedAuxiliaryDataAsBinary;
		}


		QSharedPointer<CertificateDescription> getCertificateDescription() const
		{
			return mCertificateDescription;
		}


		const QByteArray& getCertificateDescriptionAsBinary() const
		{
			return mCertificateDescriptionAsBinary;
		}


		const QVector<QSharedPointer<CVCertificate> >& getCvCertificates() const
		{
			return mCvCertificates;
		}


		const QSharedPointer<CHAT>& getOptionalChat() const
		{
			return mOptionalChat;
		}


		const QSharedPointer<CHAT>& getRequiredChat() const
		{
			return mRequiredChat;
		}


		const QString& getTransactionInfo() const
		{
			return mTransactionInfo;
		}


};

} /* namespace governikus */
