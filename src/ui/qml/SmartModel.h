/**
 * Copyright (c) 2016-2024 Governikus GmbH & Co. KG, Germany
 */

/*!
 * \brief Model implementation for Smart-eID status information.
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

		void updateStatus();
		void setErrorString(const QString& pError);
		void updatePinStatus();
		void setProgress(int pProgress);
		void setStatus(State pNewStatus);

		[[nodiscard]] bool isScanRunning() const;

	private Q_SLOTS:
		void onUpdateSupportInfoDone(const QVariant& pResult);
		void onDeletePersonalizationDone(const QVariant& pResult);
		void onDeleteSmartDone(const QVariant& pResult);
		void onUpdateStatusDone(const QVariant& pResult);
		void onUpdatePinStatusDone(const ReaderInfo& pInfo);
		void onStatusChanged(const ReaderManagerPluginInfo& pInfo);

	public:
		State getState() const;
		[[nodiscard]] QString getErrorString() const;
		[[nodiscard]] int getProgress() const;

		void workflowFinished(QSharedPointer<WorkflowContext> pContext);

		[[nodiscard]] MobileEidType getMobileEidType() const;

		Q_INVOKABLE void updateSupportInfo();
		Q_INVOKABLE void deletePersonalization();
		Q_INVOKABLE void deleteSmart();

	Q_SIGNALS:
		void fireStateChanged();
		void fireDeleteSmartDone();
		void fireDeletePersonalizationDone(bool pSuccess);
		void fireProgressChanged();
		void fireScanRunningChanged();
		void fireErrorStringChanged();
};

} // namespace governikus
