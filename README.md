## Lossless & Lossy
&ensp; This repository explores data compression techniques, both lossless and lossy, with a focus on their application in telemetry systems.
___
### Description
&ensp; Data comes in various shapes and sizes. Imagine this: It can range from something as small as a grocery list to massive databases, like AI data centers that train popular models such as GPT or Llama. Even a sound wave is simply data.  

&ensp; So, now let’s shift our focus to the goals of this work. The aim was to investigate lossy and lossless data compression algorithms, evaluate their performance in different scenarios, and identify which approaches are the most efficient for different sets of data. This is particularly important in areas like telemetry, where efficient data compression can reduce storage requirements, speed up data transmission, and cut operational costs. Ultimately, the goal is to optimize telemetry systems, making them faster and more efficient.  
___
### Analogy
&ensp;  To make it easier to understand, I came up with this analogy. If we take the example of a letter, like the ones children send to Santa Claus on Christmas:
- **[Lossless](https://github.com/t-ros/AST-Data-Compression/tree/main/Lossless),** is like writing a wishlist and then fold the letter to fit in a smaller envelope. If we do this, Santa will still be able to read everything that was written, he simply has to unfold it. Technically speaking, this has the same meaning as compressing and decompressing a file or folder to suit our needs without losing data.

- **[Lossy](https://github.com/t-ros/AST-Data-Compression/tree/main/Lossy)**, is like making a wishlist with 5 toys but when we are about to send it, our parents say we can only wish for 1 toy. So, now we have to cut out toys that we don't really need and the one that's left is the one that's most important. So, technically, removing information like this will make it shorter but it will also make it impossible to recover what was erased, leaving us only with the most important information.
___
### License
This project is licensed under [MIT License](https://github.com/t-ros/AST-Data-Compression/blob/main/LICENSE).
