/*
 * \copyright Copyright (c) 2016-2022 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12

import Governikus.Global 1.0
import Governikus.Style 1.0
import Governikus.View 1.0
import Governikus.Type.ApplicationModel 1.0

SectionPage
{
	id: baseItem

	property alias text: text.text
	property alias subText: subText.text
	property alias subTextColor: subText.color
	property alias progressText: progressText.text
	property int progressValue
	property alias progressBarVisible: progressBar.visible

	Accessible.name: qsTr("Progress view")
	Accessible.description: qsTr("This is the progress view of the AusweisApp2.")

	StatusIcon {
		id: circle

		height: Style.dimens.status_icon_large
		anchors.horizontalCenter: parent.horizontalCenter
		anchors.verticalCenter: parent.top
		anchors.verticalCenterOffset: baseItem.height / 4

		busy: true
		source: "qrc:///images/sandglass.svg"
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
		anchors.bottomMargin: Constants.component_spacing
		anchors.horizontalCenter: parent.horizontalCenter

		activeFocusOnTab: true
		Accessible.name: progressText.text

		horizontalAlignment: Text.AlignHCenter

		FocusFrame {}
	}

	GProgressBar {
		id: progressBar

		visible: false
		height: ApplicationModel.scaleFactor * 40
		anchors {
			bottom: parent.bottom
			left: parent.left
			right: parent.right
			margins: ApplicationModel.scaleFactor * 80
		}

		activeFocusOnTab: true

		value: progressValue
	}
}
