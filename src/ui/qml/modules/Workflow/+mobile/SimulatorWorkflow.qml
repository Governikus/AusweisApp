/**
 * Copyright (c) 2021-2026 Governikus GmbH & Co. KG, Germany
 */

import QtQuick
import QtQuick.Layouts
import Governikus.Global
import Governikus.Style
import Governikus.Type

GFlickableColumnLayout {
	id: root

	property var workflowModel

	clip: true
	maximumContentWidth: Style.dimens.max_text_width
	spacing: 0
	topMargin: 0

	TechnologyInfo {
		id: technologyInfo

		Layout.alignment: Qt.AlignHCenter
		//: MOBILE
		enableButtonText: qsTr("Continue")
		showAdditionalContent: true

		//: MOBILE
		titleText: qsTr("Simulator")

		progressIndicator: Item {
			id: progressIndicator

			implicitHeight: Style.dimens.workflow_progress_indicator_size
			implicitWidth: icon.implicitWidth

			TintableIcon {
				id: icon

				anchors.centerIn: parent
				desaturate: true
				source: "qrc:///images/mobile/phone_simulator.svg"
				sourceSize.height: Style.dimens.header_icon_size
				tintEnabled: false
			}
		}

		onEnableClicked: root.workflowModel.insertSimulator()

		GSpacer {
			Layout.fillHeight: true
		}
		GText {
			Layout.alignment: Qt.AlignHCenter
			//: MOBILE
			text: qsTr("Switch to:")
		}
		GButton {
			Layout.alignment: Qt.AlignHCenter
			style: Style.color.controlOptional
			//: MOBILE
			text: qsTr("NFC")

			onClicked: root.workflowModel.readerPluginType = ReaderManagerPluginType.NFC
		}
		GButton {
			Layout.alignment: Qt.AlignHCenter
			style: Style.color.controlOptional
			//: MOBILE
			text: qsTr("WiFi")

			onClicked: root.workflowModel.readerPluginType = ReaderManagerPluginType.REMOTE_IFD
		}
		GButton {
			Layout.alignment: Qt.AlignHCenter
			style: Style.color.controlOptional
			//: MOBILE
			text: qsTr("SMART")
			visible: ApplicationModel.smartSupported

			onClicked: root.workflowModel.readerPluginType = ReaderManagerPluginType.SMART
		}
		GSpacer {
			Layout.fillHeight: true
		}
	}
}
