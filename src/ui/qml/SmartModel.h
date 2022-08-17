/*!
 * \brief Model implementation for Smart-eID status information.
 *
 * \copyright Copyright (c) 2016-2022 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "Env.h"
#include "ReaderInfo.h"
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
		CardInfo mCachedCardInfo;
		int mProgress;

		void updateStatus();
		void updatePinStatus();
		void setProgress(int pProgress);
		void setStatus(QmlSmartState pNewStatus);

		bool isScanRunning() const;

	private Q_SLOTS:
		void onDeletePersonalizationDone(const QVariant& pResult);
		void onDeleteSmartDone(const QVariant& pResult);
		void onUpdateStatusDone(const QVariant& pResult);
		void onUpdatePinStatusDone(const ReaderManagerPlugInInfo& pInfo);
		void onStatusChanged(const ReaderManagerPlugInInfo& pInfo);

	public:
		QmlSmartState getSmartState() const;
		int getProgress() const;

		void workflowFinished(QSharedPointer<WorkflowContext> pContext);

		[[nodiscard]] MobileEidType getMobileEidType();

		Q_INVOKABLE void deletePersonalization();
		Q_INVOKABLE void deleteSmart();

	Q_SIGNALS:
		void fireSmartStateChanged();
		void fireDeleteSmartDone();
		void fireDeletePersonalizationDone();
		void fireProgressChanged();
		void fireScanRunningChanged();
};

} // namespace governikus
