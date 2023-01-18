/*
 * \copyright Copyright (c) 2020-2023 Governikus GmbH & Co. KG, Germany
 */
import QtQuick 2.15
import QtQuick.Layouts 1.15
import Governikus.Global 1.0
import Governikus.View 1.0
import Governikus.Style 1.0
import Governikus.Type.ReleaseInformationModel 1.0

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
