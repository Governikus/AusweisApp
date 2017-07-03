#include "fvignoredversions.h"
#include "fvversioncomparator.h"
#include <QSettings>
#include <QApplication>
#include <string>

// QSettings key for the latest skipped version
#define FV_IGNORED_VERSIONS_LATEST_SKIPPED_VERSION_KEY	"FVLatestSkippedVersion"


FVIgnoredVersions::FVIgnoredVersions(QObject *pParent) :
	QObject(pParent)
{
	// noop
}

bool FVIgnoredVersions::VersionIsIgnored(const QString& version)
{
	// We assume that variable 'version' contains either:
	//	1) The current version of the application (ignore)
	//	2) The version that was skipped before and thus stored in QSettings (ignore)
	//	3) A newer version (don't ignore)
	// 'version' is not likely to contain an older version in any case.

//	if (version == FV_APP_VERSION) {
//		return true;
//	}

	QSettings settings(QSettings::NativeFormat,
					   QSettings::UserScope,
					   QApplication::organizationName(),
					   QApplication::applicationName());

	if (settings.contains(FV_IGNORED_VERSIONS_LATEST_SKIPPED_VERSION_KEY)) {
		QString lastSkippedVersion = settings.value(FV_IGNORED_VERSIONS_LATEST_SKIPPED_VERSION_KEY).toString();
		if (version == lastSkippedVersion) {
			// Implicitly skipped version - skip
			return true;
		}
	}

	std::string currentAppVersion = QApplication::applicationVersion().toStdString();
	std::string suggestedVersion = version.toStdString();
	if (FvVersionComparator::CompareVersions(currentAppVersion, suggestedVersion) == FvVersionComparator::kAscending) {
		// Newer version - do not skip
		return false;
	}

	// Fallback - skip
	return true;
}

void FVIgnoredVersions::IgnoreVersion(const QString& version)
{
	if (version == QApplication::applicationVersion()) {
		// Don't ignore the current version
		return;
	}

	if (version.isEmpty()) {
		return;
	}

	QSettings settings(QSettings::NativeFormat,
					   QSettings::UserScope,
					   QApplication::organizationName(),
					   QApplication::applicationName());

	settings.setValue(FV_IGNORED_VERSIONS_LATEST_SKIPPED_VERSION_KEY, version);

	return;
}
