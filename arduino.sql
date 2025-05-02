drop database if exists bungbung_db;
create database bungbung_db default character set utf8mb4 collate utf8mb4_unicode_ci;

use bungbung_db;

-- 졸음 감지 테이블 --

DROP table driver_drown_logs;

create table driver_drown_logs( 
	id INT AUTO_INCREMENT PRIMARY KEY,
    car_number VARCHAR(20),       -- 운전자 식별자 (옵션)
    location VARCHAR(100), -- 감지된 장소
    detection_time DATETIME NOT NULL,      -- 감지 시각
    closed_time FLOAT,     -- 눈을 감은 시간
	warning_level INT   -- 졸음 경고 레벨 (2=졸음 (3초) 1=졸음경고 (2초), 0=정상)
);

CREATE TABLE pedestrian_detection_log (
    id INT AUTO_INCREMENT PRIMARY KEY,
	detection_time DATETIME NOT NULL,
    distance_meters FLOAT,  -- 보행자까지의 거리
    braking_distance FLOAT, -- 제동거리
    emergency_brake BOOLEAN -- 정지 신호 여부 (제동거리 >= 보행자거리)
);