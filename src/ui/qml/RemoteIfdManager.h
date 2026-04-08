/**
 * Copyright (c) 2026 Governikus GmbH & Co. KG, Germany
 */

#pragma once


#include <QObject>


class test_RemoteIfdManager;


namespace governikus
{

class RemoteIfdManager
	: public QObject
{
	Q_OBJECT
	friend class ::test_RemoteIfdManager;

	private:
		std::function<bool()> mCondition;
		bool mDetectionOnly;
		bool mRemoteIfdWasRunning;

	private Q_SLOTS:
		void onApplicationStateChanged(bool pIsAppInForeground);

	public:
		explicit RemoteIfdManager(std::function<bool()> pCondition, bool pDetectionOnly = true);
};

} // namespace governikus
