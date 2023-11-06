/**
 * Copyright (c) 2016-2023 Governikus GmbH & Co. KG, Germany
 */
import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Governikus.Global
import Governikus.Style
import Governikus.View

SectionPage {
	id: baseItem

	property alias progressBarVisible: progressBar.visible
	property alias progressText: progressText.text
	property int progressValue
	property alias subText: subText.text
	property alias subTextColor: subText.color
	property alias text: text.text

	TintableAnimation {
		anchors.horizontalCenter: parent.horizontalCenter
		anchors.verticalCenter: parent.top
		anchors.verticalCenterOffset: baseItem.height / 4
		fillMode: Image.PreserveAspectFit
		height: Style.dimens.header_icon_size
		source: "qrc:///images/sandglass.webp"
		tintColor: Style.color.control
	}
	GText {
		id: text

		Accessible.name: text.text
		activeFocusOnTab: true
		anchors.horizontalCenter: parent.horizontalCenter
		anchors.top: parent.verticalCenter
		horizontalAlignment: Text.AlignHCenter
		textStyle: Style.text.headline
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
		textStyle: Style.text.subline
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
			margins: plugin.scaleFactor * 80
			right: parent.right
		}
	}
}
