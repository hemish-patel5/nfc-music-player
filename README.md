# NFC Music Player


1. You open React dashboard

        ↓

2. Change NFC disc

   Currents Disc
   04A7821932

   → Track 420

        ↓

3. React updates Supabase

        ↓

4. config_version:
   37 → 38

        ↓

5. ESP32 polls Supabase

   local = 37
   cloud = 38

        ↓

6. ESP32 downloads config

        ↓

7. Does track 420 exist?

        │
     ┌──┴──┐
     NO   YES
     │      │
     ▼      │
 download   │
 MP3        │
     │      │
     └──┬───┘
        ▼

8. Save config_version = 38

        ↓

9. Ready
