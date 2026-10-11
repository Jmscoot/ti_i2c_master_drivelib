# I2C-A Master (driverlib, polling)

F2837xD I2C-A를 마스터 모드로 동작시켜 슬레이브로부터 데이터를 읽는 예제.

## 주요 정보

| 항목 | 값 |
|---|---|
| 사용 포트 | I2C-A (`I2CA_BASE`) |
| SDA 핀 | GPIO104 |
| SCL 핀 | GPIO105 |
| 슬레이브 주소 | 0x50 (7bit) |
| 전송 속력 | 400kHz (Fast-mode, `I2C_FASTMODE_BITRATE`) |
| Duty Cycle | 33% (`I2C_DUTYCYCLE_33`) |
| SYSCLK | 200MHz |
| I2C 모듈 클럭 소스 | SYSCLK 직접 사용 (LSPCLK 안 거침) |
| 통신 방식 | 폴링 (인터럽트 미사용) |
| 풀업 | 내부 풀업만 사용 (외부 저항 없음) |
