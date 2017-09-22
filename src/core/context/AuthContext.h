/*!
 * AuthContext.h
 *
 * \brief Authentication context.
 *
 * \copyright Copyright (c) 2014 Governikus GmbH & Co. KG
 */

#pragma once

#include "asn1/Chat.h"
#include "asn1/CVCertificate.h"
#include "asn1/CVCertificateChainBuilder.h"
#include "ActivationHandler.h"
#include "context/WorkflowContext.h"
#include "Commands.h"
#include "EnumHelper.h"
#include "NetworkManager.h"
#include "paos/invoke/DidAuthenticateResponseEac1.h"
#include "paos/invoke/DidAuthenticateResponseEac2.h"
#include "paos/invoke/DidListResponse.h"
#include "paos/invoke/DisconnectResponse.h"
#include "paos/invoke/InitializeFrameworkResponse.h"
#include "paos/invoke/StartPaos.h"
#include "paos/invoke/TransmitResponse.h"
#include "paos/MessageIdHandler.h"
#include "paos/retrieve/DidAuthenticateEac1.h"
#include "paos/retrieve/DidAuthenticateEac2.h"
#include "paos/retrieve/DidAuthenticateEacAdditional.h"
#include "paos/retrieve/DidList.h"
#include "paos/retrieve/InitializeFramework.h"
#include "paos/retrieve/StartPaosResponse.h"
#include "paos/retrieve/Transmit.h"
#include "Result.h"
#include "SelfAuthenticationData.h"
#include "TcToken.h"
#include "UrlUtil.h"

#include <QDebug>
#include <QPointer>
#include <QSharedPointer>
#include <QSslCertificate>
#include <QUrl>

class test_StatePrepareChat;
class test_StateRedirectBrowser;
class test_StatePreVerification;
class test_StateProcessCertificatesFromEac2;
class test_StateCertificateDescriptionCheck;

