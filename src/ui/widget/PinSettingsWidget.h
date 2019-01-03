/*!
 * \brief Widget for the PIN settings.
 *
 * \copyright Copyright (c) 2014-2018 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "context/ChangePinContext.h"
#include "RandomPinDialog.h"
#include "ReaderInfo.h"

#include <QPointer>
#include <QSharedPointer>
#include <QWidget>

namespace Ui
{
class PinSettingsWidget;
} // namespace Ui

namespace governikus
{

class PinSettingsWidget
	: public QWidget
{
	Q_OBJECT

	public:
		static const int SCALEWIDTH = 200;

		enum class Mode
		{
			Normal,
			AfterPinChange,
			AfterPinUnblock,
		};

	public:
		PinSettingsWidget(QWidget* pParent = nullptr);
		virtual ~PinSettingsWidget() override;

		void setContext(const QSharedPointer<ChangePinContext>& pContext);
		void cancelWorkflow();
		void continueWorkflow();

		QString getCan() const;
		QString getPin() const;
		QString getPuk() const;
		QString getNewPin() const;
		void updatePasswordFields();
		void updatePinButton(bool pEnabled);
		void clearPasswords();
		void setPasswordFocus();


		Mode getMode() const
		{
			return mMode;
		}


		void setMode(Mode pMode);
		QString getButtonText() const;
		void setUseScreenKeyboard(bool pUseScreenKeyboard);

		bool getPinButtonEnabled() const;
		bool getCancelButtonEnabled() const;

	Q_SIGNALS:
		void fireButtonEnabledUpdated();

	public Q_SLOTS:
		void updateReaders();
		void onBackspacePressedOnApply();

	protected:
		virtual void paintEvent(QPaintEvent*) override;
		virtual void showEvent(QShowEvent* pEvent) override;
		virtual void hideEvent(QHideEvent* pEvent) override;
		virtual void changeEvent(QEvent* pEvent) override;

	private Q_SLOTS:
		void onCanTextEdited(const QString& pText = QString());
		void onOldPinTextEdited(const QString& pText = QString());
		void onNewPinTextEdited(const QString& pText = QString());
		void onRepeatNewPinTextEdited(const QString& pText = QString());
		void onPukTextEdited(const QString& pText = QString());
		void focusPUK();
		void focusPIN();
		void focusCAN();
		void onRandomPinButtonClicked();
		void onRandomPukButtonClicked();

	private:
		void updateCancelButton(bool pEnabled);
		void setUserInputEnabled(bool pEnabled);
		void setupPinBasicPage(const ReaderInfo& pReaderInfo);
		void setupPinComfortPage(const ReaderInfo& pReaderInfo);
		void setupPinSuccessfullyChangedPage(const ReaderInfo& pReaderInfo);
		void setupChangePinHeader(int pRetryCounter, bool pIsBasicReader);

		bool isCanFieldVisible() const;
		bool isPukFieldVisible() const;

		QVector<ReaderInfo> getReaderWithNPA(const QVector<ReaderInfo>& pReaderInfos);
		void updateReadersWithoutNPA(const QVector<ReaderInfo>& pReaderInfos);
		bool updateReadersForOneNPA(const ReaderInfo& pReaderInfo);

		void fillInfoDescription(const QString& pTitle, const QString& pMessage);

		QScopedPointer<Ui::PinSettingsWidget> mUi;
		Mode mMode;
		QSharedPointer<ChangePinContext> mContext;
		int mRetryCounter;
		bool mPinDeactivated;
		bool mPinButtonEnabled;
		bool mCancelButtonEnabled;

		QString mPinSettingsInfoTitle;
		QString mPinSettingsInfoDescription;
		QPointer<RandomPinDialog> mRandomPinDialog;
};

} // namespace governikus
