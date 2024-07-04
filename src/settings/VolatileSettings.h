/**
 * Copyright (c) 2020-2024 Governikus GmbH & Co. KG, Germany
 */

/*!
 * \brief Contains non-persistent settings.
 *
 * This is not integrated into AppSettings to avoid
 * circular dependency because settings can use this, too.
 */

#pragma once

#include "Env.h"

#include <QObject>
#include <QReadWriteLock>

#ifndef QT_NO_QDEBUG
	#include <QScopeGuard>

	#define SDK_MODE(pEnable)\
	const auto sdkMode = Env::getSingleton<VolatileSettings>()->isUsedAsSDK();\
	Env::getSingleton<VolatileSettings>()->setUsedAsSDK(pEnable);\
	const auto sdkModeGuard = qScopeGuard([sdkMode] {\
			Env::getSingleton<VolatileSettings>()->setUsedAsSDK(sdkMode);\
		})

#endif

namespace governikus
{

class VolatileSettings
	: public QObject
	, private Env::ThreadSafe
{
	Q_OBJECT
	friend class Env;

	public:
		class Messages
		{
			// See JSON-SDK MsgHandlerAuth and MsgHandlerChangePin
			QString mSessionStarted;
			QString mSessionFailed;
			QString mSessionSucceeded;
			QString mSessionInProgress;

			public:
				Messages(const QString& pSessionStarted = QString(), const QString& pSessionFailed = QString(),
						const QString& mSessionSucceeded = QString(), const QString& pSessionInProgress = QString());

				[[nodiscard]] QString getSessionStarted() const;
				[[nodiscard]] QString getSessionFailed() const;
				[[nodiscard]] QString getSessionSucceeded() const;
				[[nodiscard]] QString getSessionInProgress() const;
		};

	private:
		static constexpr bool cHandleInterruptDefault = false;

		VolatileSettings();
		~VolatileSettings() override = default;

		mutable QReadWriteLock mLock;
		bool mUsedAsSdk;
		bool mDeveloperMode;
		bool mHandleInterrupt;
		Messages mMessages;
		ulong mDelay;

	public:
		[[nodiscard]] bool isUsedAsSDK() const;
		void setUsedAsSDK(bool pSdk);

		[[nodiscard]] bool isDeveloperMode() const;
		void setDeveloperMode(bool pMode = false);

		[[nodiscard]] bool handleInterrupt() const;
		void setHandleInterrupt(bool pScan = cHandleInterruptDefault);

		void setMessages(const Messages& pMessages = Messages());
		[[nodiscard]] Messages getMessages() const;

		void setDelay(ulong pDelay = 0);
		[[nodiscard]] ulong getDelay() const;

	Q_SIGNALS:
		void fireUsedAsSdkChanged();
};


} // namespace governikus
