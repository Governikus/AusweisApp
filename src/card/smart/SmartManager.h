/**
 * Copyright (c) 2021-2023 Governikus GmbH & Co. KG, Germany
 */

/*!
 * \brief Wrapper for calls to eID applet interface
 */

#pragma once

#include "Card.h"
#include "InputAPDUInfo.h"
#include "apdu/CommandApdu.h"
#include "apdu/ResponseApdu.h"
#include "pinpad/EstablishPaceChannelOutput.h"

#include <QDebug>
#include <QObject>
#include <QSharedPointer>
#include <QVector>
#include <QWeakPointer>

#include <eid_applet_results.h>
#include <functional>


Q_DECLARE_METATYPE(EidStatus)
Q_DECLARE_METATYPE(EidUpdateInfo)
Q_DECLARE_METATYPE(EidServiceResult)
Q_DECLARE_METATYPE(GenericDataResult)
Q_DECLARE_METATYPE(InitializeResult)
Q_DECLARE_METATYPE(PersonalizationResult)


namespace governikus
{

class SmartManager
	: public QObject
{
	Q_OBJECT

	private:
		static QWeakPointer<SmartManager> cWeakInstance;
		static QSharedPointer<SmartManager> cStrongInstance;

	public:
		static QSharedPointer<SmartManager> get(bool pKeepConnection = false);
		static void releaseConnection();

	private:
		bool mInitialized;

		SmartManager();
		[[nodiscard]] bool isValid() const;

	public:
		using ProgressHandler = std::function<void (int progress)>;

		~SmartManager() override;
		EidStatus status() const;
		EidUpdateInfo updateInfo();
		bool deleteSmart(const ProgressHandler& pHandler = ProgressHandler()) const;
		bool deletePersonalization() const;
		bool installSmart(const ProgressHandler& pHandler = ProgressHandler()) const;
		InitializeResult initializePersonalization(const QString& pChallenge, const QString& pPin) const;
		QByteArrayList performPersonalization(const QVector<InputAPDUInfo>& pInputApdus) const;
		[[nodiscard]] PersonalizationResult finalizePersonalization() const;
		EstablishPaceChannelOutput prepareIdentification(const QByteArray& pChat) const;
		[[nodiscard]] ResponseApduResult challenge() const;
		[[nodiscard]] TerminalAndChipAuthenticationResult performTAandCA(
			const CVCertificateChain& pTerminalCvcChain,
			const QByteArray& pAuxiliaryData,
			const QByteArray& pSignature,
			const QByteArray& pPin,
			const QByteArray& pEphemeralPublicKey) const;

		ResponseApduResult transmit(const CommandApdu& pCmd) const;
		void abortSDKWorkflow() const;
};

} // namespace governikus

QDebug operator<<(QDebug pDbg, const EidStatus& pStatus);
QDebug operator<<(QDebug pDbg, const EidUpdateInfo& pInfo);
QDebug operator<<(QDebug pDbg, const EidServiceResult& pResult);
