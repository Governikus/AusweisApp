/*
 * \copyright Copyright (c) 2017-2019 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.10

import Governikus.Style 1.0
import Governikus.Type.SettingsModel 1.0

import "Utils.js" as Utils


MouseArea {
	property string language
	property string name
	property string image

	function setLanguage() {
		SettingsModel.language = language
		if (typeof(navigationController) !== "undefined") {
			navigationController.close()
		}
	}

	height: 48
	width: height

	Accessible.role: Accessible.Button
	Accessible.onPressAction: if (Qt.platform.os === "ios") clicked(null)
	onClicked: setLanguage()

	Rectangle {
		opacity: 0.1
		border.color: "black"
		border.width: SettingsModel.language === language ? 0 : 1
		color: SettingsModel.language === language ? Constants.black : Style.color.accent
		anchors.fill: parent
		radius: Style.dimens.button_radius
	}

	GText {
		id: nameText

		anchors.topMargin: 4
		anchors.top: parent.top
		anchors.horizontalCenter: parent.horizontalCenter

		Accessible.ignored: true

		text: name
		textStyle: Style.text.hint_secondary
	}

	Image {
		source: image
		fillMode: Image.PreserveAspectFit

		anchors.margins: 4
		anchors.left: parent.left
		anchors.bottom: parent.bottom
		anchors.right: parent.right
		anchors.top: nameText.bottom
	}
}
