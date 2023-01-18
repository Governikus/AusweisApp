/*
 * \copyright Copyright (c) 2022-2023 Governikus GmbH & Co. KG, Germany
 */
import QtQuick 2.15

RegularExpressionValidator {
	property var expression

	regularExpression: expression
}
