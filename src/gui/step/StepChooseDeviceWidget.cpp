/*!
 * StepChooseDeviceWidget.cpp
 *
 * \copyright Copyright (c) 2014 Governikus GmbH & Co. KG
 */

#include "StepChooseDeviceWidget.h"
#include "ui_StepChooseDeviceWidget.h"

#include "generic/ListItem.h"
#include "generic/ListItemIconLeft.h"
#include "generic/ListItemIconRight.h"
#include "generic/ListItemSubTitle.h"
#include "generic/ListItemTitle.h"
#include "ReaderManager.h"

#include <QLoggingCategory>
#include <QMovie>

using namespace governikus;

Q_DECLARE_LOGGING_CATEGORY(gui)

StepChooseDeviceWidget::StepChooseDeviceWidget(QWidget* pParent)
	: Page(QStringLiteral("Choose Device"), pParent)
	, mUi(new Ui::StepChooseDeviceWidget())
	, mMovieLabel(new QLabel())
	, mInitialized(false)
{
	mMovieLabel->setMovie(new QMovie(QStringLiteral(":/images/busy_animation.gif")));
	mMovieLabel->setAlignment(Qt::AlignCenter);
	mMovieLabel->setMargin(8);
	mUi->setupUi(this);
	mUi->verticalLayout_3->addWidget(mMovieLabel.data());

	connect(mUi->scanButton, &QPushButton::clicked, this, &StepChooseDeviceWidget::onScanButtonClicked);

	mUi->scanDescriptionLabel->setText(tr("Please connect your tablet/smartphone with the Reiner SCT cyberJack wave now. You can find a detailled description in the online help."));
}


StepChooseDeviceWidget::~StepChooseDeviceWidget()
{
	if (!mMovieLabel.isNull() && mMovieLabel->movie() != nullptr)
	{
		delete mMovieLabel->movie();
		mMovieLabel->setMovie(nullptr);
	}
	disconnect(&ReaderManager::getInstance(), &ReaderManager::fireReaderAdded, this, &StepChooseDeviceWidget::onReaderAdded);
	disconnect(&ReaderManager::getInstance(), &ReaderManager::fireReaderRemoved, this, &StepChooseDeviceWidget::onReaderRemoved);
	disconnect(&ReaderManager::getInstance(), &ReaderManager::fireCardInserted, this, &StepChooseDeviceWidget::onCardInserted);
	disconnect(&ReaderManager::getInstance(), &ReaderManager::fireCardRemoved, this, &StepChooseDeviceWidget::onCardRemoved);
}


void StepChooseDeviceWidget::setState(StepChooseDeviceUi::State pState)
{
	switch (pState)
	{
		case StepChooseDeviceUi::State::INITIAL:
		{
			Q_EMIT setForwardButtonState(ButtonState::DISABLED);
			// clear the device list
			while (mUi->devicesList->count())
			{
				QWidget* readerWidget = mUi->devicesList->itemAt(0)->widget();
				mUi->devicesList->removeWidget(readerWidget);
				delete readerWidget;
			}
			// add currently known readers
			const auto allReaders = ReaderManager::getInstance().getReaderInfos();
			for (const auto& info : allReaders)
			{
				onReaderAdded(info.getName());
			}
			if (!mInitialized)
			{
				mInitialized = true;
				// lazy initialization because the  ReaderManager does not exist yet on construction time
				connect(&ReaderManager::getInstance(), &ReaderManager::fireReaderAdded, this, &StepChooseDeviceWidget::onReaderAdded);
				connect(&ReaderManager::getInstance(), &ReaderManager::fireReaderRemoved, this, &StepChooseDeviceWidget::onReaderRemoved);
				connect(&ReaderManager::getInstance(), &ReaderManager::fireCardInserted, this, &StepChooseDeviceWidget::onCardInserted);
				connect(&ReaderManager::getInstance(), &ReaderManager::fireCardRemoved, this, &StepChooseDeviceWidget::onCardRemoved);
			}
			return;
		}

		case StepChooseDeviceUi::State::SCAN_STARTED:

			mUi->scanButton->setEnabled(false);
			mMovieLabel->setVisible(true);
			mMovieLabel->movie()->start();
			return;

		case StepChooseDeviceUi::State::FINISHED:
		/* no break - fall through*/

		case StepChooseDeviceUi::State::SCAN_STOPPED:
			mUi->scanButton->setEnabled(true);
			mMovieLabel->setVisible(false);
			mMovieLabel->movie()->stop();
			return;
	}
}


