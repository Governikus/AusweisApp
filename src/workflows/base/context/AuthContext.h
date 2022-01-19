/*!
 * \brief Authentication context.
 *
 * \copyright Copyright (c) 2014-2022 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "asn1/CVCertificate.h"
#include "asn1/CVCertificateChainBuilder.h"
#include "ActivationContext.h"
#include "context/AccessRightManager.h"
#include "context/WorkflowContext.h"
#include "NetworkManager.h"
#include "paos/invoke/DidAuthenticateResponseEac1.h"
#include "paos/invoke/DidAuthenticateResponseEac2.h"
#include "paos/invoke/DidListResponse.h"
#include "paos/invoke/DisconnectResponse.h"
#include "paos/invoke/InitializeFrameworkResponse.h"
#include "paos/invoke/StartPaos.h"
#include "paos/invoke/TransmitResponse.h"
#include "paos/retrieve/DidAuthenticateEac1.h"
#include "paos/retrieve/DidAuthenticateEac2.h"
#include "paos/retrieve/DidAuthenticateEacAdditional.h"
#include "paos/retrieve/DidList.h"
#include "paos/retrieve/Disconnect.h"
#include "paos/retrieve/InitializeFramework.h"
#include "paos/retrieve/StartPaosResponse.h"
#include "paos/retrieve/Transmit.h"
#include "TcToken.h"
#include "UrlUtil.h"

#include <QDebug>
#include <QPointer>
#include <QSharedPointer>
#include <QSslCertificate>
#include <QUrl>

class test_StateRedirectBrowser;
class test_StatePreVerification;
class test_StateCertificateDescriptionCheck;

namespace governikus
{
class TestAuthContext;

class AuthContext
	: public WorkflowContext
{
	Q_OBJECT

	private:
		friend class ::test_StateRedirectBrowser;
		friend class ::test_StatePreVerification;
		friend class ::test_StateProcessCertificatesFromEac2;
		friend class ::test_StateCertificateDescriptionCheck;
		friend class TestAuthContext;

		int mProgressValue;
		QString mProgressMessage;
		bool mTcTokenNotFound;
		bool mErrorReportedToServer;
		bool mSkipRedirect;

		QSharedPointer<ActivationContext> mActivationContext;
		QUrl mTcTokenUrl;
		QSharedPointer<const TcToken> mTcToken;
		QUrl mRefreshUrl;
		QString mReceivedMessageId;
		QSharedPointer<StartPaos> mStartPaos;
		QSharedPointer<InitializeFramework> mInitializeFramework;
		QSharedPointer<InitializeFrameworkResponse> mInitializeFrameworkResponse;
		QSharedPointer<DIDList> mDIDList;
		QSharedPointer<DIDListResponse> mDIDListResponse;
		QSharedPointer<DIDAuthenticateEAC1> mDIDAuthenticateEAC1;
		QSharedPointer<DIDAuthenticateResponseEAC1> mDIDAuthenticateResponseEAC1;
		QSharedPointer<DIDAuthenticateEAC2> mDIDAuthenticateEAC2;
		QSharedPointer<DIDAuthenticateResponseEAC2> mDIDAuthenticateResponseEACAdditionalInputType;
		QSharedPointer<DIDAuthenticateEACAdditional> mDIDAuthenticateEACAdditionalInputType;
		QSharedPointer<DIDAuthenticateResponseEAC2> mDIDAuthenticateResponseEAC2;
		QVector<QSharedPointer<Transmit>> mTransmits;
		QVector<QSharedPointer<TransmitResponse>> mTransmitResponses;
		QSharedPointer<Disconnect> mDisconnect;
		QSharedPointer<DisconnectResponse> mDisconnectResponse;
		QSharedPointer<StartPaosResponse> mStartPaosResponse;
		QSharedPointer<AccessRightManager> mAccessRightManager;
		QMultiMap<QUrl, QSslCertificate> mCertificates;
		QSharedPointer<const CVCertificate> mDvCvc;
		CVCertificateChainBuilder mCvcChainBuilderProd, mCvcChainBuilderTest;
		QByteArray mSslSession;

	Q_SIGNALS:
		void fireProgressChanged();
		void fireDidAuthenticateEac1Changed();
		void fireAccessRightManagerCreated(QSharedPointer<AccessRightManager> pAccessRightManager);

	public:
		explicit AuthContext(const QSharedPointer<ActivationContext>& pActivationContext);

		[[nodiscard]] bool isErrorReportedToServer() const
		{
			return mErrorReportedToServer;
		}


		void setErrorReportedToServer(bool pErrorReportedToServer)
		{
			mErrorReportedToServer = pErrorReportedToServer;
		}


		[[nodiscard]] int getProgressValue() const
		{
			return mProgressValue;
		}


		[[nodiscard]] const QString getProgressMessage() const
		{
			return mProgressMessage;
		}


		void setProgress(int pValue, const QString& pMessage);


		[[nodiscard]] bool isTcTokenNotFound() const
		{
			return mTcTokenNotFound;
		}


		void setTcTokenNotFound(bool pTcTokenNotFound)
		{
			mTcTokenNotFound = pTcTokenNotFound;
		}


		[[nodiscard]] bool isSkipRedirect() const
		{
			return mSkipRedirect;
		}


		void setSkipRedirect(bool pSkipRedirect)
		{
			mSkipRedirect = pSkipRedirect;
		}


		[[nodiscard]] QList<QSslCertificate> getCertificateList() const
		{
			return mCertificates.values();
		}


		[[nodiscard]] bool containsCertificateFor(const QUrl& pUrl) const
		{
			return mCertificates.contains(UrlUtil::getUrlOrigin(pUrl));
		}


		void addCertificateData(const QUrl& pUrl, const QSslCertificate& pCert)
		{
			QUrl originUrl = UrlUtil::getUrlOrigin(pUrl);
			qDebug() << "Adding certificate CN=" << pCert.subjectInfo(QSslCertificate::CommonName) << "SN=" << pCert.serialNumber() << "for URL origin" << originUrl;
			mCertificates.insert(originUrl, pCert);
		}


		[[nodiscard]] ActivationContext* getActivationContext() const
		{
			return mActivationContext.data();
		}


		[[nodiscard]] const QUrl& getTcTokenUrl() const
		{
			return mTcTokenUrl;
		}


		void setTcTokenUrl(const QUrl& pTcTokenUrl)
		{
			mTcTokenUrl = pTcTokenUrl;
		}


		[[nodiscard]] const QSharedPointer<const TcToken>& getTcToken() const
		{
			return mTcToken;
		}


		void setTcToken(const QSharedPointer<const TcToken>& pTcToken)
		{
			mTcToken = pTcToken;
		}


		[[nodiscard]] const QString& getReceivedMessageId() const
		{
			return mReceivedMessageId;
		}


		void setReceivedMessageId(const QString& pReceivedMessageId)
		{
			mReceivedMessageId = pReceivedMessageId;
		}


		[[nodiscard]] const QUrl& getRefreshUrl() const
		{
			return mRefreshUrl;
		}


		void setRefreshUrl(const QUrl& pRefreshUrl)
		{
			mRefreshUrl = pRefreshUrl;
		}


		[[nodiscard]] const QSharedPointer<DIDAuthenticateEAC1>& getDidAuthenticateEac1() const
		{
			return mDIDAuthenticateEAC1;
		}


		void setDidAuthenticateEac1(const QSharedPointer<DIDAuthenticateEAC1>& pDIDAuthenticateEAC1)
		{
			mDIDAuthenticateEAC1 = pDIDAuthenticateEAC1;
			Q_EMIT fireDidAuthenticateEac1Changed();
		}


		[[nodiscard]] const QSharedPointer<DIDAuthenticateEAC2>& getDidAuthenticateEac2() const
		{
			return mDIDAuthenticateEAC2;
		}


		void setDidAuthenticateEac2(const QSharedPointer<DIDAuthenticateEAC2>& pDidAuthenticateEac2)
		{
			mDIDAuthenticateEAC2 = pDidAuthenticateEac2;
		}


		[[nodiscard]] const QSharedPointer<DIDAuthenticateResponseEAC1>& getDidAuthenticateResponseEac1() const
		{
			return mDIDAuthenticateResponseEAC1;
		}


		void setDidAuthenticateResponseEac1(const QSharedPointer<DIDAuthenticateResponseEAC1>& pDidAuthenticateResponseEac1)
		{
			mDIDAuthenticateResponseEAC1 = pDidAuthenticateResponseEac1;
		}


		[[nodiscard]] const QSharedPointer<DIDAuthenticateResponseEAC2>& getDidAuthenticateResponseEacAdditionalInputType() const
		{
			return mDIDAuthenticateResponseEACAdditionalInputType;
		}


		void setDidAuthenticateResponseEacAdditionalInputType(const QSharedPointer<DIDAuthenticateResponseEAC2>& pDidAuthenticateResponseEacAdditionalInputType)
		{
			mDIDAuthenticateResponseEACAdditionalInputType = pDidAuthenticateResponseEacAdditionalInputType;
		}


		[[nodiscard]] const QSharedPointer<DIDAuthenticateEACAdditional>& getDidAuthenticateEacAdditional() const
		{
			return mDIDAuthenticateEACAdditionalInputType;
		}


		void setDidAuthenticateEacAdditional(const QSharedPointer<DIDAuthenticateEACAdditional>& pDidAuthenticateEacAdditionalInputType)
		{
			mDIDAuthenticateEACAdditionalInputType = pDidAuthenticateEacAdditionalInputType;
		}


		[[nodiscard]] const QSharedPointer<DIDAuthenticateResponseEAC2>& getDidAuthenticateResponseEac2() const
		{
			return mDIDAuthenticateResponseEAC2;
		}


		void setDidAuthenticateResponseEac2(const QSharedPointer<DIDAuthenticateResponseEAC2>& pDidAuthenticateResponseEac2)
		{
			mDIDAuthenticateResponseEAC2 = pDidAuthenticateResponseEac2;
		}


		[[nodiscard]] const QSharedPointer<DIDList>& getDidList() const
		{
			return mDIDList;
		}


		void setDidList(const QSharedPointer<DIDList>& pDidList)
		{
			mDIDList = pDidList;
		}


		const QSharedPointer<DIDListResponse>& getDidListResponse()
		{
			return mDIDListResponse;
		}


		void setDidListResponse(const QSharedPointer<DIDListResponse>& pDidListResponse)
		{
			mDIDListResponse = pDidListResponse;
		}


		[[nodiscard]] const QSharedPointer<InitializeFramework>& getInitializeFramework() const
		{
			return mInitializeFramework;
		}


		void setInitializeFramework(const QSharedPointer<InitializeFramework>& pInitializeFramework)
		{
			mInitializeFramework = pInitializeFramework;
		}


		[[nodiscard]] const QSharedPointer<InitializeFrameworkResponse>& getInitializeFrameworkResponse() const
		{
			return mInitializeFrameworkResponse;
		}


		void setInitializeFrameworkResponse(const QSharedPointer<InitializeFrameworkResponse>& pInitializeFrameworkResponse)
		{
			mInitializeFrameworkResponse = pInitializeFrameworkResponse;
		}


		[[nodiscard]] const QSharedPointer<Disconnect>& getDisconnect() const
		{
			return mDisconnect;
		}


		void setDisconnect(const QSharedPointer<Disconnect>& pDisconnect)
		{
			mDisconnect = pDisconnect;
		}


		const QSharedPointer<DisconnectResponse>& getDisconnectResponse()
		{
			return mDisconnectResponse;
		}


		void setDisconnectResponse(const QSharedPointer<DisconnectResponse>& pDisconnectResponse)
		{
			mDisconnectResponse = pDisconnectResponse;
		}


		[[nodiscard]] const QSharedPointer<StartPaosResponse>& getStartPaosResponse() const
		{
			return mStartPaosResponse;
		}


		void setStartPaosResponse(const QSharedPointer<StartPaosResponse>& pStartPaosResponse)
		{
			mStartPaosResponse = pStartPaosResponse;
		}


		const QVector<QSharedPointer<TransmitResponse>>& getTransmitResponses()
		{
			return mTransmitResponses;
		}


		void addTransmitResponse(const QSharedPointer<TransmitResponse>& pTransmitResponse)
		{
			Q_ASSERT(!pTransmitResponse.isNull());
			mTransmitResponses += pTransmitResponse;
		}


		const QVector<QSharedPointer<Transmit>>& getTransmits()
		{
			return mTransmits;
		}


		void addTransmit(const QSharedPointer<Transmit>& pTransmit)
		{
			Q_ASSERT(!pTransmit.isNull());
			mTransmits += pTransmit;
		}


		[[nodiscard]] QSharedPointer<AccessRightManager> getAccessRightManager() const
		{
			return mAccessRightManager;
		}


		QByteArray encodeEffectiveChat();


		[[nodiscard]] const QSharedPointer<StartPaos>& getStartPaos() const
		{
			return mStartPaos;
		}


		void setStartPaos(const QSharedPointer<StartPaos>& pStartPaos)
		{
			mStartPaos = pStartPaos;
		}


		[[nodiscard]] CVCertificateChain getChainStartingWith(const QSharedPointer<const CVCertificate>& pChainRoot) const;


		[[nodiscard]] bool hasChainForCertificationAuthority(const EstablishPaceChannelOutput& pPaceOutput) const;


		[[nodiscard]] CVCertificateChain getChainForCertificationAuthority(const EstablishPaceChannelOutput& pPaceOutput) const;


		void initCvcChainBuilder(const QVector<QSharedPointer<const CVCertificate>>& pAdditionalCertificates = QVector<QSharedPointer<const CVCertificate>>());


		[[nodiscard]] const QSharedPointer<const CVCertificate>& getDvCvc() const
		{
			return mDvCvc;
		}


		void setDvCvc(const QSharedPointer<const CVCertificate>& dvCvc)
		{
			mDvCvc = dvCvc;
		}


		void initAccessRightManager(const QSharedPointer<const CVCertificate>& pTerminalCvc);
		[[nodiscard]] bool isCanAllowedMode() const override;

		[[nodiscard]] const QByteArray& getSslSession() const;
		void setSslSession(const QByteArray& pSession);
};

} // namespace governikus
