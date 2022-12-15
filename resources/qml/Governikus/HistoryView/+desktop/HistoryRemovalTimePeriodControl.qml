/*
 * \copyright Copyright (c) 2019-2022 Governikus GmbH & Co. KG, Germany
*/
import QtQuick 2.15
import QtQuick.Layouts 1.15
import Governikus.Global 1.0
import Governikus.Style 1.0
import Governikus.View 1.0
import Governikus.Type.ApplicationModel 1.0

RowLayout {
	id: root

	property alias description: description
	property string period
	property alias removalPeriod: removalPeriod

	Accessible.name: description.text
	Accessible.role: Accessible.Grouping
	spacing: Constants.groupbox_spacing

	GText {
		id: description
		activeFocusOnTab: true

		//: LABEL DESKTOP
		text: qsTr("Time period")
		textStyle: Style.text.normal

		FocusFrame {
		}
	}
	GComboBox {
		id: removalPeriod
		Layout.fillWidth: true
		activeFocusOnTab: true
		height: ApplicationModel.scaleFactor * 50

		model: ListModel {
			id: timePeriods

			//: LABEL DESKTOP
			ListElement {
				modelData: qsTr("Past hour")
				value: "PAST_HOUR"
			}
			//: LABEL DESKTOP
			ListElement {
				modelData: qsTr("Past day")
				value: "PAST_DAY"
			}
			//: LABEL DESKTOP
			ListElement {
				modelData: qsTr("Past week")
				value: "PAST_WEEK"
			}
			//: LABEL DESKTOP
			ListElement {
				modelData: qsTr("Last four weeks")
				value: "LAST_FOUR_WEEKS"
			}
			//: LABEL DESKTOP
			ListElement {
				modelData: qsTr("All history")
				value: "ALL_HISTORY"
			}
		}

		onCurrentIndexChanged: root.period = timePeriods.get(currentIndex).value
	}
}
