/*
 * \copyright Copyright (c) 2016-2022 Governikus GmbH & Co. KG, Germany
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

	signal done()

	//: LABEL ANDROID IOS
	title: qsTr("Identify")
	header: qsTr("Read data")

	onContinueClicked: done()
	onCancelClicked: done()

	GridLayout {
		id: grid

		width: parent.width

		columns: Constants.is_tablet ? 3 : 1
		rowSpacing: Constants.text_spacing
		columnSpacing: Constants.text_spacing

		Repeater {
			model: SelfAuthModel

			LabeledText {
				Layout.fillWidth: true

				Accessible.onScrollDownAction: root.scrollPageDown()
				Accessible.onScrollUpAction: root.scrollPageUp()

				label: name
				text: value === "" ? "---" : value
			}
		}
	}
}
