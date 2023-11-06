/**
 * Copyright (c) 2016-2023 Governikus GmbH & Co. KG, Germany
 */

/*!
 * \brief Model implementation for Smart-eID status information.
 */

#pragma once

#include "Env.h"
#include "ReaderManagerPlugInInfo.h"
#include "context/WorkflowContext.h"

#include <QObject>
#include <QSharedPointer>
#include <QVariant>


class test_SmartModel;


namespace governikus
{

class SmartModel
	: public QObject
{
	Q_OBJECT
	friend class Env;
	friend class ::test_SmartModel;

	Q_PROPERTY(QmlSmartState smartState READ getSmartState NOTIFY fireSmartStateChanged)
	Q_PROPERTY(QString errorString READ getErrorString NOTIFY fireErrorStringChanged)
	Q_PROPERTY(int progress READ getProgress NOTIFY fireProgressChanged)
	Q_PROPERTY(bool isScanRunning READ isScanRunning NOTIFY fireScanRunningChanged)

	public:
		enum class QmlSmartState
		{
			SMART_UPDATING_STATUS,
			SMART_UNAVAILABLE,
			SMART_NO_PROVISIONING,
			SMART_NO_PERSONALIZATION,
			SMART_UNUSABLE,
			SMART_READY
		};
		Q_ENUM(QmlSmartState)

	private:
		SmartModel();
		QmlSmartState mStatus;
		QString mErrorString;
		CardInfo mCachedCardInfo;
		int mProgress;

		void updateStatus();
		void setErrorString(const QString& pError);
		void updatePinStatus();
		void setProgress(int pProgress);
		void setStatus(QmlSmartState pNewStatus);

		[[nodiscard]] bool isScanRunning() const;

	private Q_SLOTS:
		void onUpdateSupportInfoDone(const QVariant& pResult);
		void onDeletePersonalizationDone(const QVariant& pResult);
		void onDeleteSmartDone(const QVariant& pResult);
		void onUpdateStatusDone(const QVariant& pResult);
		void onUpdatePinStatusDone(const ReaderInfo& pInfo);
		void onStatusChanged(const ReaderManagerPlugInInfo& pInfo);

	public:
		QmlSmartState getSmartState() const;
		[[nodiscard]] QString getErrorString() const;
		[[nodiscard]] int getProgress() const;

		void workflowFinished(QSharedPointer<WorkflowContext> pContext);

		[[nodiscard]] MobileEidType getMobileEidType() const;

		Q_INVOKABLE void updateSupportInfo();
		Q_INVOKABLE void deletePersonalization();
		Q_INVOKABLE void deleteSmart();

	Q_SIGNALS:
		void fireSmartStateChanged();
		void fireDeleteSmartDone();
		void fireDeletePersonalizationDone(bool pSuccess);
		void fireProgressChanged();
		void fireScanRunningChanged();
		void fireErrorStringChanged();
};

} // namespace governikus
