# Lossy
Similarly to what was done in lossless, I started by investigating what types of lossy algorithms exist and had 2 that caught my eye `Opus Codec` and `Mp3 Codec`, which probably most of us are familiar. There are also others, but I decided to proceed with the Opus Codec.  

I decided to continue my testing with a music to get a better understanding on how these types of algorithms work.
___

## Description
Because of my previous results, C++ was now my go to language for compression and so I created a script that would turn my original music from a .wav file to different .opus files, while testing different quilobits per second.  
1. This change in quilobits per second would translate in the final result to the human ear, creating a perfect environment for subjective testing. With this, and comparing to the original music, I was able to hear that:
    - at `128` quilobits per second, with a 91% compression rate, that it didn't have any perceptible changes, at least to me.  
    - At `64` it almost sounds like the original, but not so good.  
    - At `32` the sound quality had noticeably decreased.  
    - At `8`, it was audible but not enjoyable anymore.  
    - At `1` quilobits per second it basically was just noise.
  
2. After my subjective testing I proceeded with a technical analysis using MATLAB. Where I compared my original file vs mainly the opus ones being 8, 32 and 128 quilobits per second, in order to understand better how each of these behaves while compressing. For this objective analysis, I compared both original vs. compressed files by plotting:
    - Signal Amplitude
    - Spectrogram  
    - Fourier Transform  

3. **RESULTS:** 
    - In the 8 quilobits per second the amplitude of the compressed signal was lower than the original which means that we had some cuts in the frequency band. In the spectrogram we were able to see a significant cut in the frequencies and that became clear whit the Fourier Transform visualization, where there was low passage filter around the 4 quilohertz frequency.  
    - At 32 quilobits we saw basically the same but with a higher low passage filter.
    - And with 128 we observed way more from our compressed file and that reflects in a better sound quality compared to the other ones.
