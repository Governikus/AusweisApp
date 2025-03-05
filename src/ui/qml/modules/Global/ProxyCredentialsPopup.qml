/**
 * Copyright (c) 2020-2025 Governikus GmbH & Co. KG, Germany
 */

import QtQuick

import Governikus.Global
import Governikus.Type
import Governikus.Style

ConfirmationPopup {
	id: root

	property ProxyCredentials credentials

	//: LABEL DESKTOP Text of the button in the proxy credentials popup.
	okButtonText: qsTr("Sign in")

	//: LABEL DESKTOP Title of the proxy credentials popup.
	title: qsTr("Sign in")
	width: formGrid.width + 2 * Style.dimens.pane_padding

	onClosed: {
		if (!credentials)
			return;
		credentials.confirmInput();
	}
	onConfirmed: {
		if (!credentials)
			return;
		credentials.user = userInput.text;
		credentials.password = passwordInput.text;
	}
	onCredentialsChanged: {
		//: LABEL DESKTOP Text of the proxy credentials popup. An example for %1 is http://proxy.example.com:1337.
		text = qsTr("The proxy %1 requires username and password.").arg(credentials ? credentials.url : "");
		userInput.text = credentials ? credentials.proposedUser : "";
		passwordInput.text = "";
	}

	Grid {
		id: formGrid

		columns: 2
		spacing: Style.dimens.pane_spacing
		verticalItemAlignment: Text.AlignVCenter

		GText {
			//: LABEL DESKTOP Accessible name.
			Accessible.name: qsTr("Proxy credential username")

			//: LABEL DESKTOP Label of the textfield for the username.
			text: qsTr("Username")
		}
		GTextField {
			id: userInput

			width: 500 * UiPluginModel.scaleFactor
		}
		GText {
			//: LABEL DESKTOP Accessible name.
			Accessible.name: qsTr("Proxy credential password")

			//: LABEL DESKTOP Label of the textfield for the password.
			text: qsTr("Password")
		}
		GTextField {
			id: passwordInput

			echoMode: TextInput.Password
			width: 500 * UiPluginModel.scaleFactor
		}
	}
}
