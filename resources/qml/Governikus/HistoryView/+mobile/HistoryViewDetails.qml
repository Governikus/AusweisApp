/*
 * \copyright Copyright (c) 2016-2021 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12

import Governikus.Global 1.0
import Governikus.TitleBar 1.0
import Governikus.View 1.0

SectionPage {
	id: root
	property var historyModelItem

	navigationAction: NavigationAction { state: "back"; onClicked: firePop() }
	title: historyModelItem ? historyModelItem.subject : ""
	titleBarColor: Category.displayColor(historyModelItem ? historyModelItem.providerCategory : "")

	content: Item {
		height: pane.height + 2 * Constants.component_spacing
		width: root.width

		Column {
			anchors.fill: parent
			anchors.margins: Constants.component_spacing

			GPane {
				id: pane
				//: LABEL ANDROID IOS
				title: qsTr("Provider Information")

				anchors {
					left: parent.left
					right: parent.right
				}

				LabeledText {
					Accessible.onScrollDownAction: scrollPageDown()
					Accessible.onScrollUpAction: scrollPageUp()

					//: LABEL ANDROID IOS
					label: qsTr("Provider name")
					text: historyModelItem ? historyModelItem.subject : ""
					width: parent.width
				}

				LabeledText {
					visible: text !== ""

					Accessible.onScrollDownAction: scrollPageDown()
					Accessible.onScrollUpAction: scrollPageUp()

					//: LABEL ANDROID IOS
					label: qsTr("Purpose")
					text: historyModelItem ? historyModelItem.purpose : ""
					width: parent.width
				}

				LabeledText {
					Accessible.onScrollDownAction: scrollPageDown()
					Accessible.onScrollUpAction: scrollPageUp()

					//: LABEL ANDROID IOS
					label: qsTr("Date")
					text:{
						if (!historyModelItem) {
							return "";
						}
						return historyModelItem.dateTime.toLocaleString(Qt.locale(), qsTr("dd.MM.yyyy"))
					}
					width: parent.width
				}

				LabeledText {
					Accessible.onScrollDownAction: scrollPageDown()
					Accessible.onScrollUpAction: scrollPageUp()

					visible: text !== ""

					//: LABEL ANDROID IOS
					label: qsTr("Write access (update)")
					text: historyModelItem ? historyModelItem.writtenData : ""
					width: parent.width
				}

				LabeledText {
					Accessible.onScrollDownAction: scrollPageDown()
					Accessible.onScrollUpAction: scrollPageUp()

					//: LABEL ANDROID IOS
					label: qsTr("Read access")
					text: historyModelItem ? historyModelItem.requestedData : ""
					width: parent.width
				}

				LabeledText {
					Accessible.onScrollDownAction: scrollPageDown()
					Accessible.onScrollUpAction: scrollPageUp()

					//: LABEL ANDROID IOS
					label: qsTr("Terms of usage")
					text: historyModelItem ? historyModelItem.termsOfUsage : ""
					width: parent.width
				}
			}
		}
	}
}
