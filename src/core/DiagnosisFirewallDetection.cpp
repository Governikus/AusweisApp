/*!
 * \copyright Copyright (c) 2018-2019 Governikus GmbH & Co. KG, Germany
 */

#include "DiagnosisFirewallDetection.h"

#include <QFile>
#include <QLoggingCategory>
#include <QOperatingSystemVersion>
#include <QStandardPaths>

using namespace governikus;

#if defined(Q_OS_WIN)
void DiagnosisFirewallDetection::checkIfAllInformationReady()
{
	if (mFirstRuleDone && mSecondRuleDone && mProfilesDone && mInstalledFirewallsDone)
	{
		Q_EMIT fireFirewallInformationReady();
	}
}


void DiagnosisFirewallDetection::onFirstRuleDone(int exitCode, QProcess::ExitStatus exitStatus)
{
	Q_UNUSED(exitStatus);

	if (exitCode != 0)
	{
		mFirstRuleDone = true;
		mFirstFirewallRuleExists = false;
		mFirstFirewallRuleEnabled = false;
		checkIfAllInformationReady();
		return;
	}

	const QString& output = QString::fromUtf8(mFirewallFirstRuleProcess.readAllStandardOutput());

	const auto& lines = output.split(QStringLiteral("\r\n"));
	for (auto line : lines)
	{
		const QStringList& lineparts = line.split(QStringLiteral(" : "));
		if (lineparts.length() != 2)
		{
			continue;
		}
		if (lineparts[0].startsWith(QLatin1String("Name")))
		{
			if (QOperatingSystemVersion::current() < QOperatingSystemVersion::Windows8 && lineparts[1].startsWith(QLatin1String("AusweisApp2")))
			{
				mFirstFirewallRuleExists = true;
			}
			else if (lineparts[1].startsWith(QLatin1String("AusweisApp2-Firewall-Rule")))
			{
				mFirstFirewallRuleExists = true;
			}
		}
		else if (lineparts[0].startsWith(QLatin1String("Enabled")) && lineparts[1].startsWith(QLatin1String("True")))
		{
			mFirstFirewallRuleEnabled = true;
		}
	}
	mFirstRuleDone = true;
	checkIfAllInformationReady();
}


void DiagnosisFirewallDetection::onFirstRuleError(QProcess::ProcessError pError)
{
	qDebug() << "Error while getting first firewall rule:" << pError;
	mFirstRuleDone = false;
	Q_EMIT fireDetectionFailed();
}


void DiagnosisFirewallDetection::onSecondRuleDone(int exitCode, QProcess::ExitStatus exitStatus)
{
	Q_UNUSED(exitStatus);

	if (exitCode != 0)
	{
		mSecondRuleDone = true;
		mSecondFirewallRuleExists = false;
		mSecondFirewallRuleEnabled = false;
		checkIfAllInformationReady();
		return;
	}

	const QString& output = QString::fromUtf8(mFirewallSecondRuleProcess.readAllStandardOutput());

	const auto& lines = output.split(QStringLiteral("\r\n"));
	for (auto line : lines)
	{
		const QStringList& lineparts = line.split(QStringLiteral(" : "));
		if (lineparts.length() != 2)
		{
			continue;
		}
		if (lineparts[0].startsWith(QLatin1String("Name")))
		{
			if (QOperatingSystemVersion::current() < QOperatingSystemVersion::Windows8 && lineparts[1].startsWith(QLatin1String("AusweisApp2-Firewall-Rule-SaC-In")))
			{
				mSecondFirewallRuleExists = true;
			}
			else if (lineparts[1].startsWith(QLatin1String("AusweisApp2-Firewall-Rule-In")))
			{
				mSecondFirewallRuleExists = true;
			}
		}
		else if (lineparts[0].startsWith(QLatin1String("Enabled")) && lineparts[1].startsWith(QLatin1String("True")))
		{
			mSecondFirewallRuleEnabled = true;
		}
	}
	mSecondRuleDone = true;
	checkIfAllInformationReady();
}


void DiagnosisFirewallDetection::onSecondRuleError(QProcess::ProcessError pError)
{
	qDebug() << "Error while getting second firewall rule:" << pError;
	mSecondRuleDone = false;
	Q_EMIT fireDetectionFailed();
}


