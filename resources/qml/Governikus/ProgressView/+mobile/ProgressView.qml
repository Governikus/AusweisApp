/*
 * \copyright Copyright (c) 2016-2022 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

import Governikus.Global 1.0
import Governikus.Style 1.0
import Governikus.View 1.0

SectionPage {
	id: root

	property alias text: text.text
	property alias subText: subText.text
	property alias subTextColor: subText.color
	property alias progressText: progressText.text
	property int progressValue
	property alias progressBarVisible: progressBar.visible

	sectionPageFlickable: contentItem
	GFlickableColumnLayout {
		id: contentItem

		readonly property var minIconHeight: Style.dimens.medium_icon_size
		readonly property var maxIconHeight: Style.dimens.header_icon_size

		anchors.fill: parent

		minimumContentHeight: implicitContentHeight - (maxIconHeight - minIconHeight)
		maximumContentWidth: Style.dimens.max_text_width
		spacing: 0

		StatusIcon {
			id: statusIcon

			implicitWidth : height
			Layout.alignment: Qt.AlignHCenter
			Layout.fillHeight: true
			Layout.minimumHeight: contentItem.minIconHeight
			Layout.maximumHeight: contentItem.maxIconHeight
			Layout.preferredHeight: contentItem.maxIconHeight

			borderEnabled: false
			busy: root.visible
			source: "qrc:///images/sandglass.svg"
		}

		GText {
			id: text

			Layout.fillWidth: true
			Layout.alignment: Qt.AlignHCenter
			Layout.topMargin: Constants.component_spacing

			horizontalAlignment: Text.AlignHCenter
			textStyle: Style.text.header_accent
		}

		GText {
			id: subText

			Layout.fillWidth: true
			Layout.alignment: Qt.AlignHCenter
			Layout.topMargin: Constants.text_spacing

			textStyle: Style.text.normal_secondary
			horizontalAlignment: Text.AlignHCenter
		}

		GSpacer { Layout.fillHeight: true }

		GProgressBar {
			id: progressBar

			visible: false

			Layout.fillWidth: true
			Layout.alignment: Qt.AlignHCenter
			Layout.topMargin: 2 * Constants.component_spacing

			value: progressValue
		}

		GText {
			id: progressText

			visible: progressBar.visible

			Layout.fillWidth: true
			Layout.alignment: Qt.AlignHCenter
			Layout.topMargin: Constants.text_spacing

			textStyle: Style.text.normal_secondary
			horizontalAlignment: Text.AlignHCenter
		}
	}
}
