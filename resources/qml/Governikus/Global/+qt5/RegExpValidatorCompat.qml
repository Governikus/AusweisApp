/**
 * Copyright (c) 2022-2023 Governikus GmbH & Co. KG, Germany
 */
import QtQuick 2.15

RegExpValidator {
	property var expression

	regExp: expression
}
