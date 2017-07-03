#ifndef FVIGNOREDVERSIONS_H
#define FVIGNOREDVERSIONS_H

#include <QObject>

class FVIgnoredVersions : public QObject
{
	Q_OBJECT

public:
	static bool VersionIsIgnored(const QString& version);
	static void IgnoreVersion(const QString& version);

private:
	explicit FVIgnoredVersions(QObject *pParent = nullptr);

};

#endif // FVIGNOREDVERSIONS_H
