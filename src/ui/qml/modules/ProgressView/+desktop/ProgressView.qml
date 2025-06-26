/**
 * Copyright (c) 2016-2025 Governikus GmbH & Co. KG, Germany
 */
import QtQuick
import Governikus.Animations
import Governikus.Global
import Governikus.Style
import Governikus.Type
import Governikus.View

SectionPage {
	id: root

	property alias headline: headline.text
	property alias icon: statusIcon.source
	property alias progressBarVisible: progressBar.visible
	property alias progressText: progressText.text
	property alias progressValue: progressBar.value
	property alias text: infoText.text
	property alias tintColor: statusIcon.tintColor

	HourglassAnimation {
		visible: !statusIcon.visible

		anchors {
			horizontalCenter: parent.horizontalCenter
			top: parent.top
			topMargin: Style.dimens.pane_spacing
		}
	}
	TintableIcon {
		id: statusIcon

		sourceSize.height: Style.dimens.header_icon_size
		visible: source.toString() !== ""

		anchors {
			horizontalCenter: parent.horizontalCenter
			top: parent.top
			topMargin: Style.dimens.pane_spacing
		}
	}
	GText {
		id: headline

		horizontalAlignment: Text.AlignHCenter
		textStyle: Style.text.headline
		visible: text !== ""
		width: Math.min(parent.width - (2 * Style.dimens.pane_padding), Style.dimens.max_text_width)

		anchors {
			horizontalCenter: parent.horizontalCenter
			top: parent.verticalCenter
			topMargin: Style.dimens.pane_spacing * 2
		}
	}
	GText {
		id: infoText

		horizontalAlignment: Text.AlignHCenter
		visible: text !== ""
		width: Math.min(parent.width - (2 * Style.dimens.pane_padding), Style.dimens.max_text_width)

		anchors {
			horizontalCenter: parent.horizontalCenter
			top: headline.bottom
			topMargin: Style.dimens.text_spacing
		}
	}
	GText {
		id: progressText

		horizontalAlignment: Text.AlignHCenter
		visible: progressBar.visible
		width: Math.min(parent.width - (2 * Style.dimens.pane_padding), Style.dimens.max_text_width)

		anchors {
			bottom: progressBar.top
			bottomMargin: Style.dimens.pane_spacing
			horizontalCenter: parent.horizontalCenter
		}
	}
	GProgressBar {
		id: progressBar

		activeFocusOnTab: true
		visible: false

		anchors {
			bottom: parent.bottom
			left: parent.left
			margins: UiPluginModel.scaleFactor * 48
			right: parent.right
		}
	}
}
