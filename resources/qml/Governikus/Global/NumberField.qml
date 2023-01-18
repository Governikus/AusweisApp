/*
 * \copyright Copyright (c) 2017-2023 Governikus GmbH & Co. KG, Germany
 */
import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import Governikus.Global 1.0
import Governikus.Style 1.0
import Governikus.View 1.0
import Governikus.Type.ApplicationModel 1.0

Item {
	id: root

	readonly property bool confirmedInput: inputConfirmation.length !== number.length || inputConfirmation === number
	readonly property real eyeWidth: eye.width + eye.Layout.leftMargin + eye.Layout.rightMargin
	property string inputConfirmation
	property alias number: echoField.text
	property alias passwordLength: echoField.maximumLength

	//: LABEL DESKTOP Screenreader text for the password field
	readonly property string passwordState: qsTr("You entered %1 of %2 digits.").arg(number.length).arg(passwordLength)
	readonly property var text: if (Qt.platform.os === "windows")
		passwordState
	readonly property bool validInput: echoField.acceptableInput && confirmedInput

	signal accepted

	function append(number) {
		echoField.insert(echoField.length, number);
	}
	function handleKeyEvent(eventKey, eventModifiers = Qt.NoModifier) {
		if (eventKey >= Qt.Key_0 && eventKey <= Qt.Key_9) {
			root.append(eventKey - Qt.Key_0);
		} else if (eventKey === Qt.Key_Backspace) {
			root.removeLast();
		} else if (eventKey === Qt.Key_Delete) {
			echoField.clear();
		} else if (eventKey === Qt.Key_Paste || (eventKey === Qt.Key_V) && (eventModifiers & Qt.ControlModifier)) {
			echoField.paste();
		} else if (eventKey === Qt.Key_Enter || eventKey === Qt.Key_Return) {
			root.accepted();
		} else {
			return false;
		}

		// Otherwise focus is lost if last clicked button gets invisible
		// like 'C' in NumberPad.
		if (visible)
			root.forceActiveFocus();
		return true;
	}
	function removeLast() {
		echoField.remove(echoField.length - 1, echoField.length);
	}

	Accessible.name: (eye.activated ?
		//: LABEL DESKTOP Screenreader text for the password field
		qsTr("The password is visible.") :
		//: LABEL DESKTOP Screenreader text for the password field
		qsTr("The password is hidden.")) + (text === undefined ? " " + passwordState : "")
	Accessible.role: Accessible.EditableText
	activeFocusOnTab: true
	implicitHeight: layout.implicitHeight
	implicitWidth: layout.implicitWidth

	Keys.onPressed: event => {
		event.accepted = root.handleKeyEvent(event.key, event.modifiers);
	}
	onPasswordLengthChanged: root.number = ""

	FontMetrics {
		id: fontMetrics
		font.bold: true
		font.pixelSize: Constants.is_desktop ? ApplicationModel.scaleFactor * 65 : 24
	}
	TextInput {
		id: echoField
		maximumLength: 6
		visible: false

		validator: RegExpValidatorCompat {
			expression: new RegExp("[0-9]{" + echoField.maximumLength + "}")
		}
	}
	FocusFrame {
		framee: layout

		MouseArea {
			acceptedButtons: Qt.AllButtons
			anchors.fill: parent
			cursorShape: Qt.PointingHandCursor

			onClicked: mouse => {
				root.forceActiveFocus();
				if (mouse.button === Qt.RightButton || mouse.button === Qt.MiddleButton) {
					echoField.paste();
				}
			}
			onPressAndHold: root.handleKeyEvent(Qt.Key_Paste)
		}
	}
	RowLayout {
		id: layout
		anchors.fill: parent
		spacing: 0

		Repeater {
			model: root.passwordLength

			Text {
				readonly property real markerWidth: fontMetrics.averageCharacterWidth * 1.4

				Layout.fillWidth: true
				Layout.maximumWidth: Layout.preferredWidth
				Layout.minimumWidth: markerWidth
				Layout.preferredHeight: fontMetrics.height + Constants.text_spacing
				Layout.preferredWidth: markerWidth + (Constants.is_desktop ? Constants.component_spacing : Constants.groupbox_spacing)
				color: Style.color.primary_text
				font: fontMetrics.font
				horizontalAlignment: Text.AlignHCenter
				text: eye.activated ? root.number.substr(index, 1) : ""
				verticalAlignment: Text.AlignTop

				Rectangle {
					color: parent.color
					height: Constants.is_desktop ? Math.max(ApplicationModel.scaleFactor * 4, 1) : 1
					width: parent.markerWidth

					anchors {
						bottom: parent.bottom
						horizontalCenter: parent.horizontalCenter
					}
				}
				Rectangle {
					color: parent.color
					height: width
					radius: height / 2
					visible: !eye.activated && root.number.charAt(index) !== ""
					width: fontMetrics.averageCharacterWidth

					anchors {
						centerIn: parent
						verticalCenterOffset: -Constants.text_spacing / 2
					}
				}
			}
		}
		Button {
			id: eye

			property bool activated: false

			Layout.leftMargin: Constants.text_spacing
			Layout.preferredHeight: implicitHeight + (Constants.is_desktop ? 0 : Constants.text_spacing)
			Layout.preferredWidth: implicitWidth + (Constants.is_desktop ? 0 : Constants.text_spacing)
			text: (activated ?
				//: LABEL DESKTOP Screenreader text for the eye icon to change the password visibility
				qsTr("Press to hide the password") :
				//: LABEL DESKTOP Screenreader text for the eye icon to change the password visibility
				qsTr("Press to show the password"))

			background: TintableIcon {
				fillMode: Image.Pad
				source: eye.activated ? "qrc:///images/material_visibility.svg" : "qrc:///images/material_visibility_off.svg"
				sourceSize.height: Constants.is_desktop ? Style.dimens.large_icon_size : Style.dimens.small_icon_size
				tintColor: Style.color.secondary_text
			}
			contentItem: Item {
			}

			Accessible.onPressAction: clicked()
			onClicked: eye.activated = !eye.activated
			onVisibleChanged: if (visible)
				activated = false

			MouseArea {
				acceptedButtons: Qt.NoButton
				anchors.fill: parent
				cursorShape: Qt.PointingHandCursor
			}
			FocusFrame {
			}
		}
	}
}
