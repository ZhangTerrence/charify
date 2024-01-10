# Charify

C++ implementation of a media to ascii converter. Takes input from either a video or image and generates an ascii representation of the content. This project utilizes OpenCV's library to read the videos and images.  

## Usage  

### Prerequisities  

In addition to a modern C++ compiler like GCC or CLang, this project requires the following libraries:  

- [CMake](https://cmake.org/) (>= 3.1)
- [OpenCV](https://opencv.org/) (>= 4.0)

### Building and Running

```bash
❯ mkdir ~/workspace
❯ cd ~/workspace
❯ git clone https://github.com/ZhangTerrence/charify
❯ cd charify
❯ rm -rf build 
❯ mkdir build
❯ cd build
❯ cmake ..
❯ make 
```

1. If it doesn't already exist, create a media directory.
2. Download and place video or image inside the media directory.
3. Run in the terminal: </br>
    $ ./charify \<FILE>

## License

This project is licensed under the MIT License.
