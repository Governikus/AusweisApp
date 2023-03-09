/**
 * Copyright (c) 2016-2023 Governikus GmbH & Co. KG, Germany
 */
import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import Governikus.Global 1.0
import Governikus.Style 1.0
import Governikus.View 1.0

SectionPage {
	id: root

	property alias progressBarVisible: progressBar.visible
	property alias progressText: progressText.text
	property int progressValue
	property alias subText: subText.text
	property alias subTextColor: subText.color
	property alias text: text.text

	sectionPageFlickable: contentItem

	GFlickableColumnLayout {
		id: contentItem

		readonly property var maxIconHeight: Style.dimens.header_icon_size
		readonly property var minIconHeight: Style.dimens.medium_icon_size

		anchors.fill: parent
		maximumContentWidth: Style.dimens.max_text_width
		minimumContentHeight: implicitContentHeight - (maxIconHeight - minIconHeight)
		spacing: 0

		StatusIcon {
			id: statusIcon
			Layout.alignment: Qt.AlignHCenter
			Layout.fillHeight: true
			Layout.maximumHeight: contentItem.maxIconHeight
			Layout.minimumHeight: contentItem.minIconHeight
			Layout.preferredHeight: contentItem.maxIconHeight
			borderEnabled: false
			busy: root.visible
			implicitWidth: height
			source: "qrc:///images/sandglass.svg"
		}
		GText {
			id: text
			Layout.alignment: Qt.AlignHCenter
			Layout.fillWidth: true
			Layout.topMargin: Constants.component_spacing
			horizontalAlignment: Text.AlignHCenter
			textStyle: Style.text.header_accent
		}
		GText {
			id: subText
			Layout.alignment: Qt.AlignHCenter
			Layout.fillWidth: true
			Layout.topMargin: Constants.text_spacing
			horizontalAlignment: Text.AlignHCenter
			textStyle: Style.text.normal_secondary
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
			Layout.fillWidth: true
			Layout.topMargin: Constants.text_spacing
			horizontalAlignment: Text.AlignHCenter
			textStyle: Style.text.normal_secondary
			visible: progressBar.visible
		}
	}
}
