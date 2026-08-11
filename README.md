# NFC Mini Music Player

A standalone NFC-powered music player built with an **ESP32-S3**, **React**, **Supabase**, and a **microSD card**.

Physical NFC discs are assigned to songs, albums, or playlists using a web dashboard. When a disc is placed on the player, the ESP32 reads its NFC tag and plays the assigned music directly from local storage.

The dashboard is used to manage the player, while normal playback works completely offline.

---

## Project Overview

The goal of this project is to create a physical music player where small collectible discs act as the interface for choosing music.

Each disc contains an NFC tag.

When a disc is placed on the player:

1. The ESP32 reads the NFC UID.
2. The UID is matched against a locally cached configuration.
3. The assigned song or album is found on the microSD card.
4. The ESP32 plays the audio through a speaker.

A React dashboard allows discs to be reassigned without reprogramming the ESP32 or rewriting the NFC tag.

For example:

```text
Currents Disc
NFC UID: 04A7821932

        ↓

Assigned through dashboard

        ↓

Let It Happen
Tame Impala
```

The NFC tag only identifies the disc. The actual relationship between the disc and the music is stored in the application.

---

# System Architecture

```text
                         INTERNET

                ┌────────────────────┐
                │   React Dashboard  │
                │      Vercel        │
                └─────────┬──────────┘
                          │
                          │ HTTPS
                          ▼
                ┌────────────────────┐
                │      Supabase      │
                │                    │
                │ PostgreSQL         │
                │ Storage            │
                │ Authentication     │
                └─────────┬──────────┘
                          │
                          │ HTTPS
                          ▼
                ┌────────────────────┐
                │     ESP32-S3       │
                │                    │
                │ NFC Reader         │
                │ Sync Manager       │
                │ Audio Player       │
                └───────┬─────┬──────┘
                        │     │
                 ┌──────┘     └──────┐
                 ▼                   ▼
            microSD Card          PN532
                 │                   │
                 ▼                   ▼
              MP3 Files          NFC Disc
                 │
                 ▼
             MAX98357A
                 │
                 ▼
              Speaker
```

---

# Core Design Principle

The cloud manages configuration.

The ESP32 handles playback.

```text
Dashboard
    ↓
Supabase
    ↓
ESP32 synchronization
    ↓
microSD

----------------------------

NFC Disc
    ↓
ESP32
    ↓
microSD
    ↓
Speaker
```

Normal playback should never require an internet connection.

If Wi-Fi or Supabase is unavailable, previously synchronized discs and songs should continue working.

---

# Tech Stack

## Hardware

* ESP32-S3 development board
* PN532 NFC reader
* NTAG213 / NTAG215 / NTAG216 NFC tags
* microSD card module
* microSD card
* MAX98357A I2S audio amplifier
* 4Ω 3W speaker
* Rotary encoder
* Push buttons
* Optional RGB LEDs
* USB-C power
* Optional LiPo battery and charging circuit

## Firmware

* C++
* Arduino framework or ESP-IDF
* Wi-Fi
* HTTPS
* SPI
* I2C
* I2S
* FAT32 SD storage
* JSON configuration
* MP3 decoding

## Web Application

* React
* Vite
* JavaScript or TypeScript
* Supabase JavaScript client
* Vercel

## Backend

* Supabase
* PostgreSQL
* Supabase Storage
* Supabase Auth
* Row Level Security

---

# Main Features

## NFC Disc Playback

Each physical disc contains an NFC tag with a unique UID.

Example:

```text
04A7821932
```

The ESP32 maps the UID to a local music assignment.

```text
04A7821932
      ↓
track_id = 420
      ↓
/music/420.mp3
```

The MP3 is played directly from the microSD card.

---

## Web Dashboard

The React dashboard allows the user to:

* View registered discs
* Register new NFC discs
* Rename discs
* Assign songs to discs
* Assign albums to discs
* Assign playlists to discs
* Upload music
* Upload album artwork
* View device status
* View synchronization status
* View available storage
* See the currently playing song
* Remove or reassign discs

---

# Disc Assignment Workflow

Suppose a physical disc has:

```text
Name: Currents Disc
UID: 04A7821932
```

It is currently assigned to:

```text
Track 183
```

The user wants to change it to:

```text
Track 420
```

The synchronization process works as follows.

## 1. Open the React dashboard

```text
React Dashboard
      ↓
Disc Management
```

---

## 2. Change the NFC disc assignment

```text
Currents Disc

UID:
04A7821932

Current:
Track 183

New:
Track 420
```

