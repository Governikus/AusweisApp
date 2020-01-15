/*!
 * \copyright Copyright (c) 2018-2020 Governikus GmbH & Co. KG, Germany
 */

#include "DiagnosisItem.h"

using namespace governikus;

DiagnosisItem::DiagnosisItem(const QString& pText)
	: QObject()
	, QEnableSharedFromThis()
	, mText(pText)
	, mChildren()
{
}


const QString& DiagnosisItem::getText() const
{
	return mText;
}


void DiagnosisItem::addChild(const QSharedPointer<DiagnosisItem>& pChild)
{
	mChildren << pChild;
	pChild->setParent(sharedFromThis());
}


void DiagnosisItem::setParent(const QSharedPointer<DiagnosisItem>& pParent)
{
	mParent = pParent;
}


void DiagnosisItem::clearChildren()
{
	mChildren.clear();
}


const QSharedPointer<DiagnosisItem> DiagnosisItem::getChild(int pRow) const
{
	return mChildren.at(pRow);
}


int DiagnosisItem::childCount() const
{
	return mChildren.length();
}


QSharedPointer<DiagnosisItem> DiagnosisItem::parentItem()
{
	return mParent;
}


int DiagnosisItem::row() const
{
	if (!mParent.isNull())
	{
		return mParent->getIndexOf(this);
	}

	return 0;
}


int DiagnosisItem::getIndexOf(const DiagnosisItem* const pChild) const
{
	for (int i = 0; i < mChildren.length(); ++i)
	{
		auto child = mChildren.at(i);
		if (child.data() == pChild)
		{
			return i;
		}
	}
	return -1;
}


void DiagnosisItem::appendPlaintextContent(QStringList& pOutput, const QString& pPrefix)
{
	pOutput << pPrefix + getText();
	const QString childPrefix = pPrefix + QLatin1Char(' ');
	for (const auto& child : qAsConst(mChildren))
	{
		child->appendPlaintextContent(pOutput, childPrefix);
	}
}
