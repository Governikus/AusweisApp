/*
 * \copyright Copyright (c) 2020-2023 Governikus GmbH & Co. KG, Germany
 */
import QtQuick 2.15
import QtQuick.Layouts 1.15
import Governikus.Global 1.0
import Governikus.Style 1.0
import Governikus.TitleBar 1.0
import Governikus.View 1.0
import Governikus.Type.ReleaseInformationModel 1.0

SectionPage {
	id: root
	sectionPageFlickable: releaseNotes
	//: LABEL ANDROID IOS
	title: qsTr("Release notes")

	navigationAction: NavigationAction {
		action: NavigationAction.Action.Back

		onClicked: pop()
	}

	Component.onCompleted: releaseInformationModel.update()

	ReleaseInformationModel {
		id: releaseInformationModel
	}
	GPane {
		id: pane
		clip: true

		anchors {
			fill: parent
			margins: Constants.pane_padding
		}
		ColumnLayout {
			height: pane.height - pane.anchors.topMargin - pane.anchors.bottomMargin
			width: pane.width - pane.anchors.leftMargin

			ReleaseNotesView {
				id: releaseNotes
				Layout.fillHeight: true
				Layout.fillWidth: true
				model: releaseInformationModel.currentRelease
			}
			GButton {
				Layout.alignment: Qt.AlignHCenter
				Layout.rightMargin: pane.anchors.rightMargin
				icon.source: "qrc:///images/material_refresh.svg"

				//: LABEL ANDROID IOS
				text: qsTr("Retry")
				tintIcon: true
				visible: releaseInformationModel.allowRetry

				onClicked: releaseInformationModel.update()
			}
		}
	}
}
