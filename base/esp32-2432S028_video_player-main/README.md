# Cheap Yellow Display Video Player (ESP32-2432S028)

<a href="https://www.buymeacoffee.com/thelastoutpostworkshop" target="_blank">
<img src="https://www.buymeacoffee.com/assets/img/custom_images/orange_img.png" alt="Buy Me A Coffee">
</a>

## Youtube Tutorial
>⚠️ Make sure you have the board model ESP32-2432S028 with the **ILI9341 Display controller** — not the ST7789 (parallel) controller, which isn't supported by the graphic library [GFX Library for Arduino](https://github.com/moononournation/Arduino_GFX)

[<img src="https://github.com/thelastoutpostworkshop/images/blob/main/Cheay%20Yellow%20Display-3.png" width="500">](https://youtu.be/jYcxUgxz9ks)

## Project Description
This project turns the ESP32-2432S028 "Cheap Yellow Display" with the **ILI9341** controller into a standalone SD-card video player. The sketch scans a `/mjpeg` folder on the SD card, loads Motion JPEG (`.mjpeg`) files, and plays them back full-screen on the built-in 240x320 display. It is designed for simple offline playback with minimal setup: copy converted MJPEG files to the card, power the board, and the videos play in sequence on a loop.

## Features
- Plays `.mjpeg` videos directly from the SD card on the ESP32-2432S028
- Automatically scans the `/mjpeg` folder and builds a playlist from the files it finds
- Loops through all detected videos continuously
- Lets the user skip to the next video with the onboard `BOOT` button
- Uses the Arduino GFX and JPEGDEC libraries for MJPEG decoding and display output
- Initializes the display and SD card independently for reliable playback on the CYD hardware
- Prints playback statistics to the Serial Monitor, including frame count, average FPS, timing breakdown, and detected video size
- Includes sample SD card content and FFmpeg commands for preparing compatible video files

## What You Can Customize
- `DISPLAY_SPI_SPEED`: switch between `80000000L` and `40000000L` if your display is unstable at 80 MHz
- `BL_PIN`: change the display backlight pin for board variants that use a different backlight connection
- `BOOT_PIN` and `BOOT_BUTTON_DEBOUCE_TIME`: remap or adjust the skip button behavior
- `SD_CS`, `SD_MISO`, `SD_MOSI`, `SD_SCK`, and `SD_SPI_SPEED`: adapt SD card wiring or bus speed if needed
- `MJPEG_FOLDER`: point the sketch to a different folder name on the SD card
- `MAX_FILES`: increase or decrease the maximum number of videos loaded into the playlist
- `gfx->setRotation(0)`: change screen orientation to match your enclosure or video layout
- `gfx->invertDisplay(true)`: enable inversion for CYD variants that require it
- FFmpeg options such as `fps`, `scale`, `transpose`, `q:v`, and `eq=brightness`: tune video orientation, resolution, quality, frame rate, and brightness before copying files to the SD card


## Notes
> Some model of Cheap Yellow Display works only at speed of 40Mhz, change the DISPLAY_SPI_SPEED to 40000000L:
```cmd
#define DISPLAY_SPI_SPEED 40000000L // 40MHz 
```
## 🎬 How to Use These FFmpeg Commands

Each of the following commands generates a `.mjpeg` file — a Motion JPEG video format — from an input `.mp4` or `.mov` video, optimized for use in frame-by-frame playback with an SD card reader.

Make sure you have [FFmpeg](https://ffmpeg.org/download.html) installed and accessible from your terminal or command prompt.

---

## Convert video of 16:9 (horizontal) to aspect ratio to 4:3
```cmd
ffmpeg -y -i input.mp4 -pix_fmt yuvj420p -q:v 7 -vf "transpose=1,fps=24,scale=-1:320:flags=lanczos" output.mjpeg
```

## Convert video of 9:16 (vertical) to aspect ratio to 3:4
```cmd
ffmpeg -y -i input.mp4 -pix_fmt yuvj420p -q:v 7 -vf "fps=24,scale=-1:320:flags=lanczos" output.mjpeg
```
## Command for a horizontal video already of aspect ratio 4:3
```cmd
ffmpeg -y -i cropped_4x3.mp4 -pix_fmt yuvj420p -q:v 7 -vf "transpose=1,fps=24,scale=240:320:flags=lanczos" final_240x320.mjpeg
```

## Command for a vertical video already of aspect ratio 3:4
```cmd
ffmpeg -y -i cropped.mp4 -pix_fmt yuvj420p -q:v 7 -vf "fps=24,scale=240:320:flags=lanczos" scaled.mjpeg
```

## Example To reduce brightness of the output video (helps with bright colors)
see [issue 7](https://github.com/thelastoutpostworkshop/esp32-2432S028_video_player/issues/7)
```cmd
ffmpeg -y -i "input.mp4" -q:v 6 -filter:v "scale=-1:ih/2,eq=brightness=-0.05" -c:v mjpeg -an "output.mjpeg"
```

### Options explained
- -pix_fmt yuvj420p: Ensures JPEG-compatible pixel format
- -q:v 7: Controls image quality (lower is better; 1 = best, 31 = worst)
- -vf: Specifies the video filters:
- fps=24: Extracts 24 frames per second
- scale: Resizes the video
- transpose=1: Rotates the video 90° clockwise
- eq: Applies an equalizer filter to slightly darken the video. Values range from -1.0 to 1.0.
- .mjpeg: Output format used when streaming or storing a series of JPEG frames as a video
