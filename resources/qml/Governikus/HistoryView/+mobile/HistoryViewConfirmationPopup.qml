/**
 * Copyright (c) 2018-2023 Governikus GmbH & Co. KG, Germany
 */
import QtQuick 2.15
import QtQuick.Controls 2.15
import Governikus.Global 1.0
import Governikus.Style 1.0
import Governikus.Type.ApplicationModel 1.0
import Governikus.Type.SettingsModel 1.0

ConfirmationPopup {
	//: LABEL ANDROID IOS
	okButtonText: qsTr("Delete")
	//: LABEL ANDROID IOS Confirmaton popup to clear all history entries.
	text: qsTr("All history entries will be deleted.")
	//: LABEL ANDROID IOS
	title: qsTr("Delete history")

	onConfirmed: SettingsModel.removeHistory("ALL_HISTORY")
}
