/**
 * Copyright (c) 2014-2025 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "NetworkManager.h"
#include "TcToken.h"
#include "UrlUtil.h"
#include "asn1/CVCertificate.h"
#include "asn1/CVCertificateChainBuilder.h"
#include "context/AccessRightManager.h"
#include "context/WorkflowContext.h"
#include "paos/invoke/DidAuthenticateResponseEac1.h"
#include "paos/invoke/DidAuthenticateResponseEac2.h"
#include "paos/invoke/InitializeFrameworkResponse.h"
#include "paos/invoke/StartPaos.h"
#include "paos/invoke/TransmitResponse.h"
#include "paos/retrieve/DidAuthenticateEac1.h"
#include "paos/retrieve/DidAuthenticateEac2.h"
#include "paos/retrieve/DidAuthenticateEacAdditional.h"
#include "paos/retrieve/InitializeFramework.h"
#include "paos/retrieve/StartPaosResponse.h"
#include "paos/retrieve/Transmit.h"

#include <QDebug>
#include <QPointer>
#include <QSharedPointer>
#include <QSslCertificate>
#include <QUrl>

#include <functional>

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
	friend class ::test_StateRedirectBrowser;
	friend class ::test_StatePreVerification;
	friend class ::test_StateProcessCertificatesFromEac2;
	friend class ::test_StateCertificateDescriptionCheck;
	friend class TestAuthContext;

	public:
		using BrowserHandler = std::function<QString (const QSharedPointer<AuthContext>&)>;

	private:
		bool mTcTokenNotFound;
		bool mErrorReportedToServer;
		bool mReceivedBrowserSendFailed;
		bool mSkipMobileRedirect;
		bool mChangeTransportPin;

		QUrl mActivationUrl;
		QUrl mTcTokenUrl;
		QSharedPointer<const TcToken> mTcToken;
		QUrl mRefreshUrl;
		QString mReceivedMessageId;
		QSharedPointer<StartPaos> mStartPaos;
		QSharedPointer<InitializeFramework> mInitializeFramework;
		QSharedPointer<InitializeFrameworkResponse> mInitializeFrameworkResponse;
		QSharedPointer<DIDAuthenticateEAC1> mDIDAuthenticateEAC1;
		QSharedPointer<DIDAuthenticateResponseEAC1> mDIDAuthenticateResponseEAC1;
		QSharedPointer<DIDAuthenticateEAC2> mDIDAuthenticateEAC2;
		QSharedPointer<DIDAuthenticateResponseEAC2> mDIDAuthenticateResponseEACAdditionalInputType;
		QSharedPointer<DIDAuthenticateEACAdditional> mDIDAuthenticateEACAdditionalInputType;
		QSharedPointer<DIDAuthenticateResponseEAC2> mDIDAuthenticateResponseEAC2;
		QSharedPointer<Transmit> mTransmit;
		QSharedPointer<TransmitResponse> mTransmitResponse;
		QSharedPointer<StartPaosResponse> mStartPaosResponse;
		QSharedPointer<AccessRightManager> mAccessRightManager;
		QMultiMap<QUrl, QSslCertificate> mCertificates;
		QSharedPointer<const CVCertificate> mDvCvc;
		CVCertificateChainBuilder mCvcChainBuilderProd;
		CVCertificateChainBuilder mCvcChainBuilderTest;
		QByteArray mSslSession;
		BrowserHandler mBrowserHandler;

	Q_SIGNALS:
		void fireShowChangePinViewChanged();
		void fireDidAuthenticateEac1Changed();
		void fireAccessRightManagerCreated(QSharedPointer<AccessRightManager> pAccessRightManager);

	protected:
		explicit AuthContext(const Action pAction, bool pActivateUi = true, const QUrl& pActivationUrl = QUrl(), const BrowserHandler& pHandler = BrowserHandler());

	public:
		explicit AuthContext(bool pActivateUi = true, const QUrl& pActivationUrl = QUrl(), const BrowserHandler& pHandler = BrowserHandler());

		[[nodiscard]] QUrl getActivationUrl() const
		{
			return mActivationUrl;
		}


		[[nodiscard]] bool isErrorReportedToServer() const
		{
			return mErrorReportedToServer;
		}


		[[nodiscard]] bool isReceivedBrowserSendFailed() const
		{
			return mReceivedBrowserSendFailed;
		}


		void setReceivedBrowserSendFailed(bool pReceivedBrowserSendFailed);


		void setErrorReportedToServer(bool pErrorReportedToServer)
		{
			mErrorReportedToServer = pErrorReportedToServer;
		}


		[[nodiscard]] bool changeTransportPin() const
		{
			return mChangeTransportPin;
		}


		void requestChangeTransportPin();


		[[nodiscard]] bool isTcTokenNotFound() const
		{
			return mTcTokenNotFound;
		}


		void setTcTokenNotFound(bool pTcTokenNotFound)
		{
			mTcTokenNotFound = pTcTokenNotFound;
		}


		[[nodiscard]] QList<AcceptedEidType> getAcceptedEidTypes() const override
		{
			if (isCanAllowedMode() || !mDIDAuthenticateEAC1)
			{
				return {AcceptedEidType::CARD_CERTIFIED};
			}

			return mDIDAuthenticateEAC1->getAcceptedEidTypes();
		}


		[[nodiscard]] bool isSkipMobileRedirect() const
		{
			return mSkipMobileRedirect;
		}


		void setMobileSkipRedirect(bool pSkipRedirect = true)
		{
			mSkipMobileRedirect = pSkipRedirect;
		}


		[[nodiscard]] QList<QSslCertificate> getCertificateList() const
		{
			return mCertificates.values();
		}


		void addCertificateData(const QUrl& pUrl, const QSslCertificate& pCert)
		{
			QUrl originUrl = UrlUtil::getUrlOrigin(pUrl);
			qDebug() << "Adding certificate CN=" << pCert.subjectInfo(QSslCertificate::CommonName) << "SN=" << pCert.serialNumber() << "for URL origin" << originUrl;
			mCertificates.insert(originUrl, pCert);
		}


		[[nodiscard]] BrowserHandler getBrowserHandler() const
		{
			return mBrowserHandler;
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
			Q_EMIT fireAcceptedEidTypesChanged();
			Q_EMIT fireEidTypeMismatchChanged();
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


		[[nodiscard]] const QSharedPointer<StartPaosResponse>& getStartPaosResponse() const
		{
			return mStartPaosResponse;
		}


		void setStartPaosResponse(const QSharedPointer<StartPaosResponse>& pStartPaosResponse)
		{
			mStartPaosResponse = pStartPaosResponse;
		}


		[[nodiscard]] const QSharedPointer<TransmitResponse>& getTransmitResponse()
		{
			return mTransmitResponse;
		}


		void setTransmitResponse(const QSharedPointer<TransmitResponse>& pTransmitResponse)
		{
			Q_ASSERT(!pTransmitResponse.isNull());
			mTransmitResponse = pTransmitResponse;
		}


		[[nodiscard]] const QSharedPointer<Transmit>& getTransmit()
		{
			return mTransmit;
		}


		void setTransmit(const QSharedPointer<Transmit>& pTransmit)
		{
			Q_ASSERT(!pTransmit.isNull());
			mTransmit = pTransmit;
		}


		[[nodiscard]] QSharedPointer<AccessRightManager> getAccessRightManager() const
		{
			return mAccessRightManager;
		}


		[[nodiscard]] QByteArray encodeEffectiveChat();


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


		void initCvcChainBuilder(const QList<QSharedPointer<const CVCertificate>>& pAdditionalCertificates = QList<QSharedPointer<const CVCertificate>>());


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
