/*
 * \copyright Copyright (c) 2016-2019 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.10
import QtQuick.Controls 2.3

import Governikus.Global 1.0
import Governikus.Style 1.0
import Governikus.Type.SettingsModel 1.0
import Governikus.Type.ApplicationModel 1.0
import Governikus.View 1.0


Rectangle {
	id: baseItem

	height: button.height + 2 * Constants.pane_padding
	width: parent.width

	property string selectedCategory: ""
	property string providerIcon: ""
	property string address: ""
	property string shortName: ""
	property string shortDescription: ""
	property alias buttonColor: button.buttonColor

	function clickButton() {
		if (baseItem.address !== "") {
			Qt.openUrlExternally(baseItem.address)
		}
	}

	Accessible.name: qsTr("Link to service provider") + SettingsModel.translationTrigger
	Accessible.description: qsTr("Clicking this link will open the website of the service provider in your web browser. The URL of the provider is") + " " + address + SettingsModel.translationTrigger
	Accessible.role: Accessible.Button
	activeFocusOnTab: true

	Keys.onSpacePressed: clickButton()

	color: Constants.white

	Image {
		id: icon

		anchors.left: parent.left
		anchors.leftMargin: Constants.component_spacing
		anchors.verticalCenter: baseItem.top

		height: ApplicationModel.scaleFactor * 135
		width: height

		source: baseItem.providerIcon
		asynchronous: true
		fillMode: Image.PreserveAspectFit
	}

	GText {
		width: parent.width / 2
		anchors.left: icon.right
		anchors.right: button.left
		anchors.leftMargin: Constants.component_spacing
		anchors.rightMargin: Constants.component_spacing
		anchors.verticalCenter: parent.verticalCenter

		textStyle: Style.text.header_inverse
		text: shortDescription !== "" ? shortDescription : shortName
		elide: Text.ElideRight
		maximumLineCount: 2
	}

	GButton {
		id: button

		anchors.right: parent.right
		anchors.verticalCenter: parent.verticalCenter
		anchors.rightMargin: Constants.pane_padding

		//: LABEL DESKTOP_QML
		text: qsTr("To online application") + SettingsModel.translationTrigger
		icon.source: providerIcon
		buttonColor: categoryColor
		enabled: baseItem.address !== ""
		onClicked: baseItem.clickButton()

		ToolTip.delay: 500
		ToolTip.visible: hovered
		ToolTip.text: baseItem.address
	}

}
