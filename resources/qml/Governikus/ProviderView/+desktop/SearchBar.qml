import QtQuick 2.10
import QtQuick.Controls 2.3

import Governikus.Global 1.0


Row {
	id: root

	property int availableWidth: 0
	readonly property int contentWidth: root.implicitWidth
	readonly property alias searchText: searchField.displayText

	spacing: Constants.titlebar_padding
	height: searchField.height

	GTextField {
		id: searchField
		width: root.availableWidth - parent.spacing - iconItem.width - 2 * Constants.titlebar_padding
		font.pixelSize: Constants.small_font_size

		visible: false

		onAccepted: {
			iconItem.forceActiveFocus(Qt.MouseFocusReason)
		}

		Behavior on visible {
			PropertyAnimation {
				duration: 150
			}
		}
	}

	Image {
		id: iconItem

		sourceSize.height: parent.height
		source: "qrc:///images/search.svg"

		MouseArea {
			anchors.fill: parent
			onClicked: {
				// Storage of the new value is needed because the query
				// of searchField.visible still delivers the old value.
				var searchFieldVisible = !searchField.visible

				if (searchFieldVisible) {
					iconItem.source = "qrc:///images/cancel.svg"
					searchField.forceActiveFocus(Qt.MouseFocusReason)
					Qt.inputMethod.show()
				} else {
					iconItem.source = "qrc:///images/search.svg"
					iconItem.forceActiveFocus(Qt.MouseFocusReason)
					searchField.text = ""
					Qt.inputMethod.hide()
				}

				searchField.visible = searchFieldVisible
			}
		}
	}
}
