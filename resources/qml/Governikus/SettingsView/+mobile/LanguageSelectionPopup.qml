/*
 * \copyright Copyright (c) 2019-2022 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.15
import QtQuick.Layouts 1.15

import Governikus.Global 1.0
import Governikus.SettingsView 1.0
import Governikus.Style 1.0

ConfirmationPopup {
	id: baseItem

	//: LABEL ANDROID IOS
	title: qsTr("Select language")

	style: ConfirmationPopup.PopupStyle.CancelButton

	LanguageButtons {
		columns: 1
		onButtonClicked: baseItem.close()
	}
}
