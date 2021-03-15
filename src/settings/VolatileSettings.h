/*!
 * \brief Contains non-persistent settings.
 *
 * This is not integrated into AppSettings to avoid
 * circular dependency because settings can use this, too.
 *
 * \copyright Copyright (c) 2020-2021 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "Env.h"

#include <QObject>

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
{
	Q_GADGET

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
		static constexpr bool cHandleInterruptDefault = true;

		bool mUsedAsSdk;
		bool mHandleInterrupt;
		Messages mMessages;

	protected:
		VolatileSettings();
		~VolatileSettings() = default;
		static VolatileSettings& getInstance();

	public:
		[[nodiscard]] bool isUsedAsSDK() const;
		void setUsedAsSDK(bool pSdk);

		[[nodiscard]] bool handleInterrupt() const;
		void setHandleInterrupt(bool pScan = cHandleInterruptDefault);

		void setMessages(const Messages& pMessages = Messages());
		[[nodiscard]] const Messages& getMessages() const;
};


} // namespace governikus
