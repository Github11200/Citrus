# Citrus 🍋
A Tetris game written in C (Cetris -> Citrus) using just the standard library. It's meant to run in your terminal and uses ASCII and ANSI characters to display the blocks.

![image](https://github.com/user-attachments/assets/3d3b9a9d-5344-4322-b56d-1056e31c98a9)

## How to run
1. Make sure you're running on either Linux or are using Windows Subsystem for Linux (WSL) because anything other than Linux is bad.
2. Make sure you have gcc installed on Linux (you may or may not need to do this, try out step 3 first!). Good luck installing it 🫡.
3. Clone the repository and cd into it. Then simply type "./citrus".
4. It should hopefully work and not die.

## Customizability 🛠️
There isn't much you can change. If you really want to though, you can open the `citrus.c` file and change the `HEIGHT` and/or `WIDTH` varibles of the game. You can also change the `MAIN_THREAD_SLEEP_TIME` and `INPUT_THREAD_SLEEP_TIME` variables to change how quickly it refreshes. Try not to crash it!

## Other random stuff
Why did I make this?
1. I wanted to actually make something in C because I've only really done web development and the only time I've had to use C/C++ is in robotics and DSA.
2. I saw a coding montage in the movie Tetris (awesome movie) and I was like, "hmm, it can't be that hard." It really isn't hard either, it just takes me a bit of time to think :).
3. It's for my Computer Programming 11 course because I had nothing else to do.

Why didn't I use any libraries?
1. I'm too lazy to actually install them.
2. Web development is all about putting together like 50 different libraries, I wanted to have a go at coding something without that.
3. I felt like it.

Why did I choose C?

I've always been using objects because every single other language I've programmed in has objects. Then I was like, "hmm, what if OOP is just dumb?" To test this hypothesis, and force myself to not use objects, I wrote it in C.

Why is everything in one file?

I thought about splitting stuff into seperate files, then I was like, "if I put everything into one file, it can't be that bad right?" Then I just ended up using one file. Plus it's such a small project there's no point in splitting the files unless you want your code "organized."
