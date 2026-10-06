import { useRef, useState } from 'react'
import type { ChangeEvent, DragEvent } from 'react'
import scorpionAlbumCover from './assets/scorpion-album-cover.png'

type Song = { id: number; title: string; artist: string; album: string }
type WriteStatus = 'idle' | 'writing' | 'complete'

const RED = '#fa233b'

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

const WAVEFORM_HEIGHTS = [
  5, 6, 7, 9, 13, 19, 25, 42, 76, 54, 22, 45, 65, 43, 22, 11, 12, 17, 35,
  63, 39, 18, 10, 16, 23, 39, 63, 90, 65, 33, 17, 9, 12, 19, 12, 8, 11, 20,
  32, 51, 34, 15, 10, 14, 23, 42, 32, 18, 11, 9, 18, 29, 42, 29, 14, 8, 5, 4,
]

const SPINNER_DOTS = [
  { left: 18.29, top: 6, opacity: 1 }, { left: 15, top: 2.71, opacity: 0.92 },
  { left: 10.5, top: 1.5, opacity: 0.83 }, { left: 6, top: 2.71, opacity: 0.75 },
  { left: 2.71, top: 6, opacity: 0.67 }, { left: 1.5, top: 10.5, opacity: 0.58 },
  { left: 2.71, top: 15, opacity: 0.5 }, { left: 6, top: 18.29, opacity: 0.42 },
  { left: 10.5, top: 19.5, opacity: 0.33 }, { left: 15, top: 18.29, opacity: 0.25 },
  { left: 18.29, top: 15, opacity: 0.17 }, { left: 19.5, top: 10.5, opacity: 0.08 },
]

const PLAYER_ICON_CLASS = 'h-8 w-8 fill-none stroke-current stroke-[2.2] [stroke-linecap:round] [stroke-linejoin:round]'
const PLAYER_CONTROL_CLASS = 'absolute top-[10px] grid h-12 w-[42px] cursor-pointer place-items-center border-0 bg-transparent p-0 text-[#fa233b] hover:brightness-90 focus-visible:outline-[3px] focus-visible:outline-offset-2 focus-visible:outline-[#fa233b]/20'

function SearchIcon() {
  return <svg className="h-[18px] w-[18px] shrink-0 fill-none stroke-[#77777f] stroke-2 [stroke-linecap:round]" viewBox="4 4 17 17" aria-hidden="true"><circle cx="10.8" cy="10.8" r="6.2" /><path d="m15.5 15.5 4.1 4.1" /></svg>
}

function MusicIcon() {
  return <svg className="h-[22px] w-[22px] fill-none stroke-current stroke-2 [stroke-linecap:round] [stroke-linejoin:round]" viewBox="0 0 24 24" aria-hidden="true"><path d="M9 18V6.8l9-2v10.4" /><path d="M9 15.7c-3.7-.7-5.5.7-5.5 2.5 0 1.5 1.4 2.4 3 2.2 1.6-.2 2.5-1.2 2.5-2.8M18 13c-3.7-.7-5.5.7-5.5 2.5 0 1.5 1.4 2.4 3 2.2 1.6-.2 2.5-1.2 2.5-2.8" /></svg>
}

function FileMusicIcon() {
  return <svg className="h-11 w-11 fill-none stroke-current stroke-2 [stroke-linecap:round] [stroke-linejoin:round]" viewBox="0 0 48 48" aria-hidden="true"><path d="M8 2h20l11 11v33H8z" /><path d="M28 2v12h11M24 20v16M24 24l9-2v12" /><circle cx="18.5" cy="37" r="4.5" /><circle cx="28.5" cy="34" r="4.5" /></svg>
}

function NfcIcon({ compact = false }: { compact?: boolean }) {
  if (!compact) return (
    <span className="grid h-[101px] w-[101px] place-items-center rounded-full border-4 border-[#fa233b]" aria-hidden="true">
      <svg width="69" height="68" viewBox="0 0 69 68" fill="none" xmlns="http://www.w3.org/2000/svg"><path d="M1.80554 45.4003V23.7336L18.6083 41.7892V21.0253M27.4518 19.2197C36.8849 28.8494 36.8849 38.7799 27.4518 49.0114M39.8328 10.192C54.572 25.8401 54.572 41.7892 39.8328 58.0392M53.0981 1.16418C71.3747 22.8309 71.3747 44.4975 53.0981 66.1642" stroke={RED} strokeWidth="3.61111" /></svg>
    </span>
  )
  return <svg className="h-[43px] w-[43px] fill-none stroke-current stroke-[5] [stroke-linecap:round] [stroke-linejoin:round]" viewBox="20 20 96 84" aria-hidden="true"><path d="M28 73V48l20 24V47" /><path d="M57 45c13 10 13 28 0 38M69 35c22 16 22 44 0 60M81 25c31 23 31 56 0 77" /></svg>
}

