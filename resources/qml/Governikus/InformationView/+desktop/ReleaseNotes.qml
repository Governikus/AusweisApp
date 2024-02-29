/**
 * Copyright (c) 2020-2024 Governikus GmbH & Co. KG, Germany
 */
import QtQuick
import QtQuick.Layouts
import Governikus.Global
import Governikus.View
import Governikus.Style
import Governikus.Type.ReleaseInformationModel

ColumnLayout {
	id: root

	Component.onCompleted: releaseInformationModel.update()

	ReleaseInformationModel {
		id: releaseInformationModel

	}
	ReleaseNotesView {
		Layout.fillHeight: true
		Layout.fillWidth: true
		model: releaseInformationModel.currentRelease

		layer {
			enabled: GraphicsInfo.api !== GraphicsInfo.Software

			effect: GDropShadow {
			}
		}
	}
	GButton {
		Layout.alignment: Qt.AlignHCenter | Qt.AlignBottom
		Layout.bottomMargin: Constants.pane_padding
		icon.source: "qrc:///images/material_refresh.svg"
		//: LABEL DESKTOP
		text: qsTr("Retry")
		tintIcon: true
		visible: releaseInformationModel.allowRetry

		onClicked: releaseInformationModel.update()
	}
}
