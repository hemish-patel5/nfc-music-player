import { useRef, useState } from 'react'
import type { ChangeEvent, DragEvent } from 'react'
import './App.css'

type Song = {
  id: number
  title: string
  artist: string
  album: string
}

type WriteStatus = 'idle' | 'writing' | 'complete'

const SONGS: Song[] = [
  { id: 1, title: "God's Plan", artist: 'Drake', album: 'Scorpion' },
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
    <svg viewBox="0 0 24 24" aria-hidden="true">
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
      <path d="M13 4h15l9 9v29H13z" />
      <path d="M28 4v10h9M25 21v13M25 24l7-2v10" />
      <circle cx="20.5" cy="34.5" r="4.5" />
      <circle cx="27.5" cy="32.5" r="4.5" />
    </svg>
  )
}

function NfcIcon({ compact = false }: { compact?: boolean }) {
  return (
    <svg
      className={compact ? 'nfc-icon nfc-icon--compact' : 'nfc-icon'}
      viewBox="0 0 120 120"
      aria-hidden="true"
    >
      {!compact && <circle cx="60" cy="60" r="47" />}
      <path d="M28 73V48l20 24V47" />
      <path d="M57 45c13 10 13 28 0 38M69 35c22 16 22 44 0 60M81 25c31 23 31 56 0 77" />
    </svg>
  )
}

function App() {
  const [search, setSearch] = useState('')
  const [selectedSongId, setSelectedSongId] = useState(1)
  const [isDragging, setIsDragging] = useState(false)
  const [uploadedFile, setUploadedFile] = useState<File | null>(null)
  const [writeStatus, setWriteStatus] = useState<WriteStatus>('idle')
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
    if (writeStatus === 'writing') return

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
    document.querySelector('.writer-panel')?.scrollIntoView({ behavior: 'smooth' })
  }

  const statusText =
    writeStatus === 'writing'
      ? 'Tag Detected - Writing...'
      : writeStatus === 'complete'
        ? 'Tag written successfully'
        : 'Tag Detected - Ready to Write'

  return (
    <main className="dashboard-shell">
      <section className="library-panel" aria-labelledby="library-title">
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
                    <span>{song.artist} • {song.album}</span>
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
      </section>

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
          <strong>{uploadedFile ? uploadedFile.name : 'DRAG & DROP MP3 FILE HERE'}</strong>
          <span>or</span>
          <em>[Browse Files]</em>
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
          disabled={writeStatus === 'writing'}
        >
          <NfcIcon compact />
          <span>{writeStatus === 'writing' ? 'WRITING TO NFC TAG...' : 'WRITE TO NFC TAG'}</span>
        </button>

        <p className={`write-status write-status--${writeStatus}`} role="status">
          <span className="status-spinner" aria-hidden="true" />
          {statusText}
        </p>
      </section>
    </main>
  )
}

export default App
