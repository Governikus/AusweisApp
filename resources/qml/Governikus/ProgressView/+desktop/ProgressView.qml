/*
 * \copyright Copyright (c) 2016-2023 Governikus GmbH & Co. KG, Germany
 */
import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import Governikus.Global 1.0
import Governikus.Style 1.0
import Governikus.View 1.0
import Governikus.Type.ApplicationModel 1.0

SectionPage {
	id: baseItem

	property alias progressBarVisible: progressBar.visible
	property alias progressText: progressText.text
	property int progressValue
	property alias subText: subText.text
	property alias subTextColor: subText.color
	property alias text: text.text

	StatusIcon {
		id: circle
		anchors.horizontalCenter: parent.horizontalCenter
		anchors.verticalCenter: parent.top
		anchors.verticalCenterOffset: baseItem.height / 4
		borderEnabled: false
		busy: true
		height: Style.dimens.status_icon_large
		source: "qrc:///images/sandglass.svg"
	}
	GText {
		id: text
		Accessible.name: text.text
		activeFocusOnTab: true
		anchors.horizontalCenter: parent.horizontalCenter
		anchors.top: parent.verticalCenter
		horizontalAlignment: Text.AlignHCenter
		textStyle: Style.text.header
		visible: text.text !== ""
		width: Math.min(parent.width - (2 * Constants.pane_padding), Style.dimens.max_text_width)

		FocusFrame {
		}
	}
	GText {
		id: subText
		Accessible.name: subText.text
		activeFocusOnTab: true
		anchors.horizontalCenter: parent.horizontalCenter
		anchors.top: text.bottom
		anchors.topMargin: Constants.text_spacing
		horizontalAlignment: Text.AlignHCenter
		textStyle: Style.text.header_secondary
		visible: subText.text !== ""
		width: Math.min(parent.width - (2 * Constants.pane_padding), Style.dimens.max_text_width)

		FocusFrame {
		}
	}
	GText {
		id: progressText
		Accessible.name: progressText.text
		activeFocusOnTab: true
		anchors.bottom: progressBar.top
		anchors.bottomMargin: Constants.component_spacing
		anchors.horizontalCenter: parent.horizontalCenter
		horizontalAlignment: Text.AlignHCenter
		visible: progressBar.visible
		width: Math.min(parent.width - (2 * Constants.pane_padding), Style.dimens.max_text_width)

		FocusFrame {
		}
	}
	GProgressBar {
		id: progressBar
		activeFocusOnTab: true
		value: progressValue
		visible: false

		anchors {
			bottom: parent.bottom
			left: parent.left
			margins: ApplicationModel.scaleFactor * 80
			right: parent.right
		}
	}
}
