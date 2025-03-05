/**
 * Copyright (c) 2020-2025 Governikus GmbH & Co. KG, Germany
 */

pragma ComponentBehavior: Bound

import QtQuick

import Governikus.Global
import Governikus.Type

ReleaseNotesView {
	id: root

	footerPositioning: ListView.OverlayFooter
	model: releaseInformationModel.currentRelease

	footer: GButton {
		anchors.horizontalCenter: parent.horizontalCenter
		icon.source: "qrc:///images/material_refresh.svg"
		//: LABEL DESKTOP
		text: qsTr("Retry")
		tintIcon: true
		visible: releaseInformationModel.allowRetry

		onClicked: releaseInformationModel.update()
	}

	Component.onCompleted: releaseInformationModel.update()

	layer {
		enabled: GraphicsInfo.api !== GraphicsInfo.Software

		effect: GDropShadow {
		}
	}
	ReleaseInformationModel {
		id: releaseInformationModel

	}
}
