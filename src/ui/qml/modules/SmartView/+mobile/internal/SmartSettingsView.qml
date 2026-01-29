/**
 * Copyright (c) 2021-2026 Governikus GmbH & Co. KG, Germany
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

		//: MOBILE
		title: qsTr("Smart-eID")

		GMenuItem {
			Layout.fillWidth: true
			//: MOBILE
			description: qsTr("Show Smart-eID data")

			//: MOBILE
			title: qsTr("Try Smart-eID")
			visible: root.smartState === SmartModel.State.READY

			onClicked: root.startSelfAuth()
		}
		GSeparator {
			Layout.fillWidth: true
			Layout.leftMargin: Style.dimens.pane_spacing
			Layout.rightMargin: Layout.leftMargin
		}
		GMenuItem {
			Layout.fillWidth: true
			//: MOBILE
			description: qsTr("Change the chosen Smart-eID PIN")

			//: MOBILE
			title: qsTr("Change Smart-eID PIN")
			visible: root.smartState === SmartModel.State.READY

			onClicked: root.changePin()
		}
		GSeparator {
			Layout.fillWidth: true
			Layout.leftMargin: Style.dimens.pane_spacing
			Layout.rightMargin: Layout.leftMargin
		}
		GMenuItem {
			Layout.fillWidth: true
			//: MOBILE
			description: qsTr("Renew your Smart-eID with current data")

			//: MOBILE
			title: qsTr("Renew Smart-eID")
			visible: root.smartState === SmartModel.State.READY

			onClicked: root.updateSmart()
		}
		GSeparator {
			Layout.fillWidth: true
			Layout.leftMargin: Style.dimens.pane_spacing
			Layout.rightMargin: Layout.leftMargin
		}
		GMenuItem {
			Layout.fillWidth: true
			//: MOBILE
			description: qsTr("Delete Smart-eID data from your device")

			//: MOBILE
			title: qsTr("Delete Smart-eID")
			visible: root.smartState === SmartModel.State.READY

			onClicked: root.deletePersonalization()
		}
	}
}
