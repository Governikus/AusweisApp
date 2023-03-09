/**
 * Copyright (c) 2018-2023 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include <QObject>
#include <QSharedPointer>
#include <QVector>

#if defined(Q_OS_WIN)
	#include <QProcess>
#endif

class test_DiagnosisFirewallDetection;

namespace governikus
{

class FirewallProfile
{
	private:
		const QString mName;
		const bool mEnabled;

	public:
		FirewallProfile(const QString& pName, bool pEnabled);

		[[nodiscard]] const QString& getName() const
		{
			return mName;
		}


		[[nodiscard]] bool getEnabled() const
		{
			return mEnabled;
		}


};

class FirewallSoftware
{
	private:
		QString mName;
		bool mEnabled;
		bool mUpToDate;

	public:
		FirewallSoftware(const QString& pName, bool pEnabled, bool pUpToDate);

		[[nodiscard]] const QString& getName() const
		{
			return mName;
		}


		[[nodiscard]] bool getEnabled() const
		{
			return mEnabled;
		}


		[[nodiscard]] bool getUpToDate() const
		{
			return mUpToDate;
		}


};

class DiagnosisFirewallDetection
	: public QObject
{
	Q_OBJECT
	friend class ::test_DiagnosisFirewallDetection;

	private:
		bool mFirstFirewallRuleExists;
		bool mFirstFirewallRuleEnabled;
		bool mSecondFirewallRuleExists;
		bool mSecondFirewallRuleEnabled;
		QVector<QSharedPointer<FirewallSoftware>> mDetectedFirewalls;
		QVector<QSharedPointer<FirewallProfile>> mFirewallProfiles;

		bool mFirstRuleDone;
		bool mSecondRuleDone;
		bool mProfilesDone;
		bool mInstalledFirewallsDone;

#if defined(Q_OS_WIN)
		QProcess mFirewallFirstRuleProcess;
		QProcess mFirewallSecondRuleProcess;
		QProcess mFirewallProfilesProcess;
		QProcess mInstalledFirewallSoftwareProcess;

		void checkIfAllInformationReady();
		void parseFirewallFirstRuleInfos(const QString& pFirewallFirstRuleInfos);
		void parseFirewallSecondRuleInfos(const QString& pFirewallSecondRuleInfos);
		void parseFirewallProfileInfos(const QString& pFirewallProfileInfos);
		void parseFirewallSoftwareInfos(const QString& pFirewallSoftwareInfos);

	private Q_SLOTS:
		void onFirstRuleDone(int exitCode, QProcess::ExitStatus exitStatus);
		void onFirstRuleError(QProcess::ProcessError pError);
		void onSecondRuleDone(int exitCode, QProcess::ExitStatus exitStatus);
		void onSecondRuleError(QProcess::ProcessError pError);
		void onProfilesDone(int exitCode, QProcess::ExitStatus exitStatus);
		void onProfilesError(QProcess::ProcessError pError);
		void onInstalledFirewallSoftwareDone(int exitCode, QProcess::ExitStatus exitStatus);
		void onInstalledFirewallSoftwareError(QProcess::ProcessError pError);
#endif

	public:
		DiagnosisFirewallDetection();
		void startDetection();

		[[nodiscard]] bool getFirstRuleExists() const
		{
			return mFirstFirewallRuleExists;
		}


		[[nodiscard]] bool getFirstRuleEnabled() const
		{
			return mFirstFirewallRuleEnabled;
		}


		[[nodiscard]] bool getSecondRuleExists() const
		{
			return mSecondFirewallRuleExists;
		}


		[[nodiscard]] bool getSecondRuleEnabled() const
		{
			return mSecondFirewallRuleEnabled;
		}


		[[nodiscard]] const QVector<QSharedPointer<FirewallProfile>>& getFirewallProfiles() const
		{
			return mFirewallProfiles;
		}


		[[nodiscard]] const QVector<QSharedPointer<FirewallSoftware>>& getDetectedFirewalls() const
		{
			return mDetectedFirewalls;
		}

	Q_SIGNALS:
		void fireFirewallInformationReady();
		void fireDetectionFailed();
};


} // namespace governikus
