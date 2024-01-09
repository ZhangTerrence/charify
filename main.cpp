#include <sys/ioctl.h>
#include <sys/stat.h>
#include <unistd.h>

#include <chrono>
#include <filesystem>
#include <opencv2/opencv.hpp>
#include <thread>

using namespace std;

bool isVideo(string filetype) {
    if (filetype == ".mp4" || filetype == ".mp3" || filetype == ".avi") {
        return true;
    }
    return false;
}

bool isImage(string filetype) {
    if (filetype == ".png" || filetype == ".jpg" || filetype == ".jpeg" || filetype == ".webp") {
        return true;
    }
    return false;
}

char translatePixel(int pixel_brightness) {
    string ascii_chars = "        `.-':_,^=;><+/!rc*#$Bg0MNWQ%&@";
    char ascii = ascii_chars[pixel_brightness * ascii_chars.length() / 256];
    return ascii;
}

int main(int argc, char **argv) {
    if (argc != 2) {
        cerr << "Usage: ./charify <FILE>\n";
        return 1;
    }

    string working_dir = filesystem::current_path();
    working_dir = working_dir.substr(0, working_dir.find_last_of("/\\"));
    string filename = argv[1];
    string filetype = filename.substr(filename.find_first_of("."));
    string filepath = working_dir + "/media/" + filename;
    struct stat sb;

    if (stat(&filepath[0], &sb) != 0) {
        cerr << "Error: File " << argv[1] << " cannot be found in media folder.\n";
        return 1;
    }

    struct winsize window;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &window);

    int display_width = window.ws_col;
    int display_height = window.ws_row;

    if (display_width < 75 || display_height < 25) {
        cerr << "Error: Please use a larger terminal. Minimum resolution is width 75 and height 25.\n";
        return 1;
    }

    if (isVideo(filetype)) {
        cv::VideoCapture video(filepath);

        double fps = video.get(cv::CAP_PROP_FPS);
        int frame_duration_ms = 1000 / fps;

        cv::Mat frame, gray_frame, resized_frame;

        while (true) {
            video >> frame;

            if (frame.empty()) {
                break;
            }

            cv::cvtColor(frame, gray_frame, cv::COLOR_BGR2GRAY);
            cv::resize(gray_frame, resized_frame, cv::Size(display_width, display_height), 0, 0, cv::INTER_LINEAR);

            string ascii_frame;
            for (int i = 0; i < display_height; i++) {
                for (int j = 0; j < display_width; j++) {
                    ascii_frame += translatePixel(resized_frame.at<uchar>(i, j));
                }
                ascii_frame += "\n";
            }

            system("clear");
            cout << ascii_frame;
            std::this_thread::sleep_for(std::chrono::milliseconds(frame_duration_ms));
        }
    } else if (isImage(filetype)) {
        cv::Mat image = cv::imread(filepath);

        cv::Mat gray_image, resized_image;

        cv::cvtColor(image, gray_image, cv::COLOR_BGR2GRAY);
        cv::resize(gray_image, resized_image, cv::Size(display_width, display_height), 0, 0, cv::INTER_LINEAR);

        string ascii_frame;
        for (int i = 0; i < display_height; i++) {
            for (int j = 0; j < display_width; j++) {
                ascii_frame += translatePixel(resized_image.at<uchar>(i, j));
            }
            ascii_frame += "\n";
        }

        cout << ascii_frame;
    } else {
        cerr << "Error: Invalid filetype.\n";
        return 1;
    }

    return 0;
}