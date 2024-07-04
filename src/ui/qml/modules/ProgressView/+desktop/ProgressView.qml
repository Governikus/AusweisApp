/**
 * Copyright (c) 2016-2024 Governikus GmbH & Co. KG, Germany
 */
import QtQuick
import Governikus.Animations
import Governikus.Global
import Governikus.Style
import Governikus.Type
import Governikus.View

SectionPage {
	id: baseItem

	property alias progressBarVisible: progressBar.visible
	property alias progressText: progressText.text
	property alias progressValue: progressBar.value
	property alias subText: subText.text
	property alias subTextColor: subText.color
	property alias text: text.text

	HourglassAnimation {
		anchors {
			horizontalCenter: parent.horizontalCenter
			top: parent.top
			topMargin: Constants.component_spacing
		}
	}
	GText {
		id: text

		Accessible.name: text.text
		activeFocusOnTab: true
		horizontalAlignment: Text.AlignHCenter
		textStyle: Style.text.headline
		visible: text.text !== ""
		width: Math.min(parent.width - (2 * Constants.pane_padding), Style.dimens.max_text_width)

		anchors {
			horizontalCenter: parent.horizontalCenter
			top: parent.verticalCenter
			topMargin: Constants.component_spacing * 2
		}
		FocusFrame {
		}
	}
	GText {
		id: subText

		Accessible.name: subText.text
		activeFocusOnTab: true
		horizontalAlignment: Text.AlignHCenter
		textStyle: Style.text.subline
		visible: subText.text !== ""
		width: Math.min(parent.width - (2 * Constants.pane_padding), Style.dimens.max_text_width)

		anchors {
			horizontalCenter: parent.horizontalCenter
			top: text.bottom
			topMargin: Constants.text_spacing
		}
		FocusFrame {
		}
	}
	GText {
		id: progressText

		Accessible.name: progressText.text
		activeFocusOnTab: true
		horizontalAlignment: Text.AlignHCenter
		visible: progressBar.visible
		width: Math.min(parent.width - (2 * Constants.pane_padding), Style.dimens.max_text_width)

		anchors {
			bottom: progressBar.top
			bottomMargin: Constants.component_spacing
			horizontalCenter: parent.horizontalCenter
		}
		FocusFrame {
		}
	}
	GProgressBar {
		id: progressBar

		activeFocusOnTab: true
		visible: false

		anchors {
			bottom: parent.bottom
			left: parent.left
			margins: UiPluginModel.scaleFactor * 80
			right: parent.right
		}
	}
}
