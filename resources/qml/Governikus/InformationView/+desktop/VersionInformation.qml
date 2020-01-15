/*
 * \copyright Copyright (c) 2016-2020 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.10
import QtQuick.Controls 2.3
import QtQuick.Layouts 1.3

import Governikus.Global 1.0
import Governikus.View 1.0
import Governikus.Type.ApplicationModel 1.0
import Governikus.Type.SettingsModel 1.0

Item {
	id: baseItem

	implicitWidth: column.implicitWidth
	implicitHeight: column.implicitHeight

	readonly property string helpTopic: "helpVersioninformation"

	ColumnLayout {
		id: column

		anchors.fill: parent

		spacing: Constants.component_spacing

		Repeater {
			id: repeater

			model: versionInformationModel
			delegate: LabeledText {
				id: delegate

				width: baseItem.width

				Accessible.name: model.label + ": " + model.text
				activeFocusOnTab: true

				label: model.label
				text: model.text
			}
		}
	}

	MouseArea {
		property int counter: 0

		anchors.fill: parent

		onClicked: {
			counter += 1
			if (counter === 10) {
				SettingsModel.developerOptions = !SettingsModel.developerOptions
				ApplicationModel.showFeedback(
					SettingsModel.developerOptions ?
					//: LABEL DESKTOP_QML
					qsTr("Developer options activated.") :
					//: LABEL DESKTOP_QML
					qsTr("Developer options deactivated.")
				)
				counter = 0
			}
		}
	}
}
