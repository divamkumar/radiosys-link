#include <iostream>
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>

int main() {
    // Open Serial Port (Pi 4 uses /dev/ttyAMA0 or serial0)
    int fd = open("/dev/serial0", O_RDWR | O_NOCTTY | O_NDELAY);
    if (fd == -1) { std::cerr << "Error opening serial\n"; return -1; }

    struct termios opts;
    tcgetattr(fd, &opts);
    cfsetispeed(&opts, B115200);
    cfsetospeed(&opts, B115200);
    // 8N1 Configuration
    opts.c_cflag &= ~PARENB;
    opts.c_cflag &= ~CSTOPB;
    opts.c_cflag &= ~CSIZE;
    opts.c_cflag |= CS8;
    opts.c_cflag |= (CLOCAL | CREAD);
    tcsetattr(fd, TCSANOW, &opts);

    std::cout << "RadioSys Telemetry Daemon Running..." << std::endl;

    unsigned char buf[256];
    while (true) {
        int n = read(fd, buf, sizeof(buf));
        if (n > 0) {
            // Simple visualization of the protocol
            if(buf[0] == 0xAA) {
                std::cout << "[RX] Sync Byte Received. Payload Len: " << (int)buf[2] << std::endl;
            }
        }
        usleep(50000); 
    }
    return 0;
}