void StepChooseDeviceWidget::onReaderAdded(const QString& pReaderName)
{
	ListItem* listItemWidget = new ListItem(this);
	listItemWidget->installEventFilter(this);
	listItemWidget->setObjectName(QStringLiteral("listItemWidget"));

	QHBoxLayout* listItemWidgetLayout = new QHBoxLayout(listItemWidget);
	listItemWidgetLayout->setSpacing(0);
	listItemWidgetLayout->setObjectName(QStringLiteral("listItemWidgetLayout"));
	listItemWidgetLayout->setContentsMargins(0, 0, 0, 0);

	QFrame* verticalFrame = new QFrame(listItemWidget);
	verticalFrame->setObjectName(QStringLiteral("verticalFrame"));

	QVBoxLayout* titleSubtitelLayout = new QVBoxLayout(verticalFrame);
	titleSubtitelLayout->setSpacing(0);
	titleSubtitelLayout->setObjectName(QStringLiteral("titleSubtitelLayout"));
	titleSubtitelLayout->setContentsMargins(0, 0, 0, 0);

	ListItemTitle* titleLabel = new ListItemTitle(verticalFrame);
	titleLabel->setObjectName(QStringLiteral("titleLabel"));
	titleLabel->setText(pReaderName);

	listItemWidget->setProperty("ReaderName", pReaderName);

	titleSubtitelLayout->addWidget(titleLabel);

	ListItemSubTitle* subTitleLabel = new ListItemSubTitle(verticalFrame);
	subTitleLabel->setObjectName(QStringLiteral("subTitleLabel"));
	subTitleLabel->setAlignment(Qt::AlignLeading | Qt::AlignLeft | Qt::AlignVCenter);

	titleSubtitelLayout->addWidget(subTitleLabel);

	listItemWidgetLayout->addWidget(verticalFrame);

	ListItemIconRight* listItemIconRight = new ListItemIconRight(listItemWidget);
	listItemIconRight->setObjectName(QStringLiteral("listItemIconRight"));

	QSizePolicy listItemSizePolicy(QSizePolicy::Maximum, QSizePolicy::Preferred);
	listItemSizePolicy.setHorizontalStretch(0);
	listItemSizePolicy.setVerticalStretch(0);
	listItemSizePolicy.setHeightForWidth(listItemIconRight->sizePolicy().hasHeightForWidth());
	listItemIconRight->setSizePolicy(listItemSizePolicy);
	listItemIconRight->setVisible(false);
	listItemWidgetLayout->addWidget(listItemIconRight);

	mUi->devicesList->addWidget(listItemWidget);

	ReaderInfo info = ReaderManager::getInstance().getReaderInfo(pReaderName);
	if (!info.isConnected())
	{
		setSubtitleOfReader(pReaderName, tr("Not connected"));
		setIconVisible(pReaderName, true);
	}
	else if (info.getCardType() == CardType::EID_CARD)
	{
		onCardInserted(pReaderName);
	}
	else
	{
		onCardRemoved(pReaderName);
	}
}


void StepChooseDeviceWidget::onReaderRemoved(const QString& pReaderName)
{
	if (QWidget* readerItem = getWidgetWithProperty("ReaderName", pReaderName))
	{
		mUi->devicesList->removeWidget(readerItem);
		delete readerItem;
	}
}


