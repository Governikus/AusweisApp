/*
 * \copyright Copyright (c) 2021 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.12
import QtQuick.Layouts 1.12

import Governikus.Global 1.0
import Governikus.View 1.0
import Governikus.Style 1.0
import Governikus.Type.ReleaseInformationModel 1.0

ColumnLayout {
	id: root

	height: tabbedPane.availableHeight
	width: parent.width

	// Padding used by the tabbed pane
	readonly property int contentPadding: 0

	Component.onCompleted: ReleaseInformationModel.update()

	ReleaseNotesView {
		Layout.fillHeight: true
		Layout.fillWidth: true
		Layout.margins: Constants.pane_padding
		Layout.rightMargin: 0

		model: ReleaseInformationModel.currentRelease
	}

	GButton {
		visible: ReleaseInformationModel.allowRetry

		Layout.alignment: Qt.AlignHCenter | Qt.AlignBottom
		Layout.bottomMargin: Constants.pane_padding

		icon.source: "qrc:///images/material_refresh.svg"
		tintIcon: true
		//: LABEL DESKTOP_QML
		text: qsTr("Retry")

		onClicked: ReleaseInformationModel.update()
	}
}
