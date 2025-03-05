/**
 * Copyright (c) 2016-2025 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "Env.h"
#include "ReaderManagerPluginInfo.h"
#include "SingletonCreator.h"
#include "context/WorkflowContext.h"

#include <QObject>
#include <QSharedPointer>
#include <QVariant>
#include <QtQml/qqmlregistration.h>

#if __has_include("SmartManager.h")
	#define NO_SMART_CONST
#else
	#define NO_SMART_CONST const
#endif


class test_SmartModel;


namespace governikus
{

class SmartModel
	: public QObject
	, public SingletonCreator<SmartModel>
{
	Q_OBJECT
	Q_CLASSINFO("RegisterEnumClassesUnscoped", "false")
	QML_ELEMENT
	QML_SINGLETON

	friend class Env;
	friend class ::test_SmartModel;

	Q_PROPERTY(State state READ getState NOTIFY fireStateChanged)
	Q_PROPERTY(QString errorString READ getErrorString NOTIFY fireErrorStringChanged)
	Q_PROPERTY(int progress READ getProgress NOTIFY fireProgressChanged)
	Q_PROPERTY(bool isScanRunning READ isScanRunning NOTIFY fireScanRunningChanged)

	public:
		enum class State
		{
			UPDATING_STATUS,
			UNAVAILABLE,
			NO_PROVISIONING,
			NO_PERSONALIZATION,
			UNUSABLE,
			READY
		};
		Q_ENUM(State)

	private:
		SmartModel();
		State mStatus;
		QString mErrorString;
		CardInfo mCachedCardInfo;
		int mProgress;

		void updateStatus() NO_SMART_CONST;
		void setErrorString(const QString& pError);
		void updatePinStatus();
		void setProgress(int pProgress);
		void setStatus(State pNewStatus);

		[[nodiscard]] bool isScanRunning() const;

	private Q_SLOTS:
		void onUpdateSupportInfoDone(const QVariant& pResult);
		void onDeletePersonalizationDone(const QVariant& pResult);
		void onDeleteSmartDone(const QVariant& pResult) NO_SMART_CONST;
		void onUpdateStatusDone(const QVariant& pResult);
		void onUpdatePinStatusDone(const ReaderInfo& pInfo);
		void onStatusChanged(const ReaderManagerPluginInfo& pInfo);

	public:
		State getState() const;
		[[nodiscard]] QString getErrorString() const;
		[[nodiscard]] int getProgress() const;

		void workflowFinished(QSharedPointer<WorkflowContext> pContext);

		[[nodiscard]] MobileEidType getMobileEidType() const;

		Q_INVOKABLE void updateSupportInfo() NO_SMART_CONST;
		Q_INVOKABLE void deletePersonalization() NO_SMART_CONST;
		Q_INVOKABLE void deleteSmart() NO_SMART_CONST;

	Q_SIGNALS:
		void fireStateChanged();
		void fireDeleteSmartDone();
		void fireDeletePersonalizationDone(bool pSuccess);
		void fireProgressChanged();
		void fireScanRunningChanged();
		void fireErrorStringChanged();
};

} // namespace governikus