void StepChooseDeviceWidget::onCardInserted(const QString& pReaderName)
{
	if (ReaderManager::getInstance().getReaderInfo(pReaderName).getCardType() == CardType::EID_CARD)
	{
		setSubtitleOfReader(pReaderName, tr("ID card found"));
		setIconVisible(pReaderName, true);
		qDebug() << "choose reader" << pReaderName;
		Q_EMIT fireUiFinished(pReaderName);
	}
}


void StepChooseDeviceWidget::onCardRemoved(const QString& pReaderName)
{
	if (ReaderManager::getInstance().getReaderInfo(pReaderName).isConnected())
	{
		setSubtitleOfReader(pReaderName, tr("No ID card found"));
		setIconVisible(pReaderName, false);
	}
	else
	{
		setSubtitleOfReader(pReaderName, tr("Not connected"));
		setIconVisible(pReaderName, true);
	}
}


void StepChooseDeviceWidget::setSubtitleOfReader(const QString& pReaderName, const QString& pSubTitle)
{
	if (QWidget* readerItem = getWidgetWithProperty("ReaderName", pReaderName))
	{
		ListItemSubTitle* subTitle = readerItem->findChild<ListItemSubTitle*>(QStringLiteral("subTitleLabel"));

		for (int i = 0; i < mUi->devicesList->count(); ++i)
		{
			if (QObject* deviceWidget = mUi->devicesList->itemAt(i)->widget())
			{
				if (deviceWidget->property("ReaderName") == pReaderName)
				{
					if (subTitle != nullptr)
					{
						subTitle->setText(pSubTitle);
						return;
					}
				}
			}
		}
	}
	qWarning() << "No reader found to adjust subtitle" << pReaderName;
}


void StepChooseDeviceWidget::setIconVisible(const QString& pReaderName, bool pVisible)
{

	for (int i = 0; i < mUi->devicesList->count(); ++i)
	{
		if (QObject* deviceWidget = mUi->devicesList->itemAt(i)->widget())
		{
			if (deviceWidget->property("ReaderName") == pReaderName)
			{
				ListItemIconRight* listIcon = deviceWidget->findChild<ListItemIconRight*>(QStringLiteral("listItemIconRight"));
				if (listIcon != nullptr)
				{
					listIcon->setVisible(pVisible);
					return;
				}
			}
		}
	}
	qWarning() << "No reader found to adjust icon visibility" << pReaderName;
}


void StepChooseDeviceWidget::onScanButtonClicked()
{
	mUi->devicesList->setEnabled(false);

	Q_EMIT fireScanButtonClicked();
}


QWidget* StepChooseDeviceWidget::getWidgetWithProperty(const char* pName, const QString& pValue) const
{
	for (int i = 0; i < mUi->devicesList->count(); ++i)
	{
		if (QWidget* widget = mUi->devicesList->itemAt(i)->widget())
		{
			if (widget->property(pName).toString() == pValue)
			{
				return widget;
			}
		}
	}
	qCritical() << "No widget found with property " << pName << "=" << pValue;
	return nullptr;
}


bool StepChooseDeviceWidget::eventFilter(QObject* pWatched, QEvent* pEvent)
{
	switch (pEvent->type())
	{
		case QEvent::MouseButtonPress:
			if (pWatched->inherits("governikus::ListItem"))
			{
				if (!qobject_cast<ListItem*>(pWatched)->property("ReaderName").isNull() && qobject_cast<ListItem*>(pWatched)->property("ReaderName").isValid())
				{
					QString readerName = qobject_cast<ListItem*>(pWatched)->property("ReaderName").toString();
					ReaderInfo info = ReaderManager::getInstance().getReaderInfo(readerName);
					if (!info.isConnected())
					{
						qDebug() << "Connect reader" << readerName;
						ReaderManager::getInstance().connectReader(readerName);
					}
				}
			}
			return true;

		default:
			return false;
	}
}