void DiagnosisFirewallDetection::onProfilesDone(int exitCode, QProcess::ExitStatus exitStatus)
{
	Q_UNUSED(exitStatus);
	if (exitCode != 0)
	{
		qDebug() << "Error while getting firewall profiles, error code:" << exitCode;
		mProfilesDone = false;
		Q_EMIT fireDetectionFailed();
		return;
	}

	const QString& output = QString::fromUtf8(mFirewallProfilesProcess.readAllStandardOutput());

	QString name;
	bool enabled = false;

	const auto& lines = output.split(QStringLiteral("\r\n"));
	for (auto line : lines)
	{
		const QString& trimmedLine = line.trimmed();
		if (trimmedLine.isEmpty() && !name.isEmpty())
		{
			mFirewallProfiles << QSharedPointer<FirewallProfile>::create(name, enabled);
			name = QString();
			enabled = false;
			continue;
		}

		const QStringList& lineparts = line.split(QStringLiteral(" : "));
		if (lineparts.length() != 2)
		{
			continue;
		}
		if (lineparts[0].startsWith(QLatin1String("Name")))
		{
			name = lineparts[1];
		}
		else if (lineparts[0].startsWith(QLatin1String("Enabled")) && lineparts[1].startsWith(QLatin1String("True")))
		{
			enabled = true;
		}
	}

	mProfilesDone = true;
	checkIfAllInformationReady();
}


void DiagnosisFirewallDetection::onProfilesError(QProcess::ProcessError pError)
{
	qDebug() << "Error while getting firewall profiles:" << pError;
	mProfilesDone = false;
	Q_EMIT fireDetectionFailed();
}


void DiagnosisFirewallDetection::onInstalledFirewallSoftwareDone(int exitCode, QProcess::ExitStatus exitStatus)
{
	Q_UNUSED(exitStatus);

	if (exitCode != 0)
	{
		qDebug() << "Error while getting installed firewalls, error code:" << exitCode;
		mInstalledFirewallsDone = false;
		Q_EMIT fireDetectionFailed();
		return;
	}

	const QString& output = QString::fromUtf8(mInstalledFirewallSoftwareProcess.readAllStandardOutput());

	QString name;
	bool enabled = false;
	bool uptodate = false;

	const auto& lines = output.split(QStringLiteral("\r\n"));
	for (auto line : lines)
	{
		const QString& trimmedLine = line.trimmed();
		if (trimmedLine.isEmpty() && !name.isEmpty())
		{
			mDetectedFirewalls << QSharedPointer<FirewallSoftware>::create(name, enabled, uptodate);
			name = QString();
			enabled = false;
			uptodate = false;
			continue;
		}

		const QStringList& lineparts = line.split(QStringLiteral(" : "));
		if (lineparts.length() != 2)
		{
			continue;
		}
		if (lineparts[0].startsWith(QLatin1String("displayName")))
		{
			name = lineparts[1];
		}
		else if (lineparts[0].startsWith(QLatin1String("productState")))
		{
			// status decoding based on https://gallery.technet.microsoft.com/scriptcenter/Get-the-status-of-4b748f25
			bool convertStatus;
			int statusValue = lineparts[1].toInt(&convertStatus, 10);
			if (convertStatus)
			{
				QString hexString = QString::number(statusValue, 16);
				if (hexString.length() >= 4)
				{
					enabled = hexString.right(4).left(2) == QStringLiteral("10");
					uptodate = hexString.right(2) == QStringLiteral("00");
				}
				else
				{
					qDebug() << "Cannot parse productState value, hex value of int got wrong length:" << statusValue;
					enabled = false;
					uptodate = false;
				}
			}
			else
			{
				qDebug() << "Cannot parse productState value, is not an integer:" << lineparts[2];
				enabled = false;
				uptodate = false;
			}
		}
	}

	mInstalledFirewallsDone = true;
	checkIfAllInformationReady();
}


void DiagnosisFirewallDetection::onInstalledFirewallSoftwareError(QProcess::ProcessError pError)
{
	qDebug() << "Error while getting installed firewall software:" << pError;
	mInstalledFirewallsDone = false;
	Q_EMIT fireDetectionFailed();
}


#endif

DiagnosisFirewallDetection::DiagnosisFirewallDetection()
	: QObject()
	, mFirstFirewallRuleExists(false)
	, mFirstFirewallRuleEnabled(false)
	, mSecondFirewallRuleExists(false)
	, mSecondFirewallRuleEnabled(false)
	, mDetectedFirewalls()
	, mFirewallProfiles()
	, mFirstRuleDone(false)
	, mSecondRuleDone(false)
	, mProfilesDone(false)
	, mInstalledFirewallsDone(false)
#if defined(Q_OS_WIN)
	, mFirewallFirstRuleProcess()
	, mFirewallSecondRuleProcess()
	, mFirewallProfilesProcess()
	, mInstalledFirewallSoftwareProcess()
#endif
{
}