namespace governikus
{

class AuthContext
	: public WorkflowContext
{
	Q_OBJECT

	private:
		friend class ::test_StatePrepareChat;
		friend class ::test_StateRedirectBrowser;
		friend class ::test_StatePreVerification;
		friend class ::test_StateProcessCertificatesFromEac2;
		friend class ::test_StateCertificateDescriptionCheck;

		bool mTcTokenNotFound;
		bool mErrorReportedToServer;

		QScopedPointer<ActivationContext> mActivationContext;
		QScopedPointer<NetworkManager> mNetworkManager;
		QUrl mTcTokenUrl;
		QSharedPointer<TcToken> mTcToken;
		QUrl mRefreshUrl;
		QSharedPointer<MessageIdHandler> mMessageIdHandler;
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
		QVector<QSharedPointer<Transmit> > mTransmits;
		QVector<QSharedPointer<TransmitResponse> > mTransmitResponses;
		QSharedPointer<DisconnectResponse> mDisconnectResponse;
		QSharedPointer<StartPaosResponse> mStartPaosResponse;
		QSharedPointer<CHAT> mOptionalChat, mRequiredChat, mEffectiveChat;
		QString mRequiredAge;
		QMultiMap<QUrl, QSslCertificate> mCertificates;
		QSharedPointer<CVCertificate> mTerminalCvc, mDvCvc;
		CVCertificateChainBuilder mCvcChainBuilderProd, mCvcChainBuilderTest;

		bool sanitizeEffectiveAccessRights();

	Q_SIGNALS:
		void fireDidAuthenticateEac1Changed();
		void fireEffectiveChatChanged();
		void fireOptionalChatChanged();
		void fireRequiredChatChanged();
		void fireRequiredAgeChanged();

	public:
		AuthContext(ActivationContext* pActivationContext);
		virtual ~AuthContext();


		bool isErrorReportedToServer() const
		{
			return mErrorReportedToServer;
		}


		void setErrorReportedToServer(bool pErrorReportedToServer)
		{
			mErrorReportedToServer = pErrorReportedToServer;
		}


		bool isTcTokenNotFound() const
		{
			return mTcTokenNotFound;
		}


		void setTcTokenNotFound(bool pTcTokenNotFound)
		{
			this->mTcTokenNotFound = pTcTokenNotFound;
		}


		NetworkManager& getNetworkManager()
		{
			return *mNetworkManager;
		}


#ifndef Q_NO_DEBUG
		// for testing purposes only!
		void setNetworkManager(NetworkManager* pNetworkManager)
		{
			mNetworkManager.reset(pNetworkManager);
		}


#endif

		QList<QSslCertificate> getCertificateList() const
		{
			return mCertificates.values();
		}


		bool containsCertificateFor(const QUrl& pUrl) const
		{
			return mCertificates.contains(UrlUtil::getUrlOrigin(pUrl));
		}


		void addCertificateData(const QUrl& pUrl, const QSslCertificate& pCert)
		{
			QUrl originUrl = UrlUtil::getUrlOrigin(pUrl);
			qDebug() << "Adding certificate CN=" << pCert.subjectInfo(QSslCertificate::CommonName) << "SN=" << pCert.serialNumber() << "for URL origin" << originUrl;
			mCertificates.insertMulti(originUrl, pCert);
		}


		ActivationContext* getActivationContext() const
		{
			return mActivationContext.data();
		}


		const QUrl& getTcTokenUrl() const
		{
			return mTcTokenUrl;
		}


		void setTcTokenUrl(const QUrl& pTcTokenUrl)
		{
			mTcTokenUrl = pTcTokenUrl;
		}


		const QSharedPointer<TcToken>& getTcToken() const
		{
			return mTcToken;
		}


		void setTcToken(const QSharedPointer<TcToken>& pTcToken)
		{
			mTcToken = pTcToken;
		}


		const QSharedPointer<MessageIdHandler>& getMessageIdHandler() const
		{
			return mMessageIdHandler;
		}


		const QUrl& getRefreshUrl() const
		{
			return mRefreshUrl;
		}


		void setRefreshUrl(const QUrl& pRefreshUrl)
		{
			mRefreshUrl = pRefreshUrl;
		}


		const QSharedPointer<DIDAuthenticateEAC1>& getDidAuthenticateEac1() const
		{
			return mDIDAuthenticateEAC1;
		}


		void setDidAuthenticateEac1(const QSharedPointer<DIDAuthenticateEAC1>& pDIDAuthenticateEAC1)
		{
			mDIDAuthenticateEAC1 = pDIDAuthenticateEAC1;
			Q_EMIT fireDidAuthenticateEac1Changed();
		}


		const QSharedPointer<DIDAuthenticateEAC2>& getDidAuthenticateEac2() const
		{
			return mDIDAuthenticateEAC2;
		}


		void setDidAuthenticateEac2(const QSharedPointer<DIDAuthenticateEAC2>& didAuthenticateEac2)
		{
			mDIDAuthenticateEAC2 = didAuthenticateEac2;
		}


		const QSharedPointer<DIDAuthenticateResponseEAC1>& getDidAuthenticateResponseEac1() const
		{
			return mDIDAuthenticateResponseEAC1;
		}


		void setDidAuthenticateResponseEac1(const QSharedPointer<DIDAuthenticateResponseEAC1>& didAuthenticateResponseEac1)
		{
			mDIDAuthenticateResponseEAC1 = didAuthenticateResponseEac1;
		}


		const QSharedPointer<DIDAuthenticateResponseEAC2>& getDidAuthenticateResponseEacAdditionalInputType() const
		{
			return mDIDAuthenticateResponseEACAdditionalInputType;
		}


		void setDidAuthenticateResponseEacAdditionalInputType(const QSharedPointer<DIDAuthenticateResponseEAC2>& didAuthenticateResponseEacAdditionalInputType)
		{
			mDIDAuthenticateResponseEACAdditionalInputType = didAuthenticateResponseEacAdditionalInputType;
		}


		const QSharedPointer<DIDAuthenticateEACAdditional>& getDidAuthenticateEacAdditional() const
		{
			return mDIDAuthenticateEACAdditionalInputType;
		}


		void setDidAuthenticateEacAdditional(const QSharedPointer<DIDAuthenticateEACAdditional>& didAuthenticateEacAdditionalInputType)
		{
			mDIDAuthenticateEACAdditionalInputType = didAuthenticateEacAdditionalInputType;
		}


		const QSharedPointer<DIDAuthenticateResponseEAC2>& getDidAuthenticateResponseEac2() const
		{
			return mDIDAuthenticateResponseEAC2;
		}


		void setDidAuthenticateResponseEac2(const QSharedPointer<DIDAuthenticateResponseEAC2>& didAuthenticateResponseEac2)
		{
			mDIDAuthenticateResponseEAC2 = didAuthenticateResponseEac2;
		}


		const QSharedPointer<DIDList>& getDidList() const
		{
			return mDIDList;
		}


		void setDidList(const QSharedPointer<DIDList>& didList)
		{
			mDIDList = didList;
		}


		const QSharedPointer<DIDListResponse>& getDidListResponse()
		{
			return mDIDListResponse;
		}


		void setDidListResponse(const QSharedPointer<DIDListResponse>& didListResponse)
		{
			mDIDListResponse = didListResponse;
		}


		const QSharedPointer<InitializeFramework>& getInitializeFramework() const
		{
			return mInitializeFramework;
		}


		void setInitializeFramework(const QSharedPointer<InitializeFramework>& initializeFramework)
		{
			mInitializeFramework = initializeFramework;
		}


		const QSharedPointer<InitializeFrameworkResponse>& getInitializeFrameworkResponse() const
		{
			return mInitializeFrameworkResponse;
		}


		void setInitializeFrameworkResponse(const QSharedPointer<InitializeFrameworkResponse>& initializeFrameworkResponse)
		{
			mInitializeFrameworkResponse = initializeFrameworkResponse;
		}


		const QSharedPointer<DisconnectResponse>& getDisconnectResponse()
		{
			return mDisconnectResponse;
		}


		void setDisconnectResponse(const QSharedPointer<DisconnectResponse>& pDisconnectResponse)
		{
			mDisconnectResponse = pDisconnectResponse;
		}


		const QSharedPointer<StartPaosResponse>& getStartPaosResponse() const
		{
			return mStartPaosResponse;
		}


		void setStartPaosResponse(const QSharedPointer<StartPaosResponse>& startPaosResponse)
		{
			mStartPaosResponse = startPaosResponse;
		}


		const QVector<QSharedPointer<TransmitResponse> >& getTransmitResponses()
		{
			return mTransmitResponses;
		}


		void addTransmitResponse(const QSharedPointer<TransmitResponse>& pTransmitResponse)
		{
			Q_ASSERT(!pTransmitResponse.isNull());
			mTransmitResponses.append(pTransmitResponse);
		}


		const QVector<QSharedPointer<Transmit> >& getTransmits()
		{
			return mTransmits;
		}


		void addTransmit(const QSharedPointer<Transmit>& pTransmit)
		{
			Q_ASSERT(!pTransmit.isNull());
			mTransmits.append(pTransmit);
		}


		const QSharedPointer<const CHAT> getEffectiveChat() const
		{
			return mEffectiveChat;
		}


		/*!
		 * \return Returns false if the accessrights were
		 *         not valid and an adjustment was made.
		 */
		bool setEffectiveChat(const QSharedPointer<CHAT>& pEffectiveChat);


		/*!
		 * \return Returns true if the accessright is already set or was added.
		 */
		bool addEffectiveAccessRight(AccessRight pAccessRight);


		/*!
		 * \return Returns true if the accessright isn't set or was removed.
		 */
		bool removeEffectiveAccessRight(AccessRight pAccessRight);


		/*!
		 * \return Returns false if the accessrights were
		 *         not valid and an adjustment was made.
		 */
		bool setEffectiveAccessRights(const QSet<AccessRight>& pAccessRights);


		const QSharedPointer<const CHAT> getOptionalChat() const
		{
			return mOptionalChat;
		}


		void setOptionalChat(const QSharedPointer<CHAT>& pOptionalChat);


		const QSharedPointer<const CHAT> getRequiredChat() const
		{
			return mRequiredChat;
		}


		void setRequiredChat(const QSharedPointer<CHAT>& pRequiredChat);

		const QString& getRequiredAge()
		{
			return mRequiredAge;
		}


		void setRequiredAge(const QString& pRequiredAge)
		{
			if (mRequiredAge != pRequiredAge)
			{
				mRequiredAge = pRequiredAge;
				Q_EMIT fireRequiredAgeChanged();
			}
		}


		const QSharedPointer<StartPaos>& getStartPaos() const
		{
			return mStartPaos;
		}


		void setStartPaos(const QSharedPointer<StartPaos>& pStartPaos)
		{
			mStartPaos = pStartPaos;
		}


		CVCertificateChain getChainStartingWith(const QSharedPointer<CVCertificate>& pChainRoot) const;


		bool hasChainForCertificationAuthority(const EstablishPACEChannelOutput& pPaceOutput) const;


		CVCertificateChain getChainForCertificationAuthority(const EstablishPACEChannelOutput& pPaceOutput) const;


		void initCvcChainBuilder(const QVector<QSharedPointer<CVCertificate> >& pAdditionalCertificates = QVector<QSharedPointer<CVCertificate> >());


		const QSharedPointer<CVCertificate>& getDvCvc() const
		{
			return mDvCvc;
		}


		void setDvCvc(const QSharedPointer<CVCertificate>& dvCvc)
		{
			mDvCvc = dvCvc;
		}


		const QSharedPointer<CVCertificate>& getTerminalCvc() const
		{
			return mTerminalCvc;
		}


		void setTerminalCvc(const QSharedPointer<CVCertificate>& terminalCvc)
		{
			mTerminalCvc = terminalCvc;
		}


};

} /* namespace governikus */
