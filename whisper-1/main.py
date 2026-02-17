import uvicorn

from fastapi import FastAPI, UploadFile, File, Form
from fastapi.responses import StreamingResponse, JSONResponse
import io

from whisper_service import transcribe
from tts_service import tts

app = FastAPI(title="Offline Whisper + Piper API")

@app.get("/")
async def root():
    return {"message": "Offline Whisper + Piper API"}

@app.post("/transcribe")
async def transcribe_audio(file: UploadFile = File(...)):
    try:
        audio_bytes = await file.read()
        text = transcribe(audio_bytes)
        return JSONResponse({"transcription": text})
    except Exception as e:
        return JSONResponse({"error": str(e)}, status_code=500)

@app.post("/speak")
async def speak(text: str = Form(...)):
    """
    Text → Speech
    """
    try:
        wav_bytes = tts(text)
        bio = io.BytesIO(wav_bytes)
        bio.seek(0)
        return StreamingResponse(bio, media_type="audio/wav")
    except Exception as e:
        return JSONResponse({"error": str(e)}, status_code=500)
    
    
    
if __name__ == "__main__":
    uvicorn.run("main:app", host="0.0.0.0", port=8000, reload=True)