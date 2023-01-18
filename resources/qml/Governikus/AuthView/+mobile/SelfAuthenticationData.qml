/*
 * \copyright Copyright (c) 2016-2023 Governikus GmbH & Co. KG, Germany
 */
import QtQuick 2.15
import QtQuick.Layouts 1.15
import Governikus.Global 1.0
import Governikus.ResultView 1.0
import Governikus.Style 1.0
import Governikus.View 1.0
import Governikus.Type.SelfAuthModel 1.0

ResultView {
	id: root
	signal done

	header: qsTr("Read data")

	//: LABEL ANDROID IOS
	title: qsTr("Identify")

	onCancelClicked: done()
	onContinueClicked: done()

	GridLayout {
		id: grid
		columnSpacing: Constants.text_spacing
		columns: Constants.is_tablet ? 3 : 1
		rowSpacing: Constants.text_spacing
		width: parent.width

		Repeater {
			model: SelfAuthModel

			LabeledText {
				Layout.fillWidth: true
				label: name
				text: value === "" ? "---" : value

				Accessible.onScrollDownAction: root.scrollPageDown()
				Accessible.onScrollUpAction: root.scrollPageUp()
			}
		}
	}
}
