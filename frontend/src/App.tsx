import { useRef, useState } from 'react'
import type { ChangeEvent, DragEvent } from 'react'
import './App.css'
import placeholderAlbumCover from './assets/placeholder-album-cover.png'

type Song = {
  id: number
  title: string
  artist: string
  album: string
}

type WriteStatus = 'idle' | 'writing' | 'complete'

const SONGS: Song[] = [
  { id: 1, title: 'God’s Plan', artist: 'Drake', album: 'Scorpion' },
  { id: 2, title: 'Tech Noir', artist: 'Gunship', album: 'Gunship' },
  { id: 3, title: 'Midnight City', artist: 'M83', album: "Hurry Up, We're Dreaming" },
  { id: 4, title: 'Blood // Water', artist: 'grandson', album: 'A Modern Tragedy Vol. 1' },
  { id: 5, title: 'Midnight Drive', artist: 'Timecop1983', album: 'Lost in Your Eyes' },
  { id: 6, title: 'Electric', artist: 'Alina Baraz feat. Khalid', album: 'Urban Flora' },
  { id: 7, title: 'Neon Dreams', artist: 'The Midnight', album: 'Kids' },
  { id: 8, title: 'Drive', artist: 'College', album: 'Secret Diary' },
  { id: 9, title: 'Nightcall', artist: 'Kavinsky', album: 'OutRun' },
  { id: 10, title: 'After Dark', artist: 'Mr.Kitty', album: 'Time' },
  { id: 11, title: 'Resonance', artist: 'HOME', album: 'Odyssey' },
  { id: 12, title: 'Shadows', artist: 'Gunship', album: 'Dark All Day' },
]

function SearchIcon() {
  return (
    <svg viewBox="4 4 17 17" aria-hidden="true">
      <circle cx="10.8" cy="10.8" r="6.2" />
      <path d="m15.5 15.5 4.1 4.1" />
    </svg>
  )
}

function MusicIcon() {
  return (
    <svg viewBox="0 0 24 24" aria-hidden="true">
      <path d="M9 18V6.8l9-2v10.4" />
      <path d="M9 15.7c-3.7-.7-5.5.7-5.5 2.5 0 1.5 1.4 2.4 3 2.2 1.6-.2 2.5-1.2 2.5-2.8M18 13c-3.7-.7-5.5.7-5.5 2.5 0 1.5 1.4 2.4 3 2.2 1.6-.2 2.5-1.2 2.5-2.8" />
    </svg>
  )
}

function FileMusicIcon() {
  return (
    <svg viewBox="0 0 48 48" aria-hidden="true">
      <path d="M8 2h20l11 11v33H8z" />
      <path d="M28 2v12h11M24 20v16M24 24l9-2v12" />
      <circle cx="18.5" cy="37" r="4.5" />
      <circle cx="28.5" cy="34" r="4.5" />
    </svg>
  )
}

function NfcIcon({ compact = false }: { compact?: boolean }) {
  if (!compact) {
    return (
      <span className="reader-symbol" aria-hidden="true">
        <svg width="69" height="68" viewBox="0 0 69 68" fill="none" xmlns="http://www.w3.org/2000/svg">
          <path
            d="M1.80554 45.4003V23.7336L18.6083 41.7892V21.0253M27.4518 19.2197C36.8849 28.8494 36.8849 38.7799 27.4518 49.0114M39.8328 10.192C54.572 25.8401 54.572 41.7892 39.8328 58.0392M53.0981 1.16418C71.3747 22.8309 71.3747 44.4975 53.0981 66.1642"
            stroke="#FA233B"
            strokeWidth="3.61111"
          />
        </svg>
      </span>
    )
  }

  return (
    <svg
      className="nfc-icon nfc-icon--compact"
      viewBox="20 20 96 84"
      aria-hidden="true"
    >
      <path d="M28 73V48l20 24V47" />
      <path d="M57 45c13 10 13 28 0 38M69 35c22 16 22 44 0 60M81 25c31 23 31 56 0 77" />
    </svg>
  )
}

function WritingSpinner() {
  return (
    <span className="status-spinner" aria-hidden="true">
      <span /><span /><span /><span /><span /><span />
      <span /><span /><span /><span /><span /><span />
    </span>
  )
}

const WAVEFORM_HEIGHTS = [
  5, 6, 7, 9, 13, 19, 25, 42, 76, 54, 22, 45, 65, 43, 22, 11, 12, 17, 35,
  63, 39, 18, 10, 16, 23, 39, 63, 90, 65, 33, 17, 9, 12, 19, 12, 8, 11, 20,
  32, 51, 34, 15, 10, 14, 23, 42, 32, 18, 11, 9, 18, 29, 42, 29, 14, 8, 5, 4,
]

