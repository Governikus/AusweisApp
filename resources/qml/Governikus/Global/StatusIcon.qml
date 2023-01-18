/*
 * \copyright Copyright (c) 2018-2023 Governikus GmbH & Co. KG, Germany
 */
import QtQuick 2.15
import QtQuick.Controls 2.15
import Governikus.Global 1.0
import Governikus.Style 1.0

Rectangle {
	property bool borderEnabled: true
	property alias busy: busyIndicator.visible
	property alias contentBackgroundColor: content.color
	property alias source: image.source
	property alias text: text.text
	property alias textStyle: text.textStyle

	border.color: borderEnabled ? Style.color.accent : Style.color.transparent
	border.width: height / 40
	color: Style.color.transparent
	radius: height / 2
	width: height

	GBusyIndicator {
		id: busyIndicator
		anchors.fill: parent
		factor: 1.0
		running: parent.visible
		visible: false
	}
	Rectangle {
		id: content
		anchors.fill: parent
		anchors.margins: parent.height / 8
		border.color: Style.color.high_contrast_item_border
		border.width: Style.dimens.high_contrast_item_border
		color: Style.color.background_pane
		radius: height / 2

		Image {
			id: image

			readonly property string sourceSuffix: source.toString().slice(-3)

			anchors.fill: parent
			anchors.margins: parent.height / 8
			fillMode: Image.PreserveAspectFit
			sourceSize.height: Constants.is_desktop && sourceSuffix === "svg" ? height : undefined
			visible: source.toString().length > 0
		}
		GText {
			id: text
			Accessible.ignored: true
			anchors.centerIn: parent
			textStyle: Style.text.title_accent
			visible: text !== ""
		}
	}
}
