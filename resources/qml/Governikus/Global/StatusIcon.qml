/*
 * \copyright Copyright (c) 2018-2022 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.15
import QtQuick.Controls 2.15

import Governikus.Global 1.0
import Governikus.Style 1.0


Rectangle {
	property alias busy: busyIndicator.visible
	property alias source: image.source
	property alias text: text.text
	property alias textStyle: text.textStyle
	property alias contentBackgroundColor: content.color
	property bool borderEnabled: true

	width: height
	radius: height / 2
	border.width: height / 40;
	border.color: borderEnabled ? Style.color.accent : Style.color.transparent
	color: Style.color.transparent

	GBusyIndicator {
		id: busyIndicator

		visible: false
		anchors.fill: parent

		running: parent.visible
		factor: 1.0
	}

	Rectangle {
		id: content

		radius: height / 2
		anchors.fill: parent
		anchors.margins: parent.height / 8;
		color: Style.color.background_pane
		border.width: Style.dimens.high_contrast_item_border
		border.color: Style.color.high_contrast_item_border

		Image {
			id: image

			readonly property string sourceSuffix: source.toString().slice(-3)

			anchors.fill: parent
			anchors.margins: parent.height / 8
			sourceSize.height: Constants.is_desktop && sourceSuffix === "svg" ? height : undefined
			fillMode: Image.PreserveAspectFit
			visible: source.toString().length > 0
		}

		GText {
			id: text
			anchors.centerIn: parent
			textStyle: Style.text.title_accent
			visible: text !== ""

			Accessible.ignored: true
		}
	}
}
