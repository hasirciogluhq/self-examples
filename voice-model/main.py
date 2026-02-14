from kokoro import KPipeline
from IPython.display import display, Audio
import soundfile as sf
import torch
pipeline = KPipeline(lang_code='a')
text = '''
Ladies end gentlemen's! Welcome back to our new reality show!
'''
generator1 = pipeline(text, voice='af_heart')
generator2 = pipeline(text, voice='af_heart')
generator3 = pipeline(text, voice='af_heart')
generator4 = pipeline(text, voice='af_heart')
for i, (gs, ps, audio) in enumerate(generator1):
    print(i, gs, ps)
    display(Audio(data=audio, rate=24000, autoplay=i==0))
    sf.write(f'1{i}.wav', audio, 30000)