/**
 * Copyright (c) 2019-2025 Governikus GmbH & Co. KG, Germany
 */

#include "AusweisApp_p.h"

#include "Bootstrap.h"
#include "UiLoader.h"
#include "UiPluginFunctional.h"

#include <QMetaObject>
#include <QObject>

#include <chrono>
#include <future>
#include <iostream>
#include <mutex>
#include <thread>
#include <vector>

namespace
{
static bool cShutdownCalled = false;
static AusweisAppCallback cCallback = nullptr;
static std::thread cThread; // clazy:exclude=non-pod-global-static
static std::future<void> cStartedFuture; // clazy:exclude=non-pod-global-static
static std::promise<void> cStartedPromise; // clazy:exclude=non-pod-global-static
static std::mutex cMutex;
} // namespace

namespace governikus
{
Q_DECL_EXPORT void ausweisapp_init_internal(const QByteArray& pCmdline)
{
	QByteArrayList userParameter = pCmdline.split(' ');

	std::vector<const char*> args({"SDK", "--ui", "functional"});
	for (QByteArray& entry : userParameter)
	{
		args.push_back(entry.data());
	}

	int length = static_cast<int>(args.size());
	initApp(length, const_cast<char**>(args.data()));
}


Q_DECL_EXPORT bool ausweisapp_is_running_internal()
{
	return cThread.get_id() != std::thread::id();
}


Q_DECL_EXPORT void ausweisapp_started_internal()
{
	cStartedPromise.set_value();
}


Q_DECL_EXPORT AusweisAppCallback ausweisapp_get_callback_internal()
{
	return cCallback;
}


Q_DECL_EXPORT void ausweisapp_join_thread_internal()
{
	if (cThread.joinable())
	{
		if (cThread.get_id() == std::this_thread::get_id())
		{
			// User called ausweisapp_shutdown() in AusweisAppCallback
			std::cout << "Cannot join thread of AusweisApp to avoid deadlock. Do not call ausweisapp_shutdown() inside the callback if blocking is required!" << std::endl;
			return;
		}

		std::cout << "Join thread of AusweisApp..." << std::endl;
		cThread.join();
	}
	else
	{
		std::cout << "Thread of AusweisApp is not joinable" << std::endl;
	}
}


} // namespace governikus


using namespace governikus;


Q_DECL_EXPORT bool ausweisapp_init(AusweisAppCallback pCallback, const char* pCmdline)
{
	if (pCallback == nullptr)
	{
		std::cout << "Initialization failed: callback cannot be nullptr" << std::endl;
		return false;
	}

	const std::lock_guard<std::mutex> lock(cMutex);
	if (ausweisapp_is_running_internal())
	{
		std::cout << "Already initialized" << std::endl;
		return false;
	}

	cCallback = pCallback;
	cShutdownCalled = false;

	cStartedPromise = std::promise<void>();
	cStartedFuture = cStartedPromise.get_future();
	cThread = std::thread(&ausweisapp_init_internal, QByteArray(pCmdline));

	return true;
}


Q_DECL_EXPORT void ausweisapp_shutdown(void)
{
	const std::lock_guard<std::mutex> lock(cMutex);
	if (ausweisapp_is_running_internal())
	{
		if (!cShutdownCalled)
		{
			cShutdownCalled = true; // do not request twice as the UiLoader could be re-spawned after shutdown!

			cStartedFuture.wait();

			std::cout << "Send shutdown request" << std::endl;

			QMetaObject::invokeMethod(QCoreApplication::instance(), [] {
						auto* j = Env::getSingleton<UiLoader>()->getLoaded<UiPluginFunctional>();
						if (j)
						{
							j->doQuitApplicationRequest();
						}
					}, Qt::QueuedConnection);

		}

		ausweisapp_join_thread_internal();
	}
}


Q_DECL_EXPORT bool ausweisapp_is_running(void)
{
	const std::lock_guard<std::mutex> lock(cMutex);
	return ausweisapp_is_running_internal();
}


Q_DECL_EXPORT void ausweisapp_send(const char* pCmd)
{
	const std::lock_guard<std::mutex> lock(cMutex);

	using namespace std::chrono_literals;
	const bool initialized = cStartedFuture.wait_for(0s) == std::future_status::ready;
	if (cShutdownCalled || !initialized || pCmd == nullptr)
	{
		return;
	}

	const auto cmd = QByteArray(pCmd);
	QMetaObject::invokeMethod(QCoreApplication::instance(), [cmd] {
				auto* j = Env::getSingleton<UiLoader>()->getLoaded<UiPluginFunctional>();
				if (j)
				{
					j->doMessageProcessing(cmd);
				}
			}, Qt::QueuedConnection);
}


Q_DECL_EXPORT bool ausweisapp2_init(AusweisApp2Callback pCallback, const char* pCmdline)
{
	return ausweisapp_init(pCallback, pCmdline);
}


Q_DECL_EXPORT void ausweisapp2_shutdown(void)
{
	ausweisapp_shutdown();
}


Q_DECL_EXPORT bool ausweisapp2_is_running(void)
{
	return ausweisapp_is_running();
}


Q_DECL_EXPORT void ausweisapp2_send(const char* pCmd)
{
	ausweisapp_send(pCmd);
}
