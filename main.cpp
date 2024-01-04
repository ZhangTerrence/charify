#include <sys/ioctl.h>
#include <sys/stat.h>
#include <unistd.h>

#include <chrono>
#include <filesystem>
#include <iostream>
#include <opencv2/opencv.hpp>
#include <thread>
#include <vector>

using namespace std;
using namespace cv;
using namespace filesystem;

template <typename T>
T &custom_min(T &a, T &b) {
    return a < b ? &a : &b;
}

string pixelToASCII(int pixel_intensity) {
    // const string ASCII_CHARS = "@$%#&!*+=-_.                ";
    //    const string ASCII_CHARS = "@#&!*+=-_.     ";
    const string ASCII_CHARS = "   ._-=+*!&#%$@";
    string s =
        string(1, ASCII_CHARS[pixel_intensity * ASCII_CHARS.length() / 256]);
    return s;
}

int get_brightness(Vec3b pixel) {
    return (pixel[0] + pixel[1] + pixel[2]) / 3;
}

int main(int argc, char **argv) {
    if (argc != 2) {
        cerr << "Usage: ./charify <FILENAME>\n";
        return 1;
    }

    string working_dir = filesystem::current_path();
    working_dir = working_dir.substr(0, working_dir.find_last_of("/\\"));
    string file = working_dir + "/media/" + (string)argv[1];
    struct stat sb;

    if (stat(&file[0], &sb) != 0) {
        cerr << "Error: File " << file << " cannot be found in media folder.\n";
        return 1;
    }

    VideoCapture video(file);

    double frame_width = video.get(CAP_PROP_FRAME_WIDTH);
    double frame_height = video.get(CAP_PROP_FRAME_HEIGHT);

    struct winsize window;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &window);

    double ratio = min((double)frame_width / frame_height, (double)frame_height / frame_width);
    double display_width = window.ws_col;
    double display_height = window.ws_row;

    if (display_width < 75 || display_height < 25) {
        cerr << "Error: Please use a larger terminal.\n";
        return 1;
    }

    display_width = display_width * 0.95;
    display_height = display_height * 0.95;
    int blank_width = display_width * 0.05 / 2;
    int blank_height = display_height * 0.05 / 2;

    cout << window.ws_col << " " << window.ws_row << endl;
    // cout << display_width << " " << display_height << endl;

    while (frame_width > display_width) {
        // cout << frame_width << endl;
        frame_width -= 1;
    }
    while (frame_height > display_height) {
        // cout << frame_height << endl;
        frame_height -= 1;
    }

    double min_dim = min(display_width, display_height);
    double *smaller_dim;
    double *larger_dim;
    if (frame_width <= frame_height) {
        smaller_dim = &frame_width;
        larger_dim = &frame_height;
    } else {
        smaller_dim = &frame_height;
        larger_dim = &frame_width;
    }

    while (*smaller_dim > min_dim) {
        double correct_dim = *larger_dim * ratio;
        cout << correct_dim << " " << *smaller_dim << " " << *larger_dim << endl;
        while (*smaller_dim < correct_dim) {
            if (*smaller_dim < correct_dim) {
                *smaller_dim += 1;
            } else {
                *smaller_dim -= 1;
            }
        }
    }

    cout << frame_width << " " << frame_height << endl;
    double fps = video.get(CAP_PROP_FPS);
    int frame_duration_ms = 1000 / fps;
    Mat frame, gray_frame, resized_frame;
    // cout << frame_width << " " << frame_height << endl;

    while (true) {
        video >> frame;
        if (frame.empty())
            break;

        cv::cvtColor(frame, gray_frame, cv::COLOR_BGR2GRAY);

        resize(gray_frame, resized_frame, Size(frame_width, frame_height), 0, 0, INTER_LINEAR);
        cout << frame_width << " " << frame_height << endl;

        string ascii_frame;
        for (int i = 0; i < frame_height; i++) {
            for (int j = 0; j < frame_width; j++) {
                if (j <= blank_width + 3 || i <= blank_height) {
                    ascii_frame += " ";
                } else if (j > blank_width + frame_width || i > blank_height + frame_height) {
                    ascii_frame += " ";
                } else {
                    ascii_frame += pixelToASCII(resized_frame.at<uchar>(i, j));
                }
            }
            ascii_frame += "\n";
        }

        system("clear");
        cout << ascii_frame;
        std::this_thread::sleep_for(std::chrono::milliseconds(frame_duration_ms));
    }

    return 0;
}