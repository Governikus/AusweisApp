/*
 * \copyright Copyright (c) 2019 Governikus GmbH & Co. KG, Germany
*/

import QtQuick 2.10
import QtQuick.Layouts 1.3

import Governikus.Global 1.0
import Governikus.Style 1.0
import Governikus.View 1.0
import Governikus.Type.SettingsModel 1.0
import Governikus.Type.ApplicationModel 1.0


RowLayout {
	id: root

	property string period
	property alias description: description
	property alias removalPeriod: removalPeriod

	Accessible.role: Accessible.Heading
	Accessible.name: description.text

	spacing: Constants.groupbox_spacing

	GText {
		id: description

		activeFocusOnTab: true

		//: LABEL DESKTOP_QML
		text: qsTr("Time period") + SettingsModel.translationTrigger

		FocusFrame {
			dynamic: false
		}
	}

	GComboBox {
		id: removalPeriod

		Layout.fillWidth: true
		height: ApplicationModel.scaleFactor * 50

		activeFocusOnTab: true
		model: ListModel {
			id: timePeriods

			//: LABEL DESKTOP_QML
			ListElement { modelData: qsTr("Past hour"); value: "PAST_HOUR" }
			//: LABEL DESKTOP_QML
			ListElement { modelData: qsTr("Past day"); value: "PAST_DAY" }
			//: LABEL DESKTOP_QML
			ListElement { modelData: qsTr("Past week"); value: "PAST_WEEK" }
			//: LABEL DESKTOP_QML
			ListElement { modelData: qsTr("Last four weeks"); value: "LAST_FOUR_WEEKS" }
			//: LABEL DESKTOP_QML
			ListElement { modelData: qsTr("All history"); value: "ALL_HISTORY" }
		}

		onCurrentIndexChanged: root.period = timePeriods.get(currentIndex).value
	}
}
