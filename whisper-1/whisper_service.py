import whisper
import tempfile
from pathlib import Path
import subprocess

model = whisper.load_model("small")

def transcribe(audio_bytes: bytes, language: str = "tr") -> str:
    """
    Tüm yaygın formatları destekler: wav, mp3, m4a, m4d
    BytesIO → geçici dosya → ffmpeg → Whisper
    """
    with tempfile.NamedTemporaryFile(suffix=".tmp_audio", delete=True) as tmp:
        tmp.write(audio_bytes)
        tmp.flush()

        with tempfile.NamedTemporaryFile(suffix=".wav", delete=True) as tmp_wav:
            tmp_wav_path = Path(tmp_wav.name)
            subprocess.run(
                ["ffmpeg", "-y", "-i", tmp.name, "-ar", "16000", "-ac", "1", str(tmp_wav_path)],
                stdout=subprocess.PIPE,
                stderr=subprocess.PIPE,
                check=True
            )

            result = model.transcribe(str(tmp_wav_path), language=language)
            return result["text"]
