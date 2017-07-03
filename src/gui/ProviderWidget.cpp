/*!
 * ProviderWidget.cpp
 *
 * \copyright Copyright (c) 2014 Governikus GmbH & Co. KG
 */

#include "ProviderWidget.h"

#include "AppSettings.h"
#include "ProviderService.h"
#include "ProviderSettings.h"
#include "generic/ListCheckItemWidget.h"
#include "generic/ListItem.h"
#include "generic/ListItemIconLeft.h"
#include "generic/ListItemIconRight.h"
#include "generic/ListItemSubTitle.h"
#include "generic/ListItemTitle.h"
#include "ui_ProviderWidget.h"

#include <QDebug>
#include <QDesktopServices>
#include <QPainter>
#include <QPushButton>
#include <QRadioButton>
#include <QUrl>

using namespace governikus;

ProviderWidget::ProviderWidget(QWidget* pParent)
	: Page(tr("Provider"), pParent)
	, mUi(new Ui::ProviderWidget())
{
	mUi->setupUi(this);

	connect(mUi->providerSearch, &QLineEdit::textChanged, this, &ProviderWidget::searchProvider);
	connect(ProviderService::getSharedInstance(), &ProviderService::fireUpdateFinished, this, &ProviderWidget::onProviderChanged);

	fill();
}


ProviderWidget::~ProviderWidget()
{
}


void ProviderWidget::onProviderChanged()
{
	mUi->providerTableWidget->clear();
	fill();
}


void ProviderWidget::fill()
{
	qDebug() << "add provider for desktop widgets.";
	QStringList header;
	header.append(tr("Name"));
	header.append(tr("Address"));

	ProviderSettings& providerSettings = AppSettings::getInstance().getProviderSettings();
	providerSettings.load();

	mUi->providerTableWidget->setColumnCount(header.count());
	mUi->providerTableWidget->setHorizontalHeaderLabels(header);
	mUi->providerTableWidget->setRowCount(providerSettings.getProviders().size());

	int row = 0;
	for (const Provider& provider : providerSettings.getProviders())
	{
		QLabel* providerName = new QLabel(provider.getShortName());
		providerName->setFocusPolicy(Qt::TabFocus);
		providerName->setToolTip(providerName->text());
		providerName->setTextFormat(Qt::RichText);
		providerName->setMargin(3);
		mUi->providerTableWidget->setCellWidget(row, 0, providerName);

		QLabel* providerLink = new QLabel("<a href=\"" + provider.getAddress() + "\">" + provider.getAddress() + "</a>");
		providerLink->setToolTip(providerLink->text());
		providerLink->setFocusPolicy(Qt::TabFocus);
		providerLink->setTextFormat(Qt::RichText);
		providerLink->setTextInteractionFlags(Qt::TextBrowserInteraction);
		providerLink->setOpenExternalLinks(true);
		providerLink->setMargin(3);
		mUi->providerTableWidget->setCellWidget(row, 1, providerLink);

		++row;
	}

	mUi->providerTableWidget->verticalHeader()->setVisible(false); //Hide row number
	mUi->providerTableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers); //Not allowed to change content
	mUi->providerTableWidget->setAlternatingRowColors(true); //Grey and white alternating row
	mUi->providerTableWidget->resizeColumnsToContents();

	mUi->noResultWidget->setVisible(false);
}


void ProviderWidget::searchProvider()
{
	QString searchText = mUi->providerSearch->text().trimmed();
	mUi->providerTableWidget->setVisible(true);
	mUi->noResultWidget->setVisible(false);

	bool anyMatch = false;

	for (int i = 0; i < mUi->providerTableWidget->rowCount(); ++i)
	{
		bool match = false;
		for (int j = 0; j < mUi->providerTableWidget->columnCount(); ++j)
		{

			if (qobject_cast<QLabel*>(mUi->providerTableWidget->cellWidget(i, j))->text().contains(searchText, Qt::CaseInsensitive))
			{
				match = true;
				anyMatch = true;
				break;
			}
		}
		mUi->providerTableWidget->setRowHidden(i, !match);
	}

	mUi->providerTableWidget->setVisible(anyMatch);
	mUi->noResultWidget->setVisible(!anyMatch);
}


void ProviderWidget::paintEvent(QPaintEvent*)
{
	QStyleOption opt;
	opt.init(this);
	QPainter p(this);
	style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}
