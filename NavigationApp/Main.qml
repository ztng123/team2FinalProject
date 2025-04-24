import QtQuick 2.15
import QtQuick.Controls 2.15
import QtLocation 5.15
import QtPositioning 5.15

ApplicationWindow {
    id: window
    visible: true
    width: 800
    height: 600
    title: "네비게이션 앱"

    // 지도 컴포넌트
    Map {
        id: map
        anchors.fill: parent
        plugin: Plugin {
            name: "osm" // OpenStreetMap 사용
        }

        // 초기 지도 중심 위치 설정 (예: 서울)
        center: QtPositioning.coordinate(37.5665, 126.9780)
        zoomLevel: 18
        // 7-10: 도시 레벨
        // 11-14: 구/군 레벨
        // 15-17: 동네/거리 레벨
        // 18-20: 건물 레벨

        // 현재 위치 표시 마커
        MapQuickItem {
            id: marker
            coordinate: map.center
            anchorPoint.x: sourceItem.width / 2
            anchorPoint.y: sourceItem.height

            sourceItem: Image {
                id: markerImage
                source: "location.png" // 위치 마커 이미지
                width: 32
                height: 32
            }
        }
    }

    // 졸음 경고 팝업
    Popup {
        id: drowsinessAlert
        width: 400
        height: 200
        x: (parent.width - width) / 2
        y: (parent.height - height) / 2
        modal: true

        background: Rectangle {
            color: "#FFCCCC"
            border.color: "red"
            border.width: 2
            radius: 10
        }

        Column {
            anchors.centerIn: parent
            spacing: 20

            Text {
                anchors.horizontalCenter: parent.horizontalCenter
                text: "주의! 졸음 운전이 감지되었습니다!"
                font.bold: true
                font.pixelSize: 18
                color: "red"
            }

            Text {
                anchors.horizontalCenter: parent.horizontalCenter
                text: "안전한 곳에 차를 세우고 휴식을 취하세요."
                font.pixelSize: 16
            }

            Button {
                anchors.horizontalCenter: parent.horizontalCenter
                text: "확인"
                onClicked: drowsinessAlert.close()
            }
        }
    }

    // 테스트용 버튼
    Button {
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        anchors.margins: 20
        text: "졸음 시뮬레이션"
        onClicked: drowsinessDetector.simulateDrowsiness()
    }

    // 졸음 감지 이벤트 처리
    Connections {
        target: drowsinessDetector
        function onDrowsinessDetected() {
            drowsinessAlert.open()
        }
    }

    Component.onCompleted: {
        drowsinessDetector.startMonitoring()
    }
}
