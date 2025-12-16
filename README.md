# Last Requiem

> **NC AI VARCO 2nd Contest Entry** 
>
> **Development Period:** 2025.12.08 ~ 2025.12.31

![Project Status](https://img.shields.io/badge/Status-In%20Development-yellow)
![Engine](https://img.shields.io/badge/Unreal%20Engine-5.6-black)
![Language](https://img.shields.io/badge/C++-20-blue)
![AI](https://img.shields.io/badge/NC%20VARCO-AI%20Sound-purple)


## 📖 Project Overview
**Last Requiem**은 포스트 아포칼립스 세계관을 배경으로 한 **전략 디펜스 게임**입니다.
플레이어는 오퍼레이터가 되어 드론을 통해 전장을 내려다보며, 외계 생명체의 침입으로부터 최후의 방어선을 사수해야 합니다.

단순한 유닛 컨트롤을 넘어, 전장의 공포로 인해 통제 불능 상태에 빠지는 병사들의 **정신력(Sanity)**을 관리하는 것이 핵심입니다.

### 🎮 Key Concept
* **Drone View**
    * 3인칭 드론 시점으로 전장을 비행하며 정보를 수집하고 아군 유닛을 지원합니다.
    * 아군 유닛의 체력 및 정신력을 회복하러 다녀야 합니다.
* **Sanity & Panic System:**
    * 아군 유닛들은 AI 적들과 마주하거나 전투 상황이 유지될 경우 정신력이 감소합니다.
    * 패닉 상태에 빠진 유닛은 명령을 거부하거나 돌발 행동을 합니다.
* **Strategic Command UI:**
    * 별도의 전략판 UI를 통해 유닛에게 이동 및 공격 명령을 하달합니다.

<br>

## NC VARCO AI Integration & Sanity
> **"전장의 공포를 생생한 목소리로 전달하다"**

본 프로젝트는 **NC VARCO TTS API**를 활용하여 정신력(Sanity) 수치에 따라 변화하는 보이스를 구현했습니다.
| 상황 (Context) | 정신력 (Sanity) | 행동 (Action) | AI Voice 예시 |
|:---:|:---:|:---:|:---|
| 광기 | 00 ~ 01 | 팀 공격<br>적에게 돌진 |
| 패닉 | 01 ~ 10 | 적에게 돌진           |
| 공포 | 10 ~ 20 | 후퇴                 |
| 긴장 | 20 ~ 40 | 확률적 명령 이행       |
| 노말 | 40 ~ 80 | 명령 이행             |
| 각성 | 80 ~ 100| 버프 부여             |


## 🛠 Tech Stack & Implementation

### Core Engine
* **Unreal Engine 5.6** (C++ Base)
* **IDE:** Visual Studio 2022 / Rider

<br>

## 👥 Team Members
| Name | Role | Github ID |
|:---:|:---:|:---:|
|김우빈| **Developer** | [@Rabbit-M13](https://github.com/Rabbit-M13) |
|김하신| **Developer** | [@Christopher-Emunah-Kim](https://github.com/Christopher-Emunah-Kim) |
|박준범| **Developer** | [@jbaem](https://github.com/jbaem) |
|백종민| **Developer** | [@jackbongmin](https://github.com/jackbongmin) |

---

## 📅 Roadmap (Dec 2025)

### Phase 1: 기반 구축 (12.08 ~ 12.14)
- 프로젝트 세팅 및 핵심 클래스 아키텍처 설계 (GameMode, Pawn, Controller).
- Gameplay Ability System (GAS) 기본 구현.
- 맵 디자인 및 프로토타이핑.

### Phase 2: 알파 테스트 (12.15 ~ )
- **목표:** 핵심 메커니즘 검증 및 플레이 테스트.
- 드론 조작 (Enhanced Input) 및 카메라 시스템 구현.
- 기본 아군 유닛 AI (이동/공격) 및 적 AI 로직 (EQS) 구현.
- 전략판 명령 UI(Command UI) 로직 구현.

### Phase 3: 베타 테스트 (12.20 ~ )
- **목표:** 기능 완성, 콘텐츠 통합 및 밸런싱.
- **정신력(Sanity) 시스템:** 패닉 상태 및 명령 불복종 로직 완성.
- **NC VARCO AI:** 정신력 수치에 따른 다이내믹 보이스 통합.
- 웨이브 시스템 및 게임 루프 (승리/패배 조건) 구현.
- 버그 수정 및 폴리싱 (Bug fixing & Polish).

### Final Submission (12.31)
- 최종 빌드 패키징 및 시연 영상 제작.

<br>

## 📂 Installation & Build

1. **Prerequisites**
* Unreal Engine 5.6
* Visual Studio 2022