void DiagnosisFirewallDetection::startDetection()
{
#if defined(Q_OS_WIN)
	QString powershellCommand = QStandardPaths::findExecutable(QStringLiteral("powershell.exe"));
	if (powershellCommand.isEmpty())
	{
		powershellCommand = QStringLiteral("C:\\Windows\\System32\\WindowsPowerShell\\v1.0\\powershell.exe");
		if (!QFile::exists(powershellCommand))
		{
			qDebug() << "Could not find powershell";
			Q_EMIT fireDetectionFailed();
			return;
		}
	}

	QStringList firstRuleParameters;
	firstRuleParameters << QStringLiteral("-NoProfile");
	firstRuleParameters << QStringLiteral("-Command");
	if (QOperatingSystemVersion::current() < QOperatingSystemVersion::Windows8)
	{
		firstRuleParameters << QStringLiteral("(New-object -comObject HNetCfg.FwPolicy2).rules | where-object {$_.name -like \"AusweisApp2\"}");
	}
	else
	{
		firstRuleParameters << QStringLiteral("Get-NetFirewallRule -Name \"AusweisApp2-Firewall-Rule\"");
	}

	connect(&mFirewallFirstRuleProcess, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished), this, &DiagnosisFirewallDetection::onFirstRuleDone);
	connect(&mFirewallFirstRuleProcess, &QProcess::errorOccurred, this, &DiagnosisFirewallDetection::onFirstRuleError);

	mFirewallFirstRuleProcess.start(powershellCommand, firstRuleParameters);
	mFirewallFirstRuleProcess.closeWriteChannel();

	QStringList secondRuleParameters;
	secondRuleParameters << QStringLiteral("-NoProfile");
	secondRuleParameters << QStringLiteral("-Command");
	if (QOperatingSystemVersion::current() < QOperatingSystemVersion::Windows8)
	{
		secondRuleParameters << QStringLiteral("(New-object -comObject HNetCfg.FwPolicy2).rules | where-object {$_.name -like \"AusweisApp2-Firewall-Rule-SaC-In\"}");
	}
	else
	{
		secondRuleParameters << QStringLiteral("Get-NetFirewallRule -Name \"AusweisApp2-Firewall-Rule-In\"");
	}

	connect(&mFirewallSecondRuleProcess, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished), this, &DiagnosisFirewallDetection::onSecondRuleDone);
	connect(&mFirewallSecondRuleProcess, &QProcess::errorOccurred, this, &DiagnosisFirewallDetection::onSecondRuleError);

	mFirewallSecondRuleProcess.start(powershellCommand, secondRuleParameters);
	mFirewallSecondRuleProcess.closeWriteChannel();

	QStringList profilesParameters;
	profilesParameters << QStringLiteral("-NoProfile");
	profilesParameters << QStringLiteral("-Command");
	if (QOperatingSystemVersion::current() < QOperatingSystemVersion::Windows8)
	{
		profilesParameters << QStringLiteral("$fw=New-object -comObject HNetCfg.FwPolicy2; $fwProfileTypes= @{1=\"Domain\"; 2=\"Private\" ; 4=\"Public\"}; @(1,2,4) | select @{Name=\"Name\" ;expression={$fwProfileTypes[$_]}}, @{Name=\"Enabled\" ;expression={$fw.FireWallEnabled($_)}} | Format-List Name, Enabled");
	}
	else
	{
		profilesParameters << QStringLiteral("Get-NetFirewallProfile");
	}

	connect(&mFirewallProfilesProcess, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished), this, &DiagnosisFirewallDetection::onProfilesDone);
	connect(&mFirewallProfilesProcess, &QProcess::errorOccurred, this, &DiagnosisFirewallDetection::onProfilesError);

	mFirewallProfilesProcess.start(powershellCommand, profilesParameters);
	mFirewallProfilesProcess.closeWriteChannel();

	QStringList installedFirewallSoftwareParameters;
	installedFirewallSoftwareParameters << QStringLiteral("-Command");
	installedFirewallSoftwareParameters << QStringLiteral("Get-WmiObject -Namespace root\\SecurityCenter2 -Class FirewallProduct");

	connect(&mInstalledFirewallSoftwareProcess, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished), this, &DiagnosisFirewallDetection::onInstalledFirewallSoftwareDone);
	connect(&mInstalledFirewallSoftwareProcess, &QProcess::errorOccurred, this, &DiagnosisFirewallDetection::onInstalledFirewallSoftwareError);

	mInstalledFirewallSoftwareProcess.start(powershellCommand, installedFirewallSoftwareParameters);
	mInstalledFirewallSoftwareProcess.closeWriteChannel();

#endif
}


FirewallSoftware::FirewallSoftware(QString pName, bool pEnabled, bool pUpToDate)
	: mName(pName)
	, mEnabled(pEnabled)
	, mUpToDate(pUpToDate)
{
}


FirewallProfile::FirewallProfile(QString pName, bool pEnabled)
	: mName(pName)
	, mEnabled(pEnabled)
{
}
