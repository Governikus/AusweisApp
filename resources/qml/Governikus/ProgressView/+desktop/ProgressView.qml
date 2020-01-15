/*
 * \copyright Copyright (c) 2016-2020 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.10
import QtQuick.Controls 2.3
import QtQuick.Layouts 1.3

import Governikus.Global 1.0
import Governikus.Style 1.0
import Governikus.View 1.0
import Governikus.Type.ApplicationModel 1.0
import Governikus.Type.SettingsModel 1.0


SectionPage
{
	id: baseItem

	property alias text: text.text
	property alias subText: subText.text
	property alias subTextColor: subText.color
	property alias progressText: progressText.text
	property int progressValue
	property alias progressBarVisible: progressBar.visible

	Accessible.name: qsTr("Progress view") + SettingsModel.translationTrigger
	Accessible.description: qsTr("This is the progress view of the AusweisApp2.") + SettingsModel.translationTrigger

	StatusIcon {
		id: circle

		height: Style.dimens.status_icon_large
		anchors.horizontalCenter: parent.horizontalCenter
		anchors.verticalCenter: parent.top
		anchors.verticalCenterOffset: baseItem.height / 4

		busy: true
		source: "qrc:///images/desktop/sandglass.svg"
	}

	GText {
		id: text

		visible: text.text !== ""
		width: Math.min(parent.width - (2 * Constants.pane_padding), Style.dimens.max_text_width)
		anchors.horizontalCenter: parent.horizontalCenter
		anchors.top: parent.verticalCenter

		activeFocusOnTab: true
		Accessible.name: text.text

		horizontalAlignment: Text.AlignHCenter
		textStyle: Style.text.header_inverse

		FocusFrame {}
	}

	GText {
		id: subText

		visible: subText.text !== ""
		width: Math.min(parent.width - (2 * Constants.pane_padding), Style.dimens.max_text_width)
		anchors.horizontalCenter: parent.horizontalCenter
		anchors.top: text.bottom
		anchors.topMargin: Constants.text_spacing

		activeFocusOnTab: true
		Accessible.name: subText.text

		horizontalAlignment: Text.AlignHCenter
		textStyle: Style.text.header_secondary_inverse

		FocusFrame {}
	}

	GText {
		id: progressText

		visible: progressText.text !== ""
		width: Math.min(parent.width - (2 * Constants.pane_padding), Style.dimens.max_text_width)
		anchors.bottom: progressBar.top
		anchors.bottomMargin: ApplicationModel.scaleFactor * 20
		anchors.horizontalCenter: parent.horizontalCenter

		activeFocusOnTab: true
		Accessible.name: progressText.text

		horizontalAlignment: Text.AlignHCenter

		FocusFrame {}
	}

	ProgressBar {
		id: progressBar

		visible: false
		height: ApplicationModel.scaleFactor * 40
		anchors.bottom: parent.bottom
		anchors.left: parent.left
		anchors.right: parent.right
		anchors.margins: ApplicationModel.scaleFactor * 80

		Accessible.role: Accessible.ProgressBar
		activeFocusOnTab: true
		Accessible.name: qsTr("%1 percent done").arg(value) + SettingsModel.translationTrigger

		from: 0
		to: 100
		value: progressValue

		background: Rectangle {
			radius: ApplicationModel.scaleFactor * 8
			color: Constants.lightgrey
		}

		contentItem: Item {
			Rectangle {
				width: progressBar.visualPosition * parent.width
				height: parent.height
				radius: ApplicationModel.scaleFactor * 8
				color: Constants.green
			}
		}

		FocusFrame {}
	}
}
