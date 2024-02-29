/**
 * Copyright (c) 2021-2024 Governikus GmbH & Co. KG, Germany
 */
import QtQuick
import QtQuick.Layouts
import Governikus.Global
import Governikus.Style
import Governikus.TitleBar
import Governikus.View
import Governikus.Type.SmartModel
import Governikus.Type.PersonalizationModel

ColumnLayout {
	id: contentLayout

	readonly property int smartState: SmartModel.smartState

	signal changePin
	signal deletePersonalization
	signal startSelfAuth
	signal updateSmart

	GOptionsContainer {
		Layout.fillWidth: true
		spacing: Constants.component_spacing

		//: LABEL ANDROID IOS
		title: qsTr("Smart-eID")

		GMenuItem {
			//: LABEL ANDROID IOS
			description: qsTr("Show Smart-eID data")

			//: LABEL ANDROID IOS
			title: qsTr("Try Smart-eID")
			visible: smartState === SmartModel.SMART_READY
			width: parent.width

			onClicked: startSelfAuth()
		}
		GSeparator {
			anchors.left: parent.left
			anchors.leftMargin: Constants.component_spacing
			anchors.right: parent.right
			anchors.rightMargin: Constants.component_spacing
		}
		GMenuItem {
			//: LABEL ANDROID IOS
			description: qsTr("Change the chosen Smart-eID PIN")

			//: LABEL ANDROID IOS
			title: qsTr("Change Smart-eID PIN")
			visible: smartState === SmartModel.SMART_READY
			width: parent.width

			onClicked: changePin()
		}
		GSeparator {
			anchors.left: parent.left
			anchors.leftMargin: Constants.component_spacing
			anchors.right: parent.right
			anchors.rightMargin: Constants.component_spacing
		}
		GMenuItem {
			//: LABEL ANDROID IOS
			description: qsTr("Renew your Smart-eID with current data")

			//: LABEL ANDROID IOS
			title: qsTr("Renew Smart-eID")
			visible: smartState === SmartModel.SMART_READY
			width: parent.width

			onClicked: updateSmart()
		}
		GSeparator {
			anchors.left: parent.left
			anchors.leftMargin: Constants.component_spacing
			anchors.right: parent.right
			anchors.rightMargin: Constants.component_spacing
		}
		GMenuItem {
			//: LABEL ANDROID IOS
			description: qsTr("Delete Smart-eID data from your device")

			//: LABEL ANDROID IOS
			title: qsTr("Delete Smart-eID")
			visible: smartState === SmartModel.SMART_READY
			width: parent.width

			onClicked: deletePersonalization()
		}
	}
}