function ShuffleIcon() {
  return (
    <svg viewBox="0 0 32 32" aria-hidden="true">
      <path d="M3 8h4c7.5 0 8.5 16 17 16h5M24 19l5 5-5 5M3 24h4c2.6 0 4.5-2 6.2-4.7M20 8.7C21.2 8.2 22.5 8 24 8h5M24 3l5 5-5 5" />
    </svg>
  )
}

function RepeatIcon() {
  return (
    <svg viewBox="0 0 32 32" aria-hidden="true">
      <path d="M27 12V8H8a5 5 0 0 0-5 5v1M22 3l5 5-5 5M5 20v4h19a5 5 0 0 0 5-5v-1M10 29l-5-5 5-5" />
    </svg>
  )
}

function SkipIcon({ direction }: { direction: 'previous' | 'next' }) {
  return (
    <svg viewBox="0 0 32 32" aria-hidden="true">
      {direction === 'previous' ? (
        <><path d="M27 5 9 16l18 11V5Z" /><path d="M5 5v22" /></>
      ) : (
        <><path d="m5 5 18 11L5 27V5Z" /><path d="M27 5v22" /></>
      )}
    </svg>
  )
}

function SelectedSongPlayer({
  song,
  isPlaying,
  onTogglePlaying,
}: {
  song: Song
  isPlaying: boolean
  onTogglePlaying: () => void
}) {
  return (
    <section className="selected-player" aria-labelledby="selected-song-title">
      <h1 id="selected-song-title">Selected Song</h1>

      <div className="vinyl-record" aria-hidden="true">
        <img src={placeholderAlbumCover} alt="" />
      </div>

      <div className="selected-song-copy">
        <h2>{song.title}</h2>
        <p>{song.artist}</p>
      </div>

      <div className="waveform" aria-hidden="true">
        {WAVEFORM_HEIGHTS.map((height, index) => (
          <span key={index} style={{ height }} />
        ))}
      </div>

      <div className="track-progress">
        <div className="track-progress__rail">
          <span />
          <i />
        </div>
        <div className="track-progress__time">
          <span>2:15</span>
          <span>4:32</span>
        </div>
      </div>

      <div className="player-controls">
        <button type="button" aria-label="Shuffle"><ShuffleIcon /></button>
        <button type="button" aria-label="Previous song"><SkipIcon direction="previous" /></button>
        <button
          className="play-pause-button"
          type="button"
          aria-label={isPlaying ? 'Pause' : 'Play'}
          onClick={onTogglePlaying}
        >
          {isPlaying ? <span className="pause-bars" aria-hidden="true"><i /><i /></span> : <span className="play-triangle" aria-hidden="true" />}
        </button>
        <button type="button" aria-label="Next song"><SkipIcon direction="next" /></button>
        <button type="button" aria-label="Shuffle queue"><ShuffleIcon /></button>
        <button type="button" aria-label="Repeat"><RepeatIcon /></button>
      </div>
    </section>
  )
}

