/*!
 * \brief Model for accessing PIN, CAN, PUK, according to the
 * currently active workflow.
 *
 * \copyright Copyright (c) 2016 Governikus GmbH & Co. KG
 */

#pragma once

#include <QObject>
#include <QSharedPointer>

namespace governikus
{

class WorkflowContext;

class NumberModel
	: public QObject
{
	Q_OBJECT
	Q_PROPERTY(QString can READ getCan WRITE setCan NOTIFY fireCanChanged)
	Q_PROPERTY(QString pin READ getPin WRITE setPin NOTIFY firePinChanged)
	Q_PROPERTY(QString newPin READ getNewPin WRITE setNewPin NOTIFY fireNewPinChanged)
	Q_PROPERTY(QString puk READ getPuk WRITE setPuk NOTIFY firePukChanged)
	Q_PROPERTY(QString inputError READ getInputError NOTIFY fireInputErrorChanged)
	Q_PROPERTY(int retryCounter READ getRetryCounter NOTIFY fireReaderInfoChanged)
	Q_PROPERTY(bool extendedLengthApdusUnsupported READ isExtendedLengthApdusUnsupported NOTIFY fireReaderInfoChanged)
	Q_PROPERTY(bool pinDeactivated READ isPinDeactivated NOTIFY fireReaderInfoChanged)
	Q_PROPERTY(bool cardConnected READ isCardConnected NOTIFY fireReaderInfoChanged)

	QSharedPointer<WorkflowContext> mContext;

	private Q_SLOTS:
		void onCardConnectionChanged();

	public:
		NumberModel(QObject* pParent = nullptr);
		virtual ~NumberModel();

		void resetContext(const QSharedPointer<WorkflowContext>& pContext = QSharedPointer<WorkflowContext>());
		Q_INVOKABLE void continueWorkflow();

		QString getCan() const;
		void setCan(const QString& pCan);

		QString getPin() const;
		void setPin(const QString& pPin);

		QString getNewPin() const;
		void setNewPin(const QString& pNewPin);

		QString getPuk() const;
		void setPuk(const QString& pPuk);

		QString getInputError() const;

		int getRetryCounter() const;

		bool isExtendedLengthApdusUnsupported() const;

		bool isPinDeactivated() const;

		bool isCardConnected() const;

	private Q_SLOTS:
		void onReaderInfoChanged(const QString& pReaderName);

	Q_SIGNALS:
		void fireCanChanged();
		void firePinChanged();
		void fireNewPinChanged();
		void firePukChanged();
		void fireInputErrorChanged();
		void fireReaderInfoChanged();
};


} /* namespace governikus */