function WritingSpinner({ complete }: { complete: boolean }) {
  return <span className={`relative h-6 w-6 shrink-0 ${complete ? 'rounded-full border-2 border-[#278447] bg-[#eaf8ef]' : 'animate-spin motion-reduce:animate-none'}`} aria-hidden="true">
    {!complete && SPINNER_DOTS.map((dot, index) => <span className="absolute h-[3px] w-[3px] rounded-full bg-[#77777f]" key={index} style={dot} />)}
  </span>
}

function ShuffleIcon() {
  return <svg className={PLAYER_ICON_CLASS} viewBox="0 0 32 32" aria-hidden="true"><path d="M3 8h4c7.5 0 8.5 16 17 16h5M24 19l5 5-5 5M3 24h4c2.6 0 4.5-2 6.2-4.7M20 8.7C21.2 8.2 22.5 8 24 8h5M24 3l5 5-5 5" /></svg>
}

function RepeatIcon() {
  return <svg className={PLAYER_ICON_CLASS} viewBox="0 0 32 32" aria-hidden="true"><path d="M27 12V8H8a5 5 0 0 0-5 5v1M22 3l5 5-5 5M5 20v4h19a5 5 0 0 0 5-5v-1M10 29l-5-5 5-5" /></svg>
}

function SkipIcon({ direction }: { direction: 'previous' | 'next' }) {
  return <svg className={PLAYER_ICON_CLASS} viewBox="0 0 32 32" aria-hidden="true">{direction === 'previous' ? <><path d="M27 5 9 16l18 11V5Z" /><path d="M5 5v22" /></> : <><path d="m5 5 18 11L5 27V5Z" /><path d="M27 5v22" /></>}</svg>
}

