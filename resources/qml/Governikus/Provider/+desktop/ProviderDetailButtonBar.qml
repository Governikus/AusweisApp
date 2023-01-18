/*
 * \copyright Copyright (c) 2016-2023 Governikus GmbH & Co. KG, Germany
 */
import QtQuick 2.15
import QtQuick.Controls 2.15
import Governikus.Global 1.0
import Governikus.Style 1.0
import Governikus.Type.ApplicationModel 1.0
import Governikus.View 1.0

Rectangle {
	id: baseItem

	property string address: ""
	property alias buttonColor: button.buttonColor
	property string longName: ""
	property string providerIcon: ""
	property string selectedCategory: ""
	property string shortName: ""

	function clickButton() {
		if (baseItem.address !== "") {
			Qt.openUrlExternally(baseItem.address);
		}
	}

	color: Style.color.background_pane
	height: button.height + 2 * Constants.pane_padding
	width: parent.width

	Keys.onSpacePressed: clickButton()

	GSeparator {
		color: Style.color.high_contrast_item_border
		height: Style.dimens.high_contrast_item_border

		anchors {
			left: parent.left
			right: parent.right
			top: parent.top
		}
	}
	GSeparator {
		color: Style.color.high_contrast_item_border
		height: Style.dimens.high_contrast_item_border

		anchors {
			bottom: parent.bottom
			left: parent.left
			right: parent.right
		}
	}
	Rectangle {
		id: iconContainer
		anchors.left: parent.left
		anchors.leftMargin: Constants.component_spacing
		anchors.verticalCenter: baseItem.top
		border.color: Style.color.border
		border.width: Style.dimens.separator_size
		color: Constants.white
		height: ApplicationModel.scaleFactor * 135
		radius: Style.dimens.corner_radius
		width: height

		Image {
			id: icon
			anchors.fill: parent
			anchors.margins: iconContainer.radius / 2
			asynchronous: true
			fillMode: Image.PreserveAspectFit
			source: baseItem.providerIcon
			sourceSize.height: height
		}
	}
	GText {
		activeFocusOnTab: true
		anchors.left: iconContainer.right
		anchors.leftMargin: Constants.component_spacing
		anchors.right: button.left
		anchors.rightMargin: Constants.component_spacing
		anchors.verticalCenter: parent.verticalCenter
		elide: Text.ElideRight
		maximumLineCount: 2
		text: longName
		textStyle: Style.text.header
		width: parent.width / 2

		FocusFrame {
		}
	}
	GButton {
		id: button
		anchors.right: parent.right
		anchors.rightMargin: Constants.pane_padding
		anchors.verticalCenter: parent.verticalCenter
		buttonColor: categoryColor
		enabled: baseItem.address !== ""
		enabledTooltipText: baseItem.address
		icon.source: "qrc:///images/material_open_in_new.svg"

		//: LABEL DESKTOP
		text: qsTr("To provider")
		tintIcon: true

		onClicked: baseItem.clickButton()
	}
}
