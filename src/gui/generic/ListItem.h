#pragma once

#include <QWidget>


namespace governikus
{

class ListItem
	: public QWidget
{
	Q_OBJECT

	public:
		ListItem(QWidget* pParent = nullptr, Qt::WindowFlags pWindowFlags = 0);
		virtual ~ListItem();

		void paintEvent(QPaintEvent*) override;
};

}
