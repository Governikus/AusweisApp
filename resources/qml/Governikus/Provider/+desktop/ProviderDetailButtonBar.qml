/*
 * \copyright Copyright (c) 2016-2022 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.12
import QtQuick.Controls 2.12

import Governikus.Global 1.0
import Governikus.Style 1.0
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

	Keys.onSpacePressed: clickButton()

	color: Style.color.background_pane

	GSeparator {
		anchors {
			top: parent.top
			left: parent.left
			right: parent.right
		}

		height: Style.dimens.high_contrast_item_border
		color: Style.color.high_contrast_item_border
	}

	GSeparator {
		anchors {
			bottom: parent.bottom
			left: parent.left
			right: parent.right
		}

		height: Style.dimens.high_contrast_item_border
		color: Style.color.high_contrast_item_border
	}

	Rectangle {
		id: iconContainer

		height: ApplicationModel.scaleFactor * 135
		width: height

		anchors.left: parent.left
		anchors.leftMargin: Constants.component_spacing
		anchors.verticalCenter: baseItem.top

		radius: Style.dimens.corner_radius
		color: Constants.white
		border.color: Style.color.border
		border.width: Style.dimens.separator_size

		Image {
			id: icon

			anchors.fill: parent
			anchors.margins: iconContainer.radius / 2

			sourceSize.height: height
			source: baseItem.providerIcon
			asynchronous: true
			fillMode: Image.PreserveAspectFit
		}
	}

	GText {
		width: parent.width / 2
		anchors.left: iconContainer.right
		anchors.right: button.left
		anchors.leftMargin: Constants.component_spacing
		anchors.rightMargin: Constants.component_spacing
		anchors.verticalCenter: parent.verticalCenter

		activeFocusOnTab: true

		textStyle: Style.text.header
		text: shortDescription !== "" ? shortDescription : shortName
		elide: Text.ElideRight
		maximumLineCount: 2

		FocusFrame {
			borderColor: Style.color.focus_indicator
		}
	}

	GButton {
		id: button

		anchors.right: parent.right
		anchors.verticalCenter: parent.verticalCenter
		anchors.rightMargin: Constants.pane_padding

		//: LABEL DESKTOP
		text: qsTr("To provider")
		tintIcon: true
		icon.source: "qrc:///images/material_open_in_new.svg"
		buttonColor: categoryColor
		enabled: baseItem.address !== ""
		enabledTooltipText: baseItem.address
		onClicked: baseItem.clickButton()
	}

}