The user presses:

```text
Save
```

---

## 3. React updates Supabase

The dashboard updates the relevant disc assignment.

Conceptually:

```text
disc_uid = 04A7821932

track_id:
183 → 420
```

---

## 4. Configuration version increases

Each music player has a configuration version.

Before:

```text
config_version = 37
```

After the change:

```text
config_version = 38
```

This allows the ESP32 to quickly determine whether anything has changed.

---

## 5. ESP32 polls Supabase

The ESP32 periodically checks its configuration version.

```text
ESP32 local version:

37


Supabase version:

38
```

Comparison:

```text
37 != 38
```

Therefore synchronization is required.

---

## 6. ESP32 downloads the new configuration

The ESP32 requests its latest configuration from Supabase.

Example response:

```json
{
  "config_version": 38,
  "discs": [
    {
      "uid": "04A7821932",
      "track_id": 420
    }
  ]
}
```

The ESP32 does not immediately activate the new configuration.

First it verifies that all required audio files are available.

---

## 7. Check whether the required track exists

The ESP32 checks the microSD card:

```text
/music/420.mp3
```

Decision:

```text
Does track 420 exist?

             │
        ┌────┴────┐
        │         │
       NO        YES
        │         │
        ▼         │
Download          │
420.mp3            │
        │         │
        └────┬────┘
             ▼
          Continue
```

If the file does not exist, the ESP32 downloads it from Supabase Storage.

```text
Supabase Storage
       ↓
     HTTPS
       ↓
     ESP32
       ↓
   microSD Card
       ↓
 /music/420.mp3
```

The file should be downloaded in small chunks rather than loaded completely into RAM.

---

## 8. Save the new configuration

Once all required files have successfully downloaded:

```text
local config_version = 38
```

The ESP32 stores the new configuration on the SD card.

Example:

```text
/config/device.json
```

```json
{
  "config_version": 38,
  "discs": {
    "04A7821932": {
      "track_id": 420
    }
  }
}
```

---

## 9. Device is ready

The ESP32 can now play the new assignment.

```text
Place Currents Disc

        ↓

PN532 reads:

04A7821932

        ↓

Local configuration:

04A7821932 → track 420

        ↓

/music/420.mp3

        ↓

MAX98357A

        ↓

Speaker
```

No cloud request is required during playback.

---

# Complete Synchronization Flow

```text
User changes disc
       │
       ▼
React Dashboard
       │
       ▼
Supabase Database
       │
       ▼
config_version
37 → 38
       │
       ▼
ESP32 polls Supabase
       │
       ▼
37 != 38
       │
       ▼
Download new configuration
       │
       ▼
Required track = 420
       │
       ▼
Is /music/420.mp3 available?
       │
    ┌──┴──┐
    │     │
   NO    YES
    │     │
    ▼     │
Download │
MP3      │
    │     │
    └──┬──┘
       │
       ▼
Validate files
       │
       ▼
Activate configuration
       │
       ▼
Save config_version = 38
       │
       ▼
Ready
```

---

# Offline Playback

After synchronization, internet access is not required.

```text
Internet
   ✕

NFC Disc
   ↓
PN532
   ↓
ESP32
   ↓
Local configuration
   ↓
microSD MP3
   ↓
Audio amplifier
   ↓
Speaker
```

This allows the music player to behave like a normal standalone appliance.

---

# Proposed Database Structure

## `devices`

```text
id
user_id
name
device_key
config_version
last_seen
firmware_version
created_at
```

Example:

```text
player_001
Bedroom Player
38
```

---

## `discs`

```text
id
device_id
nfc_uid
name
assignment_type
track_id
album_id
playlist_id
created_at
updated_at
```

Example:

```text
id: 12

device_id:
player_001

nfc_uid:
04A7821932

name:
Currents Disc

assignment_type:
track

track_id:
420
```

Only the relevant assignment field should be populated.

---

## `tracks`

```text
id
title
artist
album_id
storage_path
artwork_url
duration
created_at
```

Example:

```text
id:
420

title:
Let It Happen

artist:
Tame Impala

storage_path:
tracks/420.mp3
```

---

## `albums`

```text
id
title
artist
artwork_url
created_at
```

---

## `playlists`

```text
id
name
artwork_url
created_at
```

---

## `playlist_tracks`

```text
playlist_id
track_id
position
```

---

# Supabase Storage

A possible bucket structure:

```text
music/
├── tracks/
│   ├── 183.mp3
│   ├── 420.mp3
│   └── 512.mp3
│
└── artwork/
    ├── album-12.jpg
    ├── album-13.jpg
    └── playlist-4.jpg
```

The ESP32 downloads tracks from Supabase Storage and saves them locally.

```text
Supabase

tracks/420.mp3
      ↓
     HTTPS
      ↓
ESP32
      ↓
microSD

/music/420.mp3
```

---

# microSD Structure

```text
/
├── config/
│   └── device.json
│
├── music/
│   ├── 183.mp3
│   ├── 420.mp3
│   └── 512.mp3
│
├── artwork/
│   ├── 12.jpg
│   └── 13.jpg
│
└── logs/
    └── device.log
```

---

# React Dashboard Pages

## Dashboard

Displays:

* Device connection status
* Currently playing track
* Current NFC disc
* Storage usage
* Last synchronization
* Configuration version

---

## Discs

Displays all physical NFC discs.

Example:

```text
┌────────────────────────────┐
│ Currents Disc              │
│                            │
│ UID: 04A7821932            │
│                            │
│ Let It Happen              │
│ Tame Impala                │
│                            │
│ [ Change Music ]           │
└────────────────────────────┘
```

---

## Music Library

Allows users to:

* Browse tracks
* Add albums
* Add playlists
* Upload MP3 files
* Upload artwork
* Delete music

---

## Device

Displays:

```text
Bedroom Player

Status:
Online

Firmware:
1.0.0

Configuration:
38

Local storage:
4.7 GB / 32 GB

Last sync:
12 seconds ago
```

---

# New Disc Registration

A new NFC disc should not need to be manually programmed.

The ESP32 reads its UID.

```text
New NFC disc

UID:
04C29311B7
```

If the UID does not exist locally, the device reports it to the backend.

The dashboard displays:

```text
New Disc Detected

04C29311B7

Name
[                         ]

Assignment

○ Song
○ Album
○ Playlist

Music
[ Select...              ]

[ Register Disc ]
```

After registration:

```text
04C29311B7
      ↓
Supabase
      ↓
config_version increases
      ↓
ESP32 synchronizes
      ↓
Disc ready
```

---

# ESP32 Firmware Modules

A suggested firmware structure:

```text
firmware/
│
├── src/
│   ├── main.cpp
│   │
│   ├── wifi/
│   │   ├── wifi_manager.cpp
│   │   └── wifi_manager.h
│   │
│   ├── nfc/
│   │   ├── nfc_manager.cpp
│   │   └── nfc_manager.h
│   │
│   ├── audio/
│   │   ├── audio_player.cpp
│   │   └── audio_player.h
│   │
│   ├── storage/
│   │   ├── sd_manager.cpp
│   │   └── sd_manager.h
│   │
│   ├── sync/
│   │   ├── sync_manager.cpp
│   │   └── sync_manager.h
│   │
│   └── config/
│       ├── config_manager.cpp
│       └── config_manager.h
```

Responsibilities:

### `wifi_manager`

* Connect to Wi-Fi
* Reconnect after connection loss
* Report connection state

### `nfc_manager`

* Initialize PN532
* Detect NFC tags
* Read UID
* Detect disc removal

### `audio_player`

* Open MP3
* Decode audio
* Send audio through I2S
* Play
* Pause
* Stop
* Next track
* Previous track
* Volume

### `sd_manager`

* Initialize SD card
* Check whether files exist
* Write downloaded files
* Report free storage

### `sync_manager`

* Contact Supabase
* Compare configuration versions
* Download configuration
* Determine missing tracks
* Download missing tracks
* Activate updated configuration

### `config_manager`

* Load local configuration
* Save local configuration
* Find a disc by NFC UID
* Resolve assigned tracks

---

# Web Application Structure

```text
dashboard/
│
├── src/
│   ├── components/
│   │   ├── DiscCard.jsx
│   │   ├── TrackCard.jsx
│   │   ├── AlbumCard.jsx
│   │   ├── DeviceStatus.jsx
│   │   └── MusicUploader.jsx
│   │
│   ├── pages/
│   │   ├── Dashboard.jsx
│   │   ├── Discs.jsx
│   │   ├── Library.jsx
│   │   ├── Device.jsx
│   │   └── Settings.jsx
│   │
│   ├── services/
│   │   └── supabase.js
│   │
│   ├── App.jsx
│   └── main.jsx
│
├── package.json
└── vite.config.js
```

---

# Repository Structure

A monorepo could be used:

```text
nfc-music-player/
│
├── dashboard/
│   └── React application
│
├── firmware/
│   └── ESP32 firmware
│
├── supabase/
│   ├── migrations/
│   └── seed.sql
│
├── hardware/
│   ├── wiring/
│   ├── schematics/
│   └── enclosure/
│
├── docs/
│   ├── architecture.md
│   └── protocol.md
│
└── README.md
```

---

# Development Plan

## Phase 1 — Basic Audio

Goal:

Play an MP3 from an SD card through the ESP32.

Tasks:

* Set up ESP32-S3
* Connect microSD module
* Connect MAX98357A
* Connect speaker
* Initialize SD card
* Play one MP3 file
* Add volume control

Success condition:

```text
ESP32 → SD → MP3 → Speaker
```

works reliably.

---

## Phase 2 — NFC Playback

Goal:

Use an NFC disc to choose a local song.

Tasks:

* Connect PN532
* Read NFC UIDs
* Create several NFC discs
* Hard-code UID mappings
* Play corresponding MP3
* Stop playback when disc is removed
* Change playback when another disc is placed

Success condition:

```text
Disc A → Song A

Disc B → Song B
```

without Wi-Fi.

---

## Phase 3 — Local Configuration

Goal:

Remove hard-coded NFC mappings.

Tasks:

* Create `device.json`
* Store UID mappings on SD
* Load configuration during startup
* Look up songs dynamically

Success condition:

Changing `device.json` changes disc behaviour without recompiling firmware.

---

## Phase 4 — Supabase

Goal:

Synchronize configuration through the internet.

Tasks:

* Create Supabase project
* Create database tables
* Create device record
* Implement ESP32 HTTPS requests
* Retrieve `config_version`
* Download configuration
* Cache configuration locally

Success condition:

Changing the database causes the ESP32 configuration to update.

---

## Phase 5 — React Dashboard

Goal:

Manage discs from a web application.

Tasks:

* Create React + Vite app
* Connect Supabase client
* Create dashboard
* Create disc management page
* Create music library page
* Allow track reassignment
* Increment `config_version`
* Deploy to Vercel

Success condition:

```text
Dashboard
   ↓
Change Disc
   ↓
Supabase
   ↓
ESP32 Sync
```

works end-to-end.

---

## Phase 6 — Music Synchronization

Goal:

Automatically download required music.

Tasks:

* Configure Supabase Storage
* Upload MP3 files
* Store file locations in `tracks`
* ESP32 checks local music
* Download missing MP3s
* Stream downloads directly to SD
* Verify successful downloads
* Only activate config after files exist

Success condition:

A newly assigned track automatically appears on the player.

---

## Phase 7 — New Disc Registration

Goal:

Register blank NFC discs through the dashboard.

Tasks:

* Detect unknown UID
* Send UID to Supabase
* Display pending disc in dashboard
* Choose song/album/playlist
* Save assignment
* Synchronize ESP32

---

## Phase 8 — Product Features

Possible additions:

* Albums
* Playlists
* Shuffle mode
* Resume playback
* Volume persistence
* RGB status lights
* Physical volume knob
* Battery support
* Battery level reporting
* OTA firmware updates
* Device pairing
* Multiple music players
* Multiple users
* Listening history
* Recently played
* Track progress
* Remote playback controls

---

# MVP

The first complete MVP should support:

* [ ] ESP32-S3
* [ ] PN532 NFC reader
* [ ] microSD storage
* [ ] MAX98357A audio output
* [ ] MP3 playback
* [ ] NFC disc detection
* [ ] Local UID-to-track mapping
* [ ] Wi-Fi connection
* [ ] Supabase database
* [ ] React dashboard
* [ ] Vercel deployment
* [ ] Reassign disc from dashboard
* [ ] Configuration version synchronization
* [ ] ESP32 configuration download
* [ ] MP3 download from cloud storage
* [ ] Offline playback

---

# Future Goal

The finished product should feel like a normal physical music player:

```text
Pick a disc
     ↓
Place it on the player
     ↓
Music starts
```

The user should never need to think about NFC UIDs, databases, APIs, SD cards, or synchronization during normal use.

The technology should remain behind the interaction.

---

# Status

🚧 **Currently in development**

Initial focus:

```text
ESP32-S3
   ↓
microSD
   ↓
MP3 playback
   ↓
PN532 NFC
   ↓
local disc mapping
```

Cloud synchronization and the React dashboard will be added after the standalone playback system is reliable.
