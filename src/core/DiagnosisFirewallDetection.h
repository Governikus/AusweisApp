/*!
 * \copyright Copyright (c) 2018 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include <QObject>
#include <QSharedPointer>
#include <QVector>

#if defined(Q_OS_WIN)
#include <QProcess>
#endif


namespace governikus
{

class FirewallProfile
{
	private:
		const QString mName;
		const bool mEnabled;

	public:
		FirewallProfile(QString pName, bool pEnabled);

		const QString& getName() const
		{
			return mName;
		}


		bool getEnabled() const
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
		FirewallSoftware(QString pName, bool pEnabled, bool pUpToDate);

		const QString& getName() const
		{
			return mName;
		}


		bool getEnabled() const
		{
			return mEnabled;
		}


		bool getUpToDate() const
		{
			return mUpToDate;
		}


};

class DiagnosisFirewallDetection
	: public QObject
{
	Q_OBJECT

	private:
		bool mFirstFirewallRuleExists;
		bool mFirstFirewallRuleEnabled;
		bool mSecondFirewallRuleExists;
		bool mSecondFirewallRuleEnabled;
		QVector<QSharedPointer<FirewallSoftware> > mDetectedFirewalls;
		QVector<QSharedPointer<FirewallProfile> > mFirewallProfiles;

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

		bool getFirstRuleExists() const
		{
			return mFirstFirewallRuleExists;
		}


		bool getFirstRuleEnabled() const
		{
			return mFirstFirewallRuleEnabled;
		}


		bool getSecondRuleExists() const
		{
			return mSecondFirewallRuleExists;
		}


		bool getSecondRuleEnabled() const
		{
			return mSecondFirewallRuleEnabled;
		}


		const QVector<QSharedPointer<FirewallProfile> >& getFirewallProfiles() const
		{
			return mFirewallProfiles;
		}


		const QVector<QSharedPointer<FirewallSoftware> >& getDetectedFirewalls() const
		{
			return mDetectedFirewalls;
		}


	Q_SIGNALS:
		void fireFirewallInformationReady();
		void fireDetectionFailed();
};


} // namespace governikus
