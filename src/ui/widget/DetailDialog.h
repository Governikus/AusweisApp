/*!
 * \brief Detail dialog for certificate description
 *
 * \copyright Copyright (c) 2014-2018 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include <QDialog>

namespace Ui
{
class DetailDialog;
} // namespace Ui

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
		virtual ~DetailDialog() override;

		void setDetails(const QString& pDetails);

	protected:
		virtual bool eventFilter(QObject* pObject, QEvent* pEvent) override;
		virtual void changeEvent(QEvent* pEvent) override;

};

} // namespace governikus
