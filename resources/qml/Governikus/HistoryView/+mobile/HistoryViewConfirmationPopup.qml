/*
 * \copyright Copyright (c) 2018-2019 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.10
import QtQuick.Controls 2.3

import Governikus.Global 1.0
import Governikus.Style 1.0
import Governikus.Type.ApplicationModel 1.0
import Governikus.Type.SettingsModel 1.0

ConfirmationPopup {
	//: LABEL ANDROID IOS
	title: qsTr("Delete history") + SettingsModel.translationTrigger
	//: LABEL ANDROID IOS Confirmaton popup to clear all history entries.
	text: qsTr("Please confirm that you want to delete your complete history.") + SettingsModel.translationTrigger
	//: LABEL ANDROID IOS
	okButtonText: qsTr("Delete") + SettingsModel.translationTrigger

	onConfirmed: SettingsModel.removeHistory("ALL_HISTORY")
}
