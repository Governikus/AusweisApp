/**
 * Copyright (c) 2021-2026 Governikus GmbH & Co. KG, Germany
 */

import QtQuick

import Governikus.Global
import Governikus.Style
import Governikus.Type

GText {
	//: ALL_PLATFORMS Text of the self authentication html link inside of a sentence
	readonly property string privacyStatementDescription: qsTr("data privacy statement")
	readonly property string privacyStatementLink: "<a href=\"%1\">%2</a>".arg(privacyStatementUrl).arg(privacyStatementDescription)
	//: ALL_PLATFORMS
	readonly property string privacyStatementText: qsTr("Your personal data is neither saved nor processed in any way. Please see our %1 for details on how your personal data is processed.")
	readonly property string privacyStatementUrl: "https://www.ausweisapp.bund.de/%1/aa2/privacy".arg(SettingsModel.language)

	font.weight: Style.font.bold
	text: privacyStatementText.arg(privacyStatementLink)
	wrapMode: Text.WordWrap
}
