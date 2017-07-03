/*!
 * LayoutBuilder.h
 *
 * \brief Support classes for building layouts.
 *
 * \copyright Copyright (c) 2014 Governikus GmbH & Co. KG
 */

#pragma once

#include <QBoxLayout>
#include <QGridLayout>
#include <QWidget>

namespace governikus
{

namespace LayoutBuilder
{

struct Dummy
{
};

template<typename Delegate, typename ParentBuilder, typename ActualClass>
class Base
{
	public:
		template<typename ... Arguments>
		Base(Arguments ... pArguments)
			: mDelegate(pArguments ...)
			, mParent(nullptr)
		{
		}


		void setParent(ParentBuilder* pParent)
		{
			mParent = pParent;
		}


		template<typename ... Arguments>
		ActualClass& add(QWidget* pWidget, Arguments ... pArguments)
		{
			mDelegate.addWidget(pWidget, pArguments ...);
			return *static_cast<ActualClass*>(this);
		}


		template<typename ... Arguments>
		ActualClass& add(const QString& pWidgetName, QWidget* pWidget, Arguments ... pArguments)
		{
			pWidget->setObjectName(pWidgetName);
			mDelegate.addWidget(pWidget, pArguments ...);
			return *static_cast<ActualClass*>(this);
		}


		template<typename ... Arguments>
		ActualClass& add(QLayoutItem* pItem, Arguments ... pArguments)
		{
			mDelegate.addItem(pItem, pArguments ...);
			return *static_cast<ActualClass*>(this);
		}


		ParentBuilder& end()
		{
			return *mParent;
		}


	protected:
		Delegate mDelegate;
		ParentBuilder* mParent;
};


class BoxDelegate
{
	public:
		BoxDelegate(QBoxLayout::Direction pDirection, int pMargin = -1, int pSpacing = -1)
			: mLayout(new QBoxLayout(pDirection))
		{
			if (pMargin >= 0)
			{
				mLayout->setMargin(pMargin);
			}

			if (pSpacing >= 0)
			{
				mLayout->setSpacing(pSpacing);
			}
		}


		template<typename ... Arguments>
		BoxDelegate(QWidget* pWidget, QBoxLayout::Direction pDirection, Arguments ... pArguments)
			: BoxDelegate(pDirection, pArguments ...)
		{
			pWidget->setLayout(mLayout);
		}


		template<typename ... Arguments>
		BoxDelegate(QBoxLayout::Direction pDirection, QWidget* pWidget, Arguments ... pArguments)
			: BoxDelegate(pWidget, pDirection, pArguments ...)
		{
		}


		void addWidget(QWidget* pWidget)
		{
			mLayout->addWidget(pWidget);
		}


		void addItem(QLayoutItem* pItem)
		{
			mLayout->addItem(pItem);
		}


	private:
		QBoxLayout* mLayout;
		Q_DISABLE_COPY(BoxDelegate)
};

template<typename ParentBuilder = Dummy>
class Box
	: public Base<BoxDelegate
	, ParentBuilder
	, Box<ParentBuilder> >
{
	public:
		template<typename ... Arguments>
		Box(Arguments ... pArguments)
			: Base<BoxDelegate
			, ParentBuilder
			, Box<ParentBuilder> >(pArguments ...)
		{
		}


};


template<typename ParentBuilder = Dummy>
class HBox
	: public Box<ParentBuilder>
{
	public:
		template<typename ... Arguments>
		HBox(Arguments ... pArguments)
			: Box<ParentBuilder>(QBoxLayout::LeftToRight, pArguments ...)
		{
		}


};


template<typename ParentBuilder = Dummy>
class VBox
	: public Box<ParentBuilder>
{
	public:
		template<typename ... Arguments>
		VBox(Arguments ... pArguments)
			: Box<ParentBuilder>(QBoxLayout::TopToBottom, pArguments ...)
		{
		}


};


class GridDelegate
{
	public:
		GridDelegate(QGridLayout* pLayout)
			: mLayout(pLayout)
		{
		}


		GridDelegate(int pMargin = -1, int pSpacing = -1)
			: mLayout(new QGridLayout())
		{
			if (pMargin >= 0)
			{
				mLayout->setMargin(pMargin);
			}

			if (pSpacing >= 0)
			{
				mLayout->setSpacing(pSpacing);
			}
		}


		template<typename ... Arguments>
		GridDelegate(QWidget* pWidget, Arguments ... pArguments)
			: GridDelegate(pArguments ...)
		{
			pWidget->setLayout(mLayout);
		}


		void addWidget(QWidget* pWidget, int pRow, int pColumn, int pRowSpan = 1, int pColumnSpan = 1)
		{
			mLayout->addWidget(pWidget, pRow, pColumn, pRowSpan, pColumnSpan);
		}


		void addItem(QLayoutItem* pItem, int pRow, int pColumn, int pRowSpan = 1, int pColumnSpan = 1)
		{
			mLayout->addItem(pItem, pRow, pColumn, pRowSpan, pColumnSpan);
		}


	private:
		QGridLayout* mLayout;
		Q_DISABLE_COPY(GridDelegate)
};

template<typename ParentBuilder = Dummy>
class Grid
	: public Base<GridDelegate
	, ParentBuilder
	, Grid<ParentBuilder> >
{
	public:
		template<typename ... Arguments>
		Grid(Arguments ... pArguments)
			: Base<GridDelegate
			, ParentBuilder
			, Grid<ParentBuilder> >(pArguments ...)
		{
		}


};

} /* namespace LayoutBuilder */

} /* namespace governikus */
