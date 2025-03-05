/**
 * Copyright (c) 2016-2025 Governikus GmbH & Co. KG, Germany
 */
import QtQuick
import QtQuick.Layouts
import Governikus.Animations
import Governikus.Global
import Governikus.Style
import Governikus.View

FlickableSectionPage {
	id: root

	property alias headline: headline.text
	property alias icon: statusIcon.source
	property alias progressBarVisible: progressBar.visible
	property alias progressText: progressText.text
	property alias progressValue: progressBar.value
	property alias text: infoText.text

	HourglassAnimation {
		Layout.alignment: Qt.AlignHCenter
		visible: !statusIcon.visible
	}
	TintableIcon {
		id: statusIcon

		Layout.alignment: Qt.AlignHCenter
		sourceSize.height: Style.dimens.header_icon_size
		tintColor: Style.color.image
		visible: source.toString() !== ""
	}
	GText {
		id: headline

		Layout.alignment: Qt.AlignHCenter
		Layout.topMargin: Style.dimens.pane_spacing
		horizontalAlignment: Text.AlignHCenter
		textStyle: Style.text.headline
		visible: text !== ""
	}
	GText {
		id: infoText

		Layout.alignment: Qt.AlignHCenter
		Layout.topMargin: Style.dimens.text_spacing
		horizontalAlignment: Text.AlignHCenter
		visible: text !== ""
	}
	GSpacer {
		Layout.fillHeight: true
	}
	GProgressBar {
		id: progressBar

		Layout.alignment: Qt.AlignHCenter
		Layout.fillWidth: true
		Layout.topMargin: 2 * Style.dimens.pane_spacing
		visible: false
	}
	GText {
		id: progressText

		Layout.alignment: Qt.AlignHCenter
		Layout.topMargin: Style.dimens.text_spacing
		horizontalAlignment: Text.AlignHCenter
		visible: progressBar.visible
	}
}
