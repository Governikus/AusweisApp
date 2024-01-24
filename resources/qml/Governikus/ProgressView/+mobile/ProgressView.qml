/**
 * Copyright (c) 2016-2023 Governikus GmbH & Co. KG, Germany
 */
import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Governikus.Global
import Governikus.Style
import Governikus.View

FlickableSectionPage {
	id: root

	property alias icon: statusIcon.source
	property alias progressBarVisible: progressBar.visible
	property alias progressText: progressText.text
	property int progressValue
	property alias subText: subText.text
	property alias subTextColor: subText.color
	property alias text: text.text

	AnimatedImage {
		id: statusIcon

		Layout.alignment: Qt.AlignHCenter
		Layout.preferredHeight: Style.dimens.header_icon_size
		fillMode: Image.PreserveAspectFit
		source: "qrc:///images/sandglass.webp"
	}
	GText {
		id: text

		Layout.alignment: Qt.AlignHCenter
		Layout.topMargin: Constants.component_spacing
		horizontalAlignment: Text.AlignHCenter
		textStyle: Style.text.headline
	}
	GText {
		id: subText

		Layout.alignment: Qt.AlignHCenter
		Layout.topMargin: Constants.text_spacing
		horizontalAlignment: Text.AlignHCenter
		textStyle: Style.text.subline
	}
	GSpacer {
		Layout.fillHeight: true
	}
	GProgressBar {
		id: progressBar

		Layout.alignment: Qt.AlignHCenter
		Layout.fillWidth: true
		Layout.topMargin: 2 * Constants.component_spacing
		value: progressValue
		visible: false
	}
	GText {
		id: progressText

		Layout.alignment: Qt.AlignHCenter
		Layout.topMargin: Constants.text_spacing
		horizontalAlignment: Text.AlignHCenter
		visible: progressBar.visible
	}
}
