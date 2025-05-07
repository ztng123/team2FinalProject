import QtQuick 2.15
import QtQuick.Controls 2.15
import QtLocation 5.15
import QtPositioning 5.15

Item {
    id: root
    anchors.fill: parent

    // 속성들
    property double initialZoomLevel: 19.5
    property double initialLatitude: 37.334942
    property double initialLongitude: 127.103565
    property bool followMode: true
    property int updateCounter: 0
    property var lastPosition: QtPositioning.coordinate(initialLatitude, initialLongitude)
    property bool showRoadNetwork: true // 도로 네트워크 표시 여부

    // 도로 색상
    property color roadColor: "#4285F4"
    property int roadWidth: 10

    // 이전 매칭 결과를 저장할 변수 추가
    property var previousMatchedLocations: []
    property int smoothingFactor: 5 // 스무딩에 사용할 이전 위치 개수

    Map {
        id: map
        anchors.fill: parent
        plugin: Plugin {
            name: "osm"
            // 간단한 설정만 유지
            PluginParameter {
                name: "osm.mapping.highdpi_tiles"
                value: "true"
            }
        }
        zoomLevel: initialZoomLevel
        tilt: 45
        bearing: 0
        center: QtPositioning.coordinate(initialLatitude, initialLongitude)

        // 부드러운 움직임을 위한 애니메이션
        Behavior on center {
            CoordinateAnimation {
                duration: 800 // 애니메이션 시간 증가 (더 부드럽게)
                easing.type: Easing.OutCubic // 부드러운 이징 적용
            }
        }

        // 맵 초기화 완료 시 호출
        Component.onCompleted: {
            console.log("지도 초기화 완료");
            drawRoadNetwork();
        }
    }

    // 현재 화면에 보이는 도로들의 위치 저장
    property var visibleRoads: [
        // 도로 경도 값들 (화면에 보이는 실제 도로 위치에 맞게 조정)
        127.103550, 127.103565, 127.103580
    ]

    // 도로 네트워크 그리기 함수
    function drawRoadNetwork() {
        if (!showRoadNetwork) return;

        // 기존 도로 그래픽 요소 제거
        for (var i = map.mapItems.length - 1; i >= 0; i--) {
            if (map.mapItems[i].objectName === "roadSegment") {
                map.removeMapItem(map.mapItems[i]);
            }
        }

        // 도로 네트워크 데이터가 없으면 리턴
        if (typeof roadNetworkData === 'undefined' || roadNetworkData.length === 0) {
            console.log("도로 네트워크 데이터가 없어 그리기를 건너뜁니다.");
            return;
        }

        // 도로 네트워크 그리기
        for (var j = 0; j < roadNetworkData.length; j++) {
            var segment = roadNetworkData[j];

            // 내장 MapPolyline으로 도로 그리기
            var path = [];
            path.push(QtPositioning.coordinate(segment.startLat, segment.startLon));
            path.push(QtPositioning.coordinate(segment.endLat, segment.endLon));

            var polyline = Qt.createQmlObject('
                import QtLocation 5.15;
                MapPolyline {
                    objectName: "roadSegment"
                    line.width: ' + roadWidth + '
                    line.color: "' + roadColor + '"
                    z: 1
                }', map);

            polyline.path = path;
            map.addMapItem(polyline);
        }

        console.log("도로 네트워크 그리기 완료: " + roadNetworkData.length + "개 세그먼트");
    }

    // 차량 마커
    Image {
        id: carIcon
        source: "qrc:/images/car_marker.png"
        width: 60
        height: 60
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.bottom: parent.bottom
        anchors.bottomMargin: parent.height * 0.4
        z: 10

        // 위치 애니메이션
        Behavior on x {
            NumberAnimation {
                duration: 800
                easing.type: Easing.OutCubic
            }
        }
        Behavior on y {
            NumberAnimation {
                duration: 800
                easing.type: Easing.OutCubic
            }
        }
    }

    // 속도계
    Rectangle {
        id: speedometer
        width: 150
        height: 150
        radius: 75
        color: "#ffffff"
        border.color: "#cccccc"
        border.width: 2
        anchors {
            left: parent.left
            top: parent.top
            margins: 20
        }

        Text {
            id: speedText
            anchors.centerIn: parent
            text: "0"
            font.pixelSize: 50
            font.bold: true
        }

        Text {
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.top: speedText.bottom
            anchors.topMargin: -5
            text: "km/h"
            font.pixelSize: 16
            color: "#666"
        }
    }

    // 속도 제한 표시
    Rectangle {
        width: 80
        height: 80
        radius: 40
        color: "#e53935"
        border.color: "white"
        border.width: 3
        anchors.left: parent.left
        anchors.top: speedometer.bottom
        anchors.leftMargin: 60
        anchors.topMargin: 20

        Text {
            anchors.centerIn: parent
            text: "80"
            color: "white"
            font.pixelSize: 32
            font.bold: true
        }
    }

    // 경고창
    Rectangle {
        id: alertBox
        visible: false
        width: 500
        height: 200
        color: "#FF5252"
        radius: 10
        anchors.centerIn: parent
        z: 20

        Text {
            anchors.centerIn: parent
            text: "⚠️ 졸음 운전이 감지되었습니다!\n안전한 곳에 정차 후 휴식을 취하세요."
            color: "white"
            font.pixelSize: 18
            font.bold: true
            horizontalAlignment: Text.AlignHCenter
        }

        Button {
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.bottom: parent.bottom
            anchors.bottomMargin: 20
            text: "확인"
            onClicked: alertBox.visible = false
        }

        Timer {
            interval: 10000
            running: alertBox.visible
            onTriggered: alertBox.visible = false
        }
    }

    // 맵 매칭 함수 - 스무딩 적용 버전
    function mapMatchToRoad(position) {
        // 도로 위치 추정 - 가장 가까운 도로 선택
        var closestDistance = 9999;
        var closestLongitude = position.longitude;

        for (var i = 0; i < visibleRoads.length; i++) {
            var roadLon = visibleRoads[i];
            var distance = Math.abs(position.longitude - roadLon);

            if (distance < closestDistance) {
                closestDistance = distance;
                closestLongitude = roadLon;
            }
        }

        // 매칭된 위치 계산
        var rawMatchedPosition = QtPositioning.coordinate(position.latitude, closestLongitude);

        // 스무딩 적용
        if (previousMatchedLocations.length > 0) {
            // 이전 매칭 결과가 있는 경우 스무딩 적용
            var smoothedLongitude = closestLongitude;

            // 최대 smoothingFactor 개의 이전 위치 사용
            var count = Math.min(previousMatchedLocations.length, smoothingFactor);
            if (count > 0) {
                var totalLon = closestLongitude; // 현재 매칭 결과 포함
                for (var j = 0; j < count; j++) {
                    totalLon += previousMatchedLocations[j].longitude;
                }
                // 가중 평균 계산 (최근 위치에 더 많은 가중치 부여)
                smoothedLongitude = totalLon / (count + 1);
            }

            // 이전 매칭 결과와의 급격한 변화 방지 (최대 변화량 제한)
            var lastLon = previousMatchedLocations[0].longitude;
            var maxChange = 0.00001; // 최대 변화량 제한
            if (Math.abs(smoothedLongitude - lastLon) > maxChange) {
                // 변화량이 너무 크면 제한
                smoothedLongitude = lastLon + (smoothedLongitude > lastLon ? maxChange : -maxChange);
            }

            // 최종 매칭 위치 계산
            var matchedPosition = QtPositioning.coordinate(position.latitude, smoothedLongitude);

            // 이전 매칭 결과 배열 업데이트 (최신 결과가 앞에 오도록)
            previousMatchedLocations.unshift(matchedPosition);
            if (previousMatchedLocations.length > smoothingFactor) {
                previousMatchedLocations.pop(); // 가장 오래된 결과 제거
            }

            var distanceMeters = position.distanceTo(matchedPosition);
            console.log("화면 도로 매칭 결과 (스무딩 적용): " + distanceMeters.toFixed(2) + "m 거리 보정됨");
            return matchedPosition;
        } else {
            // 첫 번째 매칭인 경우 그대로 사용
            previousMatchedLocations.unshift(rawMatchedPosition);
            var distanceMeters = position.distanceTo(rawMatchedPosition);
            console.log("화면 도로 매칭 결과: " + distanceMeters.toFixed(2) + "m 거리 보정됨");
            return rawMatchedPosition;
        }
    }

    // 차량 위치 업데이트 함수
    function updateMarkerPosition(lat, lon, speed, bearing) {
        // 속도 표시 업데이트
        speedText.text = speed.toString();

        // 실제 위치 업데이트
        if (followMode) {
            // 원래 위치
            var position = QtPositioning.coordinate(lat, lon);

            // 맵 매칭 적용
            var matchedPosition = mapMatchToRoad(position);

            // 지도 중심 업데이트
            map.center = matchedPosition;

            // 디버깅 로그
            console.log("원래 위치: " + lat.toFixed(6) + ", " + lon.toFixed(6) +
                        ", 매칭된 위치: " + matchedPosition.latitude.toFixed(6) +
                        ", " + matchedPosition.longitude.toFixed(6) +
                        ", 거리: " + position.distanceTo(matchedPosition).toFixed(2) + "m" +
                        ", 속도: " + speed);

            // 현재 위치 저장
            lastPosition = matchedPosition;
        }
    }

    // 졸음운전 경고 함수
    function showDrowsinessAlert() {
        alertBox.visible = true;
    }

    // 초기화
    Component.onCompleted: {
        console.log("Map QML 로드 완료");
    }
}
