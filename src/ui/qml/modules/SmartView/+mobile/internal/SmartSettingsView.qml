/**
 * Copyright (c) 2021-2025 Governikus GmbH & Co. KG, Germany
 */

import QtQuick
import QtQuick.Layouts

import Governikus.Global
import Governikus.Type
import Governikus.Style

ColumnLayout {
	id: root

	readonly property int smartState: SmartModel.state

	signal changePin
	signal deletePersonalization
	signal startSelfAuth
	signal updateSmart

	GOptionsContainer {
		Layout.fillWidth: true
		spacing: Style.dimens.pane_spacing

		//: LABEL ANDROID IOS
		title: qsTr("Smart-eID")

		GMenuItem {
			//: LABEL ANDROID IOS
			description: qsTr("Show Smart-eID data")

			//: LABEL ANDROID IOS
			title: qsTr("Try Smart-eID")
			visible: root.smartState === SmartModel.State.READY
			width: parent.width

			onClicked: root.startSelfAuth()
		}
		GSeparator {
			anchors.left: parent.left
			anchors.leftMargin: Style.dimens.pane_spacing
			anchors.right: parent.right
			anchors.rightMargin: Style.dimens.pane_spacing
		}
		GMenuItem {
			//: LABEL ANDROID IOS
			description: qsTr("Change the chosen Smart-eID PIN")

			//: LABEL ANDROID IOS
			title: qsTr("Change Smart-eID PIN")
			visible: root.smartState === SmartModel.State.READY
			width: parent.width

			onClicked: root.changePin()
		}
		GSeparator {
			anchors.left: parent.left
			anchors.leftMargin: Style.dimens.pane_spacing
			anchors.right: parent.right
			anchors.rightMargin: Style.dimens.pane_spacing
		}
		GMenuItem {
			//: LABEL ANDROID IOS
			description: qsTr("Renew your Smart-eID with current data")

			//: LABEL ANDROID IOS
			title: qsTr("Renew Smart-eID")
			visible: root.smartState === SmartModel.State.READY
			width: parent.width

			onClicked: root.updateSmart()
		}
		GSeparator {
			anchors.left: parent.left
			anchors.leftMargin: Style.dimens.pane_spacing
			anchors.right: parent.right
			anchors.rightMargin: Style.dimens.pane_spacing
		}
		GMenuItem {
			//: LABEL ANDROID IOS
			description: qsTr("Delete Smart-eID data from your device")

			//: LABEL ANDROID IOS
			title: qsTr("Delete Smart-eID")
			visible: root.smartState === SmartModel.State.READY
			width: parent.width

			onClicked: root.deletePersonalization()
		}
	}
}
