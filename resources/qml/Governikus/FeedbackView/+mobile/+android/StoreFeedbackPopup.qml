/*
 * \copyright Copyright (c) 2018-2020 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.10
import QtQuick.Controls 2.3

import Governikus.Global 1.0
import Governikus.Style 1.0
import Governikus.Type.ApplicationModel 1.0
import Governikus.Type.SettingsModel 1.0

ConfirmationPopup {
	//: INFO ANDROID Header of the app rating popup.
	title: qsTr("Are you satisfied with AusweisApp2?") + SettingsModel.translationTrigger
	//: INFO ANDROID Content of the app rating popup.
	text: qsTr("We would be very grateful if you could leave a rating on the Google Play Store!") + SettingsModel.translationTrigger
	//: LABEL ANDROID
	cancelButtonText: qsTr("Don't ask again") + SettingsModel.translationTrigger
	//: LABEL ANDROID
	okButtonText: qsTr("Rate app") + SettingsModel.translationTrigger

	onConfirmed: Qt.openUrlExternally("market://details?id=" + ApplicationModel.packageName)
}
