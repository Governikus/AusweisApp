/*!
 * DetailDialog.h
 *
 * \brief Detail dialog for certificate description
 *
 * \copyright Copyright (c) 2014 Governikus GmbH & Co. KG
 */

#pragma once

#include <QDialog>

namespace Ui
{
class DetailDialog;
}

namespace governikus
{

class DetailDialog
	: public QDialog
{
	Q_OBJECT

	private:
		Ui::DetailDialog* mUi;

	public:
		explicit DetailDialog(QWidget* pParent = nullptr);
		~DetailDialog();

		void setDetails(const QString& pDetails);

	protected:
		virtual bool eventFilter(QObject* pObject, QEvent* pEvent) override;

};

} /* namespace governikus */
