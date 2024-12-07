#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <termios.h>
#include <fcntl.h>
#include <sys/select.h>

// 設置信號處理器
void handle_signal(int sig) {
    printf("\nReceived signal %d. Exiting...\n", sig);
    exit(0);
}

// 設置終端為非阻塞模式
void set_non_blocking_terminal() {
    struct termios t;
    tcgetattr(STDIN_FILENO, &t);
    t.c_lflag &= ~ICANON;  // 禁用規範模式
    t.c_lflag &= ~ECHO;    // 禁用回顯
    t.c_cc[VMIN] = 1;      // 最少讀取 1 字符
    t.c_cc[VTIME] = 0;     // 不設超時
    tcsetattr(STDIN_FILENO, TCSANOW, &t);
}

// 讀取一個字符並檢查是否是 Esc
int read_char() {
    unsigned char ch;
    if (read(STDIN_FILENO, &ch, 1) == 1) {
        return ch;
    }
    return -1;
}

int main() {
    // 設置信號處理器
    signal(SIGINT, handle_signal);  // 可以使用 SIGINT 或自定義信號
    set_non_blocking_terminal();   // 設置非阻塞模式

    fd_set read_fds;
    struct timeval timeout;

    printf("Press 'Esc' to terminate the program...\n");

    while (1) {
        FD_ZERO(&read_fds);
        FD_SET(STDIN_FILENO, &read_fds);

        // 設定 1 秒的超時，這樣 select 可以每秒檢查一次
        timeout.tv_sec = 1;
        timeout.tv_usec = 0;

        // 使用 select 等待輸入或超時
        int ret = select(STDIN_FILENO + 1, &read_fds, NULL, NULL, &timeout);

        if (ret > 0 && FD_ISSET(STDIN_FILENO, &read_fds)) {
            int ch = read_char();
            if (ch == 27) {  // Esc 鍵的 ASCII 值是 27
                raise(SIGINT);  // 發送 SIGINT 信號
            } else {
                // 正常的資料輸入
                printf("You entered: ");
                char input[100];
                scanf("%99[^\n]", input);  // 讀取一行文字
                printf("Input received: %s\n", input);
            }
        } else {
            // 如果沒有按鍵被按下，可以繼續其他操作
            continue;
        }
    }

    return 0;
}
