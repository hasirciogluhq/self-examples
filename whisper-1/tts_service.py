import subprocess
import tempfile
from pathlib import Path

MODEL_PATH = Path("models/tts/tr_TR-fahrettin-medium.onnx")

def tts(text: str) -> bytes:
    """
    Piper TTS ile text → WAV memory bytes
    """
    with tempfile.NamedTemporaryFile(suffix=".wav", prefix="piper_", delete=True) as tmpfile:
        temp_wav_path = Path(tmpfile.name)

        subprocess.run([
            "piper",
            "--model", str(MODEL_PATH),
            "--text", text,
            "--output_file", str(temp_wav_path)
        ], check=True)

        wav_bytes = temp_wav_path.read_bytes()

    return wav_bytes
