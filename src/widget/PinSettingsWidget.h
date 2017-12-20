/*!
 * \brief Widget for the PIN settings.
 *
 * \copyright Copyright (c) 2014-2017 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include <QPointer>
#include <QSharedPointer>
#include <QWidget>


namespace Ui
{
class PinSettingsWidget;
}

namespace governikus
{

class RandomPinDialog;
class ReaderInfo;

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

		void setInProgress(bool pInProgress);

		QString getCan() const;
		QString getPin() const;
		QString getPuk() const;
		QString getNewPin() const;


		Mode getMode() const
		{
			return mMode;
		}


		void setMode(Mode pMode);
		QString getButtonText() const;
		void setUseScreenKeyboard(bool pUseScreenKeyboard);

		bool getPinButtonEnabled() const;

	Q_SIGNALS:
		void firePinButtonEnabledUpdated(bool pEnabled);

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
		void onScanTimeout();

		void onScanButtonClicked();
		void onUiFinished(const QString& pReaderName);

	private:
		void setupPinBasicPage(const ReaderInfo& pReaderInfo);
		void setupPinComfortPage(const ReaderInfo& pReaderInfo);
		void setupPinSuccessfullyChangedPage(const ReaderInfo& pReaderInfo);
		void setupChangePinHeader(int pRetryCounter, bool pIsBasicReader);

		bool isCanFieldVisible() const;

		QVector<ReaderInfo> getReaderWithNPA(const QVector<ReaderInfo>& pReaderInfos);
		void updateReadersWithoutNPA(const QVector<ReaderInfo>& pReaderInfos);
		bool updateReadersForOneNPA(const ReaderInfo& pReaderInfo);

		void fillInfoDescription(const QString& pTitle, const QString& pMessage);

		QScopedPointer<Ui::PinSettingsWidget> mUi;
		Mode mMode;
		int mRetryCounter;
		bool mPinDeactivated;
		bool mPinButtonEnabled;

		QString mPinSettingsInfoTitle;
		QString mPinSettingsInfoDescription;
		QPointer<RandomPinDialog> mRandomPinDialog;
};

} /* namespace governikus */
