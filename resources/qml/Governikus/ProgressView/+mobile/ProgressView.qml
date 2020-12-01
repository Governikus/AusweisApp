/*
 * \copyright Copyright (c) 2016-2020 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12

import Governikus.Global 1.0
import Governikus.Style 1.0
import Governikus.View 1.0

SectionPage {
	id: baseItem

	property alias text: text.text
	property alias subText: subText.text
	property alias subTextColor: subText.color
	property alias progressText: progressText.text
	property int progressValue
	property alias progressBarVisible: progressBar.visible

	ColumnLayout {
		anchors {
			top: parent.top
			left: parent.left
			right: parent.right
			margins: Constants.pane_padding
		}

		spacing: 0

		StatusIcon {
			id: statusIcon

			Layout.alignment: Qt.AlignHCenter
			Layout.preferredWidth: Style.dimens.header_icon_size
			Layout.preferredHeight: Style.dimens.header_icon_size

			borderEnabled: false
			busy: baseItem.visible
			source: "qrc:///images/sandglass.svg"
		}

		GText {
			id: text

			Layout.fillWidth: true
			Layout.maximumWidth: Style.dimens.max_text_width
			Layout.alignment: Qt.AlignHCenter
			Layout.topMargin: Constants.component_spacing

			horizontalAlignment: Text.AlignHCenter
			textStyle: Style.text.header_accent
		}

		GText {
			id: subText

			Layout.fillWidth: true
			Layout.maximumWidth: Style.dimens.max_text_width
			Layout.alignment: Qt.AlignHCenter
			Layout.topMargin: Constants.text_spacing

			textStyle: Style.text.normal_secondary
			horizontalAlignment: Text.AlignHCenter
		}

		GProgressBar {
			id: progressBar

			visible: false

			height: 24

			Layout.fillWidth: true
			Layout.maximumWidth: Style.dimens.max_text_width
			Layout.alignment: Qt.AlignHCenter
			Layout.topMargin: 2 * Constants.component_spacing

			value: progressValue
		}

		GText {
			id: progressText

			Layout.fillWidth: true
			Layout.maximumWidth: Style.dimens.max_text_width
			Layout.alignment: Qt.AlignHCenter
			Layout.topMargin: Constants.text_spacing

			textStyle: Style.text.normal_secondary
			horizontalAlignment: Text.AlignHCenter
		}
	}
}
