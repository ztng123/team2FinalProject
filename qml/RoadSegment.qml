import QtQuick 2.15
import QtLocation 5.15
import QtPositioning 5.15

// 도로 세그먼트 컴포넌트
MapPolyline {
    property var startCoordinate
    property var endCoordinate
    property color lineColor: "#4285F4"
    property int lineWidth: 10

    line.width: lineWidth
    line.color: lineColor
    z: 1

    // 경로 설정
    path: [
        startCoordinate,
        endCoordinate
    ]

    // 초기화 완료 시
    Component.onCompleted: {
        if (startCoordinate && endCoordinate) {
            console.log("도로 세그먼트 생성: " +
                        startCoordinate.latitude.toFixed(6) + "," +
                        startCoordinate.longitude.toFixed(6) + " -> " +
                        endCoordinate.latitude.toFixed(6) + "," +
                        endCoordinate.longitude.toFixed(6));
        }
    }
}
