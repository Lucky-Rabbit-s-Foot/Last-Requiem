# Last Requiem

> **NC AI VARCO 2nd Contest Entry** > **Development Period:** 2025.12.08 ~ 2025.12.31

![Project Status](https://img.shields.io/badge/Status-In%20Development-yellow)
![Engine](https://img.shields.io/badge/Unreal%20Engine-5.6-black)
![Language](https://img.shields.io/badge/C++-20-blue)
![AI](https://img.shields.io/badge/NC%20VARCO-AI%20Sound-purple)

## 📖 Project Overview
**Last Requiem**은 포스트 아포칼립스 세계관을 배경으로 한 **전략 디펜스 게임**입니다.
플레이어는 오퍼레이터가 되어 드론을 통해 제한된 전장 정보를 확인하고, 외계 생명체의 침입으로부터 최후의 방어선을 사수해야 합니다.

단순한 유닛 컨트롤을 넘어, 전장의 공포로 인해 통제 불능 상태에 빠지는 병사들의 **정신력(Sanity)**을 관리하는 것이 핵심입니다.

### 🎮 Key Concept
* **Drone View & Fog of War:** 플레이어는 1인칭 드론 시점으로 전장을 내려다보며, 드론이 비추지 않는 곳은 정보를 알 수 없는 제한된 시야(Fog of War) 속에서 전략을 수립해야 합니다.
* **Sanity & Panic System:** 병사들은 AI 적들과 마주하거나 전투 상황이 유지될 경우 정신력이 감소합니다. 패닉 상태에 빠진 유닛은 명령을 거부하거나 돌발 행동을 합니다.
* **Strategic Command UI:** 별도의 전략판 UI를 통해 유닛에게 이동 및 공격 명령을 하달합니다.

---

## 🛠 Tech Stack & Implementation

### Core Engine
* **Unreal Engine 5.6** (C++ Base)

### Key Systems
* **Gameplay Ability System (GAS):**
    * 적/아군의 스탯(Health, Sanity) 및 스킬(Attack, Fear Aura) 구현.
    * `AttributeSet`을 활용한 모듈식 스탯 관리.
* **AI Architecture:**
    * **Behavior Tree:** 아군 및 적군 기본 행동 패턴 정의.
    * **EQS (Environment Query System):** 적 AI의 전략적 침투 경로 탐색 및 공격 위치 선정.
* **Enhanced Input:** 드론의 6축 이동 및 카메라 줌 인/아웃 제어.
* **NC VARCO AI Integration:**
    * 유닛의 정신력(Sanity) 수치에 따라 실시간으로 변화하는 다이내믹 보이스 생성.
    * 정상 상태의 보고 vs 패닉 상태의 절규를 AI 보이스로 차별화하여 몰입감 극대화.

---

## 👥 Team Members
| Role | Github ID |
|:---:|:---:|
| **Developer** | [@Rabbit-M13](https://github.com/Rabbit-M13) |
| **Developer** | [@Christopher-Emunah-Kim](https://github.com/Christopher-Emunah-Kim) |
| **Developer** | [@jbaem](https://github.com/jbaem) |
| **Developer** | [@jackbongmin](https://github.com/jackbongmin) |

---

## 📅 Roadmap (Dec 2025)

### Phase 1: 기반 구축 (12.08 ~ 12.14)
- 프로젝트 세팅 및 핵심 클래스 아키텍처 설계 (GameMode, Pawn, Controller).
- Gameplay Ability System (GAS) 기본 구현.
- 맵 디자인 및 Fog of War(전장의 안개) 프로토타이핑.

### Phase 2: 알파 테스트 (12.15 ~ )
- **목표:** 핵심 메커니즘 검증 및 플레이 테스트.
- [ ] 드론 조작 (Enhanced Input) 및 카메라 시스템 구현.
- [ ] 기본 아군 유닛 AI (이동/공격) 및 적 AI 로직 (EQS) 구현.
- [ ] 전략판 명령 UI(Command UI) 로직 구현.

### Phase 3: 베타 테스트 (12.20 ~ )
- **목표:** 기능 완성, 콘텐츠 통합 및 밸런싱.
- [ ] **정신력(Sanity) 시스템:** 패닉 상태 및 명령 불복종 로직 완성.
- [ ] **NC VARCO AI:** 정신력 수치에 따른 다이내믹 보이스 통합.
- [ ] 웨이브 시스템 및 게임 루프 (승리/패배 조건) 구현.
- [ ] 버그 수정 및 폴리싱 (Bug fixing & Polish).

### Final Submission (12.31)
- 최종 빌드 패키징 및 시연 영상 제작.

---

## 📂 Installation & Build

1. **Prerequisites**
* Unreal Engine 5.6
* Visual Studio 2022