function App() {
  const [search, setSearch] = useState('')
  const [selectedSongId, setSelectedSongId] = useState(1)
  const [isPlayerOpen, setIsPlayerOpen] = useState(false)
  const [isPlaying, setIsPlaying] = useState(true)
  const [isDragging, setIsDragging] = useState(false)
  const [uploadedFile, setUploadedFile] = useState<File | null>(null)
  const [writeStatus, setWriteStatus] = useState<WriteStatus>('writing')
  const [metadata, setMetadata] = useState({
    title: 'Shadows',
    artist: 'Gunship',
    album: 'Dark All Day',
  })
  const writeTimer = useRef<number | null>(null)

  const matchingSongs = SONGS.filter((song) => {
    const query = search.trim().toLowerCase()
    return (
      query.length === 0 ||
      `${song.title} ${song.artist} ${song.album}`.toLowerCase().includes(query)
    )
  })
  const visibleSongs = matchingSongs.slice(0, 8)
  const selectedSong = SONGS.find((song) => song.id === selectedSongId) ?? SONGS[0]

  const selectSong = (song: Song) => {
    setSelectedSongId(song.id)
    setMetadata({ title: song.title, artist: song.artist, album: song.album })
    setWriteStatus('idle')
  }

  const acceptFile = (file?: File) => {
    if (!file || (!file.name.toLowerCase().endsWith('.mp3') && file.type !== 'audio/mpeg')) {
      return
    }

    const title = file.name.replace(/\.mp3$/i, '').replace(/[_-]+/g, ' ')
    setUploadedFile(file)
    setMetadata((current) => ({ ...current, title }))
    setWriteStatus('idle')
  }

  const handleFileInput = (event: ChangeEvent<HTMLInputElement>) => {
    acceptFile(event.target.files?.[0])
  }

  const handleDrop = (event: DragEvent<HTMLLabelElement>) => {
    event.preventDefault()
    setIsDragging(false)
    acceptFile(event.dataTransfer.files[0])
  }

  const writeToTag = () => {
    if (writeTimer.current !== null) {
      window.clearTimeout(writeTimer.current)
    }

    setWriteStatus('writing')
    writeTimer.current = window.setTimeout(() => {
      setWriteStatus('complete')
    }, 1800)
  }

  const continueWithSelection = () => {
    const song = SONGS.find((item) => item.id === selectedSongId)
    if (song) selectSong(song)
    setIsPlayerOpen(true)
    setIsPlaying(true)
  }

  const statusText =
    writeStatus === 'writing'
      ? 'Tag Detected - Writing...'
      : writeStatus === 'complete'
        ? 'Tag written successfully'
        : 'Tag Detected - Ready to Write'

  return (
    <main className="dashboard-shell">
      {isPlayerOpen ? (
        <SelectedSongPlayer
          song={selectedSong}
          isPlaying={isPlaying}
          onTogglePlaying={() => setIsPlaying((current) => !current)}
        />
      ) : <section className="library-panel" aria-labelledby="library-title">
        <div className="library-content">
          <header className="library-header">
            <div>
              <h1 id="library-title">Saved Song Library</h1>
              <p>Your saved tracks, ready for quick access.</p>
            </div>
            <span className="saved-count">{SONGS.length} saved</span>
          </header>

          <label className="search-box">
            <SearchIcon />
            <span className="sr-only">Search saved songs</span>
            <input
              type="search"
              value={search}
              onChange={(event) => setSearch(event.target.value)}
              placeholder="Search saved songs"
            />
          </label>

          <div className="song-list" aria-live="polite">
            {visibleSongs.map((song) => {
              const isSelected = song.id === selectedSongId
              return (
                <button
                  className={`song-row${isSelected ? ' song-row--selected' : ''}`}
                  type="button"
                  key={song.id}
                  onClick={() => selectSong(song)}
                  aria-pressed={isSelected}
                >
                  <span className="song-art"><MusicIcon /></span>
                  <span className="song-copy">
                    <strong>{song.title}</strong>
                    <span>{song.artist}{isSelected ? '' : ` • ${song.album}`}</span>
                  </span>
                  <span className="song-indicator" aria-hidden="true" />
                </button>
              )
            })}

            {visibleSongs.length === 0 && (
              <div className="empty-library">No saved songs match “{search}”.</div>
            )}
          </div>
        </div>

        <footer className="library-footer">
          <span>{visibleSongs.length} of {matchingSongs.length} saved songs shown</span>
          <button type="button" onClick={continueWithSelection}>Select</button>
        </footer>
      </section>}

      <div className="panel-divider" aria-hidden="true" />

      <section className="writer-panel" aria-labelledby="writer-title">
        <h1 id="writer-title">NFC Tag Manager &amp; MP3 Writer</h1>

        <div className="reader-card" aria-label="NFC reader connected">
          <NfcIcon />
        </div>
        <p className="reader-status">NFC Reader Connected - Ready</p>

        <h2>MP3 File Upload</h2>
        <label
          className={`drop-zone${isDragging ? ' drop-zone--dragging' : ''}`}
          onDragEnter={(event) => {
            event.preventDefault()
            setIsDragging(true)
          }}
          onDragOver={(event) => event.preventDefault()}
          onDragLeave={() => setIsDragging(false)}
          onDrop={handleDrop}
        >
          <input type="file" accept=".mp3,audio/mpeg" onChange={handleFileInput} />
          <FileMusicIcon />
          <span className="upload-instructions">
            <strong>{uploadedFile ? uploadedFile.name : 'DRAG & DROP MP3 FILE HERE'}</strong>
            <span>or</span>
            <em>[Browse Files]</em>
          </span>
        </label>

        <div className="metadata-grid">
          <label>
            <span>Title</span>
            <input
              value={metadata.title}
              onChange={(event) => setMetadata({ ...metadata, title: event.target.value })}
            />
          </label>
          <label>
            <span>Artist</span>
            <input
              value={metadata.artist}
              onChange={(event) => setMetadata({ ...metadata, artist: event.target.value })}
            />
          </label>
          <label>
            <span>Album</span>
            <input
              value={metadata.album}
              onChange={(event) => setMetadata({ ...metadata, album: event.target.value })}
            />
          </label>
        </div>

        <button
          className="write-button"
          type="button"
          onClick={writeToTag}
        >
          <NfcIcon compact />
          <span>WRITE TO NFC TAG</span>
        </button>

        <p className={`write-status write-status--${writeStatus}`} role="status">
          <WritingSpinner />
          <span className="write-message">{statusText}</span>
        </p>
      </section>
    </main>
  )
}

export default App
