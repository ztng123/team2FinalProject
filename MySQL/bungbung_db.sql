drop database if exists bungbung_db;
create database bungbung_db default character set utf8mb4 collate utf8mb4_unicode_ci;

use bungbung_db;

create table driver_logs(
	id INT AUTO_INCREMENT PRIMARY KEY,
    driver_id VARCHAR(20),       -- 운전자 식별자 (옵션)
    detection_time DATETIME,      -- 감지 시각
    drowsiness_detected BOOLEAN   -- 졸음 감지 여부 (1=졸음, 0=정상)
);