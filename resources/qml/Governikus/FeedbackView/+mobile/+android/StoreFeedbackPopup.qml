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
	//: INFO ANDROID Header of the app rating popup.
	title: qsTr("Would you like to rate this app?") + SettingsModel.translationTrigger
	//: INFO ANDROID Content of the app rating popup.
	text: qsTr("We would be very grateful if you could leave a rating on the Google Play Store!") + SettingsModel.translationTrigger
	//: LABEL ANDROID
	cancelButtonText: qsTr("No, thanks") + SettingsModel.translationTrigger
	//: LABEL ANDROID
	okButtonText: qsTr("Rate") + SettingsModel.translationTrigger

	onConfirmed: Qt.openUrlExternally("market://details?id=" + ApplicationModel.packageName)
}