function SelectedSongPlayer({ song, isPlaying, onTogglePlaying, onBack }: { song: Song; isPlaying: boolean; onTogglePlaying: () => void; onBack: () => void }) {
  return (
    <section className="relative h-full min-w-0 overflow-hidden border border-[#e7e7ec] bg-white max-[760px]:mx-auto max-[760px]:min-h-[803px] max-[760px]:w-[610px] max-[520px]:left-1/2 max-[520px]:ml-[-305px]" aria-labelledby="selected-song-title">
      <h1 className="absolute top-1 left-[21px] z-1 m-0 text-[26px] leading-[31px] font-bold text-[#202024]" id="selected-song-title">Selected Song</h1>
      <button className="absolute top-[5px] right-5 z-2 grid h-[38px] w-[38px] cursor-pointer place-items-center rounded-[10px] border-0 bg-[#fa233b] p-0 text-white transition-colors duration-150 hover:bg-[#e91d34] focus-visible:outline-[3px] focus-visible:outline-offset-2 focus-visible:outline-[#fa233b]/20" type="button" onClick={onBack} aria-label="Back to song library"><svg className="h-6 w-6 fill-none stroke-current stroke-[2.25] [stroke-linecap:round] [stroke-linejoin:round]" viewBox="0 0 24 24" aria-hidden="true"><path d="m10 6-5 5 5 5" /><path d="M5 11h7.5a6.5 6.5 0 0 1 6.5 6.5V19" /></svg></button>

      <div className="absolute top-[42px] left-[95px] h-[412px] w-[420px] rounded-full border-[5px] border-[#fa233b] bg-[#202024] [background:repeating-radial-gradient(ellipse_at_center,transparent_0_8px,#35353a_8px_9px),#202024] shadow-[0_14px_25px_rgba(32,32,36,0.18)]" aria-hidden="true"><img className="absolute top-[68px] left-[70px] h-[268px] w-[268px] object-cover" src={scorpionAlbumCover} alt="" /></div>
      <div className="absolute top-[478px] left-[95px] flex w-[420px] flex-col items-center text-center"><h2 className="m-0 w-full overflow-hidden text-center text-[32px] leading-[39px] font-bold text-ellipsis whitespace-nowrap text-[#202024]">{song.title}</h2><p className="m-0 w-full overflow-hidden text-center text-[21px] leading-[26px] font-medium text-ellipsis whitespace-nowrap text-[#fa233b]">{song.artist}</p></div>
      <div className="absolute top-[559px] left-[39px] flex h-[91px] w-[530px] items-center justify-between" aria-hidden="true">{WAVEFORM_HEIGHTS.map((height, index) => <span className="w-1 shrink-0 rounded-sm bg-[#fa233b]" key={index} style={{ height }} />)}</div>

      <div className="absolute top-[671px] left-[39px] w-[530px]"><div className="relative h-1 w-full rounded-sm bg-[#e7e7ec]"><span className="block h-1 w-[261px] rounded-sm bg-[#fa233b]" /><i className="absolute top-[-6px] left-[253px] h-4 w-4 rounded-full bg-[#fa233b]" /></div><div className="mt-[10px] flex justify-between text-base leading-[19px] text-[#77777f]"><span>2:15</span><span>4:32</span></div></div>

      <div className="absolute right-0 bottom-5 left-0 h-[68px]">
        <button className={`${PLAYER_CONTROL_CLASS} left-[34px]`} type="button" aria-label="Shuffle"><ShuffleIcon /></button>
        <button className={`${PLAYER_CONTROL_CLASS} left-[204px]`} type="button" aria-label="Previous song"><SkipIcon direction="previous" /></button>
        <button className="absolute top-0 left-[270px] grid h-[68px] w-[68px] cursor-pointer place-items-center rounded-full border-0 bg-[#fa233b] p-0 text-white hover:brightness-90 focus-visible:outline-[3px] focus-visible:outline-offset-2 focus-visible:outline-[#fa233b]/20" type="button" aria-label={isPlaying ? 'Pause' : 'Play'} onClick={onTogglePlaying}>
          {isPlaying ? <span className="flex gap-[7px]" aria-hidden="true"><i className="h-[29px] w-[7px] rounded-sm bg-white" /><i className="h-[29px] w-[7px] rounded-sm bg-white" /></span> : <span className="ml-[5px] h-0 w-0 border-y-[14px] border-y-transparent border-l-[23px] border-l-white" aria-hidden="true" />}
        </button>
        <button className={`${PLAYER_CONTROL_CLASS} left-[361px]`} type="button" aria-label="Next song"><SkipIcon direction="next" /></button>
        <button className={`${PLAYER_CONTROL_CLASS} left-[452px]`} type="button" aria-label="Shuffle queue"><ShuffleIcon /></button>
        <button className={`${PLAYER_CONTROL_CLASS} left-[528px]`} type="button" aria-label="Repeat"><RepeatIcon /></button>
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
  const [metadata, setMetadata] = useState({ title: 'Shadows', artist: 'Gunship', album: 'Dark All Day' })
  const writeTimer = useRef<number | null>(null)

  const matchingSongs = SONGS.filter((song) => {
    const query = search.trim().toLowerCase()
    return query.length === 0 || `${song.title} ${song.artist} ${song.album}`.toLowerCase().includes(query)
  })
  const visibleSongs = matchingSongs.slice(0, 8)
  const selectedSong = SONGS.find((song) => song.id === selectedSongId) ?? SONGS[0]

  const acceptFile = (file?: File) => {
    if (!file || (!file.name.toLowerCase().endsWith('.mp3') && file.type !== 'audio/mpeg')) return
    const title = file.name.replace(/\.mp3$/i, '').replace(/[_-]+/g, ' ')
    setUploadedFile(file)
    setMetadata((current) => ({ ...current, title }))
    setWriteStatus('idle')
  }

  const handleFileInput = (event: ChangeEvent<HTMLInputElement>) => acceptFile(event.target.files?.[0])
  const handleDrop = (event: DragEvent<HTMLLabelElement>) => { event.preventDefault(); setIsDragging(false); acceptFile(event.dataTransfer.files[0]) }
  const writeToTag = () => {
    if (writeTimer.current !== null) window.clearTimeout(writeTimer.current)
    setWriteStatus('writing')
    writeTimer.current = window.setTimeout(() => setWriteStatus('complete'), 1800)
  }
  const statusText = writeStatus === 'writing' ? 'Tag Detected - Writing...' : writeStatus === 'complete' ? 'Tag written successfully' : 'Tag Detected - Ready to Write'

  return (
    <main className="grid h-svh min-h-svh w-full grid-cols-[minmax(0,610fr)_34px_minmax(0,775fr)] overflow-hidden bg-[#fff0f2] px-[7px] font-[InterVariable,Inter,ui-sans-serif,system-ui,sans-serif] text-[#202024] antialiased max-[980px]:grid-cols-[minmax(340px,43%)_24px_minmax(0,1fr)] max-[980px]:px-0 max-[760px]:block max-[760px]:h-auto max-[760px]:min-h-screen max-[760px]:overflow-visible">
      {isPlayerOpen ? <SelectedSongPlayer song={selectedSong} isPlaying={isPlaying} onTogglePlaying={() => setIsPlaying((current) => !current)} onBack={() => setIsPlayerOpen(false)} /> : (
        <section className="flex h-[calc(100%-14px)] min-w-0 flex-col border border-[#e7e7ec] bg-white max-[980px]:h-full max-[760px]:min-h-svh" aria-labelledby="library-title">
          <div className="flex min-h-0 flex-1 flex-col px-[23px] pt-6 pb-[14px] max-[980px]:px-4 max-[520px]:px-[13px] max-[520px]:pt-[18px] max-[520px]:pb-3">
            <header className="mb-5 flex items-start justify-between gap-5 max-[520px]:items-center"><h1 className="m-0 text-[26px] leading-[31px] font-bold text-[#202024]" id="library-title">Library of Songs</h1></header>
            <label className="mb-4 flex h-12 items-center gap-[10px] rounded-[10px] border-[1.5px] border-[#ffd5dc] bg-white px-[13px] transition-[border-color,box-shadow] duration-150 focus-within:border-[#fa233b] focus-within:shadow-[0_0_0_3px_rgba(250,35,59,0.1)]"><SearchIcon /><span className="sr-only">Search saved songs</span><input className="w-full min-w-0 -translate-y-px border-0 bg-transparent text-base leading-5 tracking-[0.01em] text-[#202024] outline-0 placeholder:text-[#77777f]" type="search" value={search} onChange={(event) => setSearch(event.target.value)} placeholder="Search saved songs" /></label>

            <div className="grid min-h-0 flex-1 content-start gap-[6px] overflow-y-auto [scrollbar-color:#ffd5dc_transparent] [scrollbar-width:thin]" aria-live="polite">
              {visibleSongs.map((song) => {
                const isSelected = song.id === selectedSongId
                return <button className={`flex h-[60px] min-h-[60px] w-full cursor-pointer items-center gap-3 rounded-xl border-[1.5px] px-[13px] py-[7px] text-left text-inherit transition-[border-color,background-color] duration-150 focus-visible:outline-[3px] focus-visible:outline-offset-1 focus-visible:outline-[#fa233b]/20 ${isSelected ? 'border-[#fa233b] bg-[#fff6f7]' : 'border-[#ffd5dc] bg-white'}`} type="button" key={song.id} onClick={() => setSelectedSongId(song.id)} aria-pressed={isSelected}>
                  <span className={`grid h-11 w-11 shrink-0 place-items-center rounded-[10px] ${isSelected ? 'bg-[#fa233b] text-white' : 'bg-[#fff6f7] text-[#fa233b]'}`}><MusicIcon /></span>
                  <span className="grid min-w-0 flex-1 leading-[1.15]"><strong className="overflow-hidden text-[18px] leading-[22px] font-semibold text-ellipsis whitespace-nowrap text-[#202024]">{song.title}</strong><span className="mt-1 overflow-hidden text-[15px] leading-[18px] text-ellipsis whitespace-nowrap text-[#77777f]">{song.artist}{isSelected ? '' : ` • ${song.album}`}</span></span>
                </button>
              })}
              {visibleSongs.length === 0 && <div className="rounded-[10px] border border-dashed border-[#ffd5dc] px-[18px] py-9 text-center text-[#77777f]">No saved songs match “{search}”.</div>}
            </div>
          </div>
          <footer className="flex min-h-[68px] items-center justify-between gap-5 border-t border-[#ffd5dc] bg-[#fff6f7] px-[23px] py-2 max-[980px]:px-4 max-[520px]:px-[14px]"><span className="-translate-y-[5px] text-sm text-[#77777f]">{visibleSongs.length} of {matchingSongs.length} songs shown</span><button className="h-11 w-[90px] -translate-y-[5px] cursor-pointer rounded-[10px] border-0 bg-[#fa233b] p-0 text-xl font-semibold text-white" type="button" onClick={() => { setIsPlayerOpen(true); setIsPlaying(true) }}>Select</button></footer>
        </section>
      )}

      <div className="bg-[#ffd5dc] max-[760px]:h-[18px]" aria-hidden="true" />

      <section className="min-w-0 overflow-auto border border-[#e7e7ec] bg-white px-7 pt-1 pb-[22px] max-[980px]:px-[22px] max-[760px]:min-h-svh max-[760px]:px-5 max-[760px]:py-6" aria-labelledby="writer-title">
        <h1 className="m-0 ml-[-9px] text-[26px] leading-[31px] font-bold text-[#202024]" id="writer-title">NFC Tag Manager &amp; MP3 Writer</h1>
        <div className="mx-auto mt-[37px] mb-5 grid h-[150px] w-[263px] place-items-center rounded-xl border-[2.5px] border-[#fa233b] bg-[#fff6f7] shadow-[0_5px_20px_rgba(250,35,59,0.1255)] max-[520px]:w-[min(262px,80%)]" aria-label="NFC reader connected"><NfcIcon /></div>
        <p className="mb-[19px] flex items-center justify-center text-xl leading-6 font-semibold text-[#278447]">NFC Reader Connected - Ready</p>
        <h2 className="mb-[14px] ml-[-2px] text-[26px] leading-[31px] font-bold text-[#202024]">MP3 File Upload</h2>
        <label className={`flex h-[187px] min-h-[187px] cursor-pointer flex-col items-center justify-center gap-[9px] rounded-xl border-2 border-dashed border-[#fa233b] p-0 text-[#fa233b] transition-[background-color,transform] duration-150 hover:-translate-y-px hover:bg-[#ffeef1] ${isDragging ? '-translate-y-px bg-[#ffeef1]' : 'bg-[#fff6f7]'}`} onDragEnter={(event) => { event.preventDefault(); setIsDragging(true) }} onDragOver={(event) => event.preventDefault()} onDragLeave={() => setIsDragging(false)} onDrop={handleDrop}>
          <input className="absolute h-px w-px overflow-hidden opacity-0" type="file" accept=".mp3,audio/mpeg" onChange={handleFileInput} /><FileMusicIcon />
          <span className="flex h-[77px] w-[317px] flex-col items-center gap-px"><strong className="w-max max-w-none overflow-visible text-[22px] leading-[27px] font-bold whitespace-nowrap">{uploadedFile ? uploadedFile.name : 'DRAG & DROP MP3 FILE HERE'}</strong><span className="text-xl leading-6 text-[#77777f]">or</span><em className="text-xl leading-6 not-italic">[Browse Files]</em></span>
        </label>

        <div className="mt-[26px] mb-[31px] grid grid-cols-3 gap-[26px] max-[980px]:gap-3 max-[520px]:mb-[22px] max-[520px]:grid-cols-1">
          {(['title', 'artist', 'album'] as const).map((field) => <label className="grid min-w-0 gap-[10px]" key={field}><span className="text-[21px] leading-[25px] font-medium text-[#202024]">{field[0].toUpperCase() + field.slice(1)}</span><input className="h-[47px] w-full rounded-[10px] border-[1.5px] border-[#ffd5dc] bg-white px-[13px] text-xl text-[#202024] outline-0 transition-[border-color,box-shadow] duration-150 focus:border-[#fa233b] focus:shadow-[0_0_0_3px_rgba(250,35,59,0.1)]" value={metadata[field]} onChange={(event) => setMetadata({ ...metadata, [field]: event.target.value })} /></label>)}
        </div>

        <button className="flex h-[104px] min-h-[104px] w-full cursor-pointer items-center justify-center gap-[10px] rounded-[10px] border-0 bg-[#fa233b] text-[26px] font-bold text-white transition-[background-color,transform,box-shadow] duration-150 hover:-translate-y-px hover:bg-[#e91d34] hover:shadow-[0_10px_22px_rgba(250,35,59,0.25)] max-[520px]:h-[76px] max-[520px]:min-h-[76px] max-[520px]:text-[21px]" type="button" onClick={writeToTag}><NfcIcon compact /><span>WRITE TO NFC TAG</span></button>
        <p className={`mt-3 flex h-[25px] min-h-[25px] items-center justify-center gap-[9px] text-xl leading-6 ${writeStatus === 'complete' ? 'font-semibold text-[#278447]' : 'text-[#88888f]'}`} role="status"><WritingSpinner complete={writeStatus === 'complete'} /><span className={writeStatus === 'writing' ? 'w-[231px] shrink-0' : ''}>{statusText}</span></p>
      </section>
    </main>
  )
}

export default App
